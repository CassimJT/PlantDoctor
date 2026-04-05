#include "rtsvideoworker.h"
#include <QDebug>
#include <QThread>
#include <QtConcurrent>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>

using namespace executorch::extension;
using executorch::runtime::EValue;

// ============================================
// DetectionTask Implementation using ExecuTorch
// ============================================
DetectionTask::DetectionTask(std::shared_ptr<executorch::extension::Module> module,
                             cv::Mat frame, cv::Size originalShape,
                             const QStringList& classNames, float confThreshold)
    : QRunnable()
    , m_module(module)
    , m_frame(frame.clone())
    , m_originalShape(originalShape)
    , m_classNames(classNames)
    , m_confThreshold(confThreshold)
    , m_nmsThreshold(0.45f)
    , m_inputSize(640)
{
    setAutoDelete(true);
}

DetectionTask::~DetectionTask()
{
}

void DetectionTask::run()
{
    try {
        if (!m_module || !m_module->is_loaded()) {
            qDebug() << "[DetectionTask] Module not loaded";
            return;
        }

        // Preprocess image (same as InfarenceRunner)
        cv::Mat resized, normalized;
        cv::resize(m_frame, resized, cv::Size(m_inputSize, m_inputSize));
        resized.convertTo(normalized, CV_32FC3, 1.0 / 255.0);

        // Split into channels (CHW format)
        std::vector<cv::Mat> channels(3);
        cv::split(normalized, channels);

        // Create tensor data
        auto data = std::make_shared<std::vector<float>>(1 * 3 * m_inputSize * m_inputSize);

        for (int c = 0; c < 3; ++c) {
            memcpy(data->data() + c * m_inputSize * m_inputSize,
                   channels[c].data,
                   m_inputSize * m_inputSize * sizeof(float));
        }

        std::vector<executorch::aten::SizesType> shape = {1, 3, m_inputSize, m_inputSize};

        auto tensor = from_blob(
            data->data(),
            shape,
            executorch::aten::ScalarType::Float
            );

        if (!tensor) {
            qDebug() << "[DetectionTask] Failed to create tensor";
            return;
        }

        // Run inference (exactly like InfarenceRunner)
        std::vector<EValue> inputs;
        inputs.emplace_back(tensor);

        const auto result = m_module->forward(inputs);

        // Check if inference failed (same as InfarenceRunner)
        if (!result.ok()) {
            qDebug() << "[DetectionTask] Inference failed";
            return;
        }

        // Process output tensor
        const auto& outputTensor = result->at(0).toTensor();
        const float* output = outputTensor.const_data_ptr<float>();
        int numOutputs = outputTensor.numel();

        qDebug() << "[DetectionTask] Num output elements:" << numOutputs;

        // For YOLO detection output format: [batch, num_detections, 6]
        // 6 = [x1, y1, x2, y2, confidence, class_id]
        int numDetections = numOutputs / 6;

        QList<QMap<QString, QVariant>> detections;

        float scaleX = (float)m_originalShape.width / m_inputSize;
        float scaleY = (float)m_originalShape.height / m_inputSize;

        for (int i = 0; i < numDetections; i++) {
            float confidence = output[i * 6 + 4];

            if (confidence >= m_confThreshold) {
                int classId = (int)output[i * 6 + 5];
                float x1 = output[i * 6 + 0] * scaleX;
                float y1 = output[i * 6 + 1] * scaleY;
                float x2 = output[i * 6 + 2] * scaleX;
                float y2 = output[i * 6 + 3] * scaleY;

                QMap<QString, QVariant> detection;
                detection["x1"] = (int)x1;
                detection["y1"] = (int)y1;
                detection["x2"] = (int)x2;
                detection["y2"] = (int)y2;
                detection["label"] = (classId < m_classNames.size()) ?
                                         m_classNames[classId] :
                                         QString("Pest_%1").arg(classId);
                detection["confidence"] = confidence;
                detections.append(detection);
            }
        }

        if (m_callback) {
            m_callback(detections, m_originalShape);
        }

    } catch (const std::exception& e) {
        qDebug() << "[DetectionTask] Error:" << e.what();
    }
}

// ============================================
// RTSVideoWorker Implementation
// ============================================
RTSVideoWorker::RTSVideoWorker(QObject *parent)
    : QObject(parent)
    , m_nam(nullptr)
    , m_reply(nullptr)
    , m_running(0)
    , m_decoderRunning(0)
    , m_processingEnabled(true)
    , m_overlayText("ESP32-CAM")
    , m_frameCount(0)
    , m_currentFps(0.0)
    , m_isModelLoaded(false)
    , m_detectionEnabled(false)
    , m_confThreshold(0.20f)
    , m_nmsThreshold(0.45f)
    , m_detectionInterval(800)
    , m_lastDetectionTime(0)
    , m_detectionInProgress(false)
    , m_lastGoodDetectionTime(0)
{
    qDebug() << "RTSVideoWorker created on thread:" << QThread::currentThread();

    m_threadPool = QThreadPool::globalInstance();
    m_threadPool->setMaxThreadCount(2);

    m_classNames = QStringList()
                   << "Beetle" << "Caterpillar" << "Aphid" << "Grasshopper"
                   << "Mite" << "Weevil" << "Thrips" << "Whitefly"
                   << "Locust" << "Ant" << "Spider" << "Snail";

    m_fpsTimer.start();
    m_lastDetection.clear();

    // Load model on construction (same as InfarenceRunner)
    loadModel();
}

RTSVideoWorker::~RTSVideoWorker()
{
    stop();
}

// ============================================
// Model Loading (Exactly like InfarenceRunner)
// ============================================
QString RTSVideoWorker::prepareModelFile()
{
    QString targetPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
    + "/pest_detection.pte";  // Changed to .pte for ExecuTorch

    if (QFile::exists(targetPath)) {
        qDebug() << "Model already exists at:" << targetPath;
        return targetPath;
    }

#ifdef Q_OS_ANDROID
    QString sourcePath = "assets:/model/pest_detection.pte";
#else
    QString sourcePath = ":/model/pest_detection.pte";
#endif

    QFile source(sourcePath);
    if (!source.exists()) {
        qDebug() << "Model file not found at:" << sourcePath;

#ifndef Q_OS_ANDROID
        QString alternativePath = QCoreApplication::applicationDirPath()
                                  + "/pest_detection.pte";
        source.setFileName(alternativePath);
        if (!source.exists()) {
            qDebug() << "Also not found at:" << alternativePath;
            return {};
        }
#endif
    }

    QDir().mkpath(QFileInfo(targetPath).path());
    if (!source.copy(targetPath)) {
        qDebug() << "Failed to copy model! Error:" << source.errorString();
        return {};
    }

    QFile::setPermissions(targetPath,
                          QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther);

    qDebug() << "Model copied to:" << targetPath;
    return targetPath;
}

void RTSVideoWorker::loadModel()
{
    QString path = prepareModelFile();
    if (path.isEmpty()) {
        qDebug() << "Pest detection model not found!";
        m_isModelLoaded = false;
        emit modelLoadingFailed("Model file not found");
        return;
    }

    try {
        qDebug() << "Loading pest detection model from:" << path;
        m_module.reset(new executorch::extension::Module(path.toStdString()));
        m_isModelLoaded = true;
        qDebug() << "Pest detection model loaded successfully!";
        emit modelloaded();
    } catch (const std::exception &e) {
        qDebug() << "Failed to load pest detection model:" << e.what();
        m_isModelLoaded = false;
        emit modelLoadingFailed(QString("Model load error: %1").arg(e.what()));
    }
}

// ============================================
// Preprocess (Exactly like InfarenceRunner)
// ============================================
TensorPtr RTSVideoWorker::preprocess(const cv::Mat &img)
{
    if (img.empty()) {
        qDebug() << "Empty image for preprocess";
        return nullptr;
    }

    cv::Mat resized, normalized;
    cv::resize(img, resized, cv::Size(MODEL_INPUT_SIZE, MODEL_INPUT_SIZE));
    resized.convertTo(normalized, CV_32FC3, 1.0 / 255.0);

    std::vector<cv::Mat> channels(3);
    cv::split(normalized, channels);

    // Allocate data
    auto data = std::make_shared<std::vector<float>>(1 * 3 * MODEL_INPUT_SIZE * MODEL_INPUT_SIZE);

    for (int c = 0; c < 3; ++c) {
        memcpy(data->data() + c * MODEL_INPUT_SIZE * MODEL_INPUT_SIZE,
               channels[c].data,
               MODEL_INPUT_SIZE * MODEL_INPUT_SIZE * sizeof(float));
    }

    std::vector<executorch::aten::SizesType> shape = {1, 3, MODEL_INPUT_SIZE, MODEL_INPUT_SIZE};

    auto tensor = from_blob(
        data->data(),
        shape,
        executorch::aten::ScalarType::Float
        );

    if (!tensor) {
        qDebug() << "Failed to create tensor";
        return nullptr;
    }

    // Simple memory management - store in static map (same as InfarenceRunner)
    static QMap<TensorPtr, std::shared_ptr<std::vector<float>>> tensorDataMap;
    tensorDataMap[tensor] = data;

    return tensor;
}

// ============================================
// Public Slots
// ============================================
void RTSVideoWorker::setDetectionEnabled(bool enabled)
{
    m_detectionEnabled = enabled;
    qDebug() << "Detection" << (enabled ? "ON" : "OFF");
}

void RTSVideoWorker::setProcessingEnabled(bool enabled)
{
    m_processingEnabled = enabled;
}

void RTSVideoWorker::setOverlayText(const QString &text)
{
    m_overlayText = text;
}

void RTSVideoWorker::start(const QString &url)
{
    qDebug() << "Worker start on thread:" << QThread::currentThread() << "URL:" << url;

    if (url.isEmpty() || m_running.loadRelaxed()) {
        return;
    }

    m_url = url;
    m_running = 1;
    m_buffer.clear();
    m_jpegQueue.clear();
    m_frameCount = 0;
    m_fpsTimer.restart();

    m_nam = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setUrl(QUrl(m_url));
    request.setRawHeader("Connection", "close");
    request.setRawHeader("User-Agent", "ESP32-CAM-Qt-Client/1.0");

    m_reply = m_nam->get(request);
    m_reply->setReadBufferSize(1024);

    connect(m_reply, &QNetworkReply::readyRead, this, &RTSVideoWorker::onData);
    connect(m_reply, &QNetworkReply::finished, this, &RTSVideoWorker::onFinished);

    emit connectionStatusChanged(true);
    qDebug() << "Streaming started to:" << m_url;
}

void RTSVideoWorker::stop()
{
    if (!m_running.loadRelaxed()) return;

    qDebug() << "Worker stop called";
    m_running = 0;

    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    if (m_nam) {
        m_nam->deleteLater();
        m_nam = nullptr;
    }

    {
        QMutexLocker lock(&m_queueMutex);
        m_jpegQueue.clear();
    }
    {
        QMutexLocker lock(&m_bufferMutex);
        m_buffer.clear();
    }

    emit connectionStatusChanged(false);
    qDebug() << "Streaming stopped";
}

void RTSVideoWorker::onData()
{
    if (!m_running.loadRelaxed()) return;

    QByteArray data = m_reply->readAll();

    {
        QMutexLocker lock(&m_bufferMutex);
        if (m_buffer.size() > 32768) {
            m_buffer.clear();
        }
        m_buffer.append(data);
    }

    parseStreamData();
}

void RTSVideoWorker::parseStreamData()
{
    while (m_running.loadRelaxed()) {
        QByteArray buffer;
        {
            QMutexLocker lock(&m_bufferMutex);
            buffer = m_buffer;
        }

        int start = buffer.indexOf("\xff\xd8");
        int end = buffer.indexOf("\xff\xd9");

        if (start == -1 || end == -1) break;

        QByteArray jpeg = buffer.mid(start, end - start + 2);

        {
            QMutexLocker lock(&m_bufferMutex);
            m_buffer.remove(0, end + 2);
        }

        if (!jpeg.isEmpty()) {
            {
                QMutexLocker lock(&m_queueMutex);
                while (m_jpegQueue.size() >= 3) {
                    m_jpegQueue.dequeue();
                }
                m_jpegQueue.enqueue(jpeg);
            }

            if (m_decoderRunning.testAndSetRelaxed(0, 1)) {
                QtConcurrent::run([this]() { decodeLoop(); });
            }
        }
    }
}

void RTSVideoWorker::decodeLoop()
{
    while (m_running.loadRelaxed()) {
        QByteArray jpeg;
        {
            QMutexLocker lock(&m_queueMutex);
            if (m_jpegQueue.isEmpty()) {
                m_decoderRunning = 0;
                return;
            }
            jpeg = m_jpegQueue.dequeue();
        }

        std::vector<uchar> data(jpeg.begin(), jpeg.end());
        cv::Mat raw = cv::imdecode(data, cv::IMREAD_COLOR);

        if (raw.empty()) continue;

        if (m_processingEnabled) {
            QImage processedImage = processFrame(raw);
            updateFPS();
            emit frameReady(processedImage);
        }

        QThread::yieldCurrentThread();
    }
    m_decoderRunning = 0;
}

// ============================================
// Frame Processing with ExecuTorch Detection
// ============================================
QImage RTSVideoWorker::processFrame(const cv::Mat &rawFrame)
{
    cv::Mat frame;
    cv::resize(rawFrame, frame, cv::Size(640, 480));
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

    {
        QMutexLocker lock(&m_frameMutex);
        m_latestFrame = frame.clone();
    }

    if (m_detectionEnabled && m_isModelLoaded && m_module) {
        runDetection(frame);
    }

    applyOverlay(frame);

    return QImage(frame.data, frame.cols, frame.rows,
                  frame.step, QImage::Format_RGB888).copy();
}

void RTSVideoWorker::runDetection(cv::Mat &frame)
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    if (!m_detectionInProgress &&
        (now - m_lastDetectionTime) >= m_detectionInterval &&
        m_isModelLoaded) {

        m_lastDetectionTime = now;
        m_detectionInProgress = true;

        cv::Mat frameCopy;
        {
            QMutexLocker lock(&m_frameMutex);
            if (!m_latestFrame.empty()) {
                frameCopy = m_latestFrame.clone();
            }
        }

        if (!frameCopy.empty()) {
            DetectionTask *task = new DetectionTask(m_module, frameCopy, frame.size(),
                                                    m_classNames, m_confThreshold);

            task->setCallback([this](const QList<QMap<QString, QVariant>>& detections,
                                     const cv::Size& originalShape) {
                detectionCallback(detections, originalShape);
            });

            m_threadPool->start(task);
        }
    }

    drawDetections(frame);
}

void RTSVideoWorker::detectionCallback(const QList<QMap<QString, QVariant>>& detections,
                                       const cv::Size& originalShape)
{
    QMutexLocker lock(&m_detectionMutex);
    m_detectionInProgress = false;

    qint64 now = QDateTime::currentMSecsSinceEpoch();

    if (!detections.isEmpty()) {
        QList<Detection> newDetections;

        for (const auto& det : detections) {
            Detection detection;
            detection.rect = cv::Rect(
                det["x1"].toInt(),
                det["y1"].toInt(),
                det["x2"].toInt() - det["x1"].toInt(),
                det["y2"].toInt() - det["y1"].toInt()
                );
            detection.label = det["label"].toString();
            detection.confidence = det["confidence"].toFloat();
            detection.timestamp = now;
            newDetections.append(detection);
        }

        m_lastDetection = newDetections;
        m_lastGoodDetectionTime = now;

        QString result = QString("Detected %1 pest(s)").arg(detections.size());
        emit detectionResult(result);

        qDebug() << "Detected" << detections.size() << "pests";
    }
}

void RTSVideoWorker::drawDetections(cv::Mat &frame)
{
    QMutexLocker lock(&m_detectionMutex);

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    QList<Detection> activeDetections;

    bool showDetections = (now - m_lastGoodDetectionTime) < PERSISTENCE_TIME;

    if (showDetections) {
        for (const Detection& det : m_lastDetection) {
            cv::rectangle(frame, det.rect, cv::Scalar(0, 255, 0), 2);

            QString text = QString("%1 %2%").arg(det.label).arg(int(det.confidence * 100));
            cv::putText(frame, text.toStdString(),
                        cv::Point(det.rect.x, det.rect.y - 5),
                        cv::FONT_HERSHEY_SIMPLEX, 0.55,
                        cv::Scalar(0, 255, 0), 2);
        }
    }

    int count = showDetections ? m_lastDetection.size() : 0;
    cv::Scalar color = (count > 0) ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
    cv::putText(frame, QString("Pests: %1").arg(count).toStdString(),
                cv::Point(10, 65),
                cv::FONT_HERSHEY_SIMPLEX, 0.8, color, 2);
}

void RTSVideoWorker::applyOverlay(cv::Mat &frame)
{
    cv::putText(frame, m_overlayText.toStdString(),
                cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX,
                0.7, cv::Scalar(0, 255, 0), 2);

    if (m_currentFps > 0) {
        QString fpsText = QString("%1 fps").arg(int(m_currentFps));
        cv::putText(frame, fpsText.toStdString(),
                    cv::Point(frame.cols - 80, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5,
                    cv::Scalar(255, 255, 0), 1);
    }
}

void RTSVideoWorker::updateFPS()
{
    m_frameCount++;
    qint64 elapsed = m_fpsTimer.elapsed();

    if (elapsed >= 1000) {
        m_currentFps = (m_frameCount * 1000.0) / elapsed;
        emit fpsUpdated(m_currentFps);
        m_frameCount = 0;
        m_fpsTimer.restart();
    }
}

void RTSVideoWorker::onFinished()
{
    if (m_reply && m_reply->error() != QNetworkReply::NoError) {
        emit error(m_reply->errorString());
        qDebug() << "Stream error:" << m_reply->errorString();
    }
    stop();
}