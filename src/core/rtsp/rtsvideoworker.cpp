#include "rtsvideoworker.h"
#include <QDebug>
#include <QMutexLocker>
#include <QtConcurrent>
#include <QElapsedTimer>

// Register QImage for meta-type system
Q_DECLARE_METATYPE(QImage)

namespace {
int registerQImage() {
    qRegisterMetaType<QImage>("QImage");
    return 0;
}
static int registered = registerQImage();
}

RTSVideoWorker::RTSVideoWorker(QObject *parent)
    : QObject(parent)
{
    qDebug() << "Worker created on thread:" << QThread::currentThread();
    m_fpsTimer.start();
    // Pre-allocate buffer to avoid frequent reallocations
    m_reusableBuffer.reserve(65536); // 64KB initial buffer
}

RTSVideoWorker::~RTSVideoWorker()
{
    stop();
}

void RTSVideoWorker::setProcessingEnabled(bool enabled)
{
    m_config.enabled = enabled;
}

void RTSVideoWorker::setTargetSize(int width, int height)
{
    m_config.targetWidth = width;
    m_config.targetHeight = height;
}

void RTSVideoWorker::setOverlayText(const QString &text)
{
    m_config.overlayText = text;
}

void RTSVideoWorker::setOverlayEnabled(bool enabled)
{
    m_config.overlayEnabled = enabled;
}

void RTSVideoWorker::start(const QString &url)
{
    qDebug() << "Worker start called on thread:" << QThread::currentThread() << "URL:" << url;

    if (url.isEmpty() || m_running.loadRelaxed()) {
        qDebug() << "Worker already running or URL empty";
        return;
    }

    m_url = url;
    m_running = 1;
    m_buffer.clear();
    m_frameCount = 0;
    m_fpsTimer.restart();

    m_nam = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setUrl(QUrl(m_url));
    request.setRawHeader("User-Agent", "ESP32-CAM-Qt-Client/1.0");

    m_reply = m_nam->get(request);
    m_reply->setReadBufferSize(0);

    connect(m_reply, &QNetworkReply::readyRead, this, &RTSVideoWorker::onData);
    connect(m_reply, &QNetworkReply::finished, this, &RTSVideoWorker::onFinished);

    emit started();
    qDebug() << "Worker: Connecting to" << m_url;
}

void RTSVideoWorker::stop()
{
    qDebug() << "Worker stop called";

    m_running = 0;
    clearQueue();

    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    if (m_nam) {
        m_nam->deleteLater();
        m_nam = nullptr;
    }

    // Clear reusable buffer to free memory
    {
        QMutexLocker lock(&m_bufferMutex);
        m_reusableBuffer.clear();
        m_reusableBuffer.squeeze(); // Release memory
    }

    emit stopped();
}

void RTSVideoWorker::onData()
{
    if (!m_running.loadRelaxed()) return;

    QByteArray data = m_reply->readAll();

    // Use reusable buffer to avoid reallocations
    {
        QMutexLocker lock(&m_bufferMutex);
        m_buffer.append(data);
    }

    parseStreamData();
}

void RTSVideoWorker::parseStreamData()
{
    while (m_running.loadRelaxed()) {
        QByteArray *bufferPtr = nullptr;
        {
            QMutexLocker lock(&m_bufferMutex);
            bufferPtr = &m_buffer;
        }

        int start = bufferPtr->indexOf("--frame");
        if (start == -1) break;

        int headerEnd = bufferPtr->indexOf("\r\n\r\n", start);
        if (headerEnd == -1) break;

        int end = bufferPtr->indexOf("--frame", headerEnd + 4);
        if (end == -1) break;

        // Extract JPEG data
        QByteArray jpeg = bufferPtr->mid(headerEnd + 4, end - (headerEnd + 4));

        if (jpeg.endsWith("\r\n")) {
            jpeg.chop(2);
        }

        // Remove processed data from buffer
        bufferPtr->remove(0, end);

        if (jpeg.size() > 0) {
            {
                QMutexLocker lock(&m_queueMutex);
                // Limit queue size to prevent memory bloat
                while (m_jpegQueue.size() >= 3) {
                    // Dequeue old frames to free memory
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

QImage RTSVideoWorker::processFrame(const cv::Mat &rawFrame)
{
    // Step 1: Resize to target dimensions
    cv::Mat processed;
    cv::resize(rawFrame, processed, cv::Size(m_config.targetWidth, m_config.targetHeight));

    // Step 2: Convert BGR to RGB for Qt
    cv::cvtColor(processed, processed, cv::COLOR_BGR2RGB);

    // Step 3: Run detection models (if enabled)
    if (m_config.detectionEnabled) {
        m_detectionCounter++;
        if (m_detectionCounter >= m_config.detectionInterval) {
            m_detectionCounter = 0;
            runDetectionModels(processed);
        }
        drawDetectionResults(processed);
    }

    // Step 4: Apply overlay text
    if (m_config.overlayEnabled) {
        applyOverlay(processed);
    }

    // Step 5: Convert to QImage - IMPORTANT: Use shallow copy then detach
    QImage img(processed.data, processed.cols, processed.rows,
               processed.step, QImage::Format_RGB888);

    // Return a deep copy to ensure the image data survives after processed is destroyed
    // This is necessary for thread safety
    return img.copy();
}

void RTSVideoWorker::applyOverlay(cv::Mat &frame)
{
    std::string overlayText = m_config.overlayText.toStdString();
    if (m_currentFps > 0) {
        overlayText += " | FPS: " + std::to_string(int(m_currentFps));
    }

    cv::putText(frame, overlayText, cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);

    // Add detection results if any
    QMutexLocker lock(&m_detectionMutex);
    int y = 60;
    for (const QString &face : m_lastDetection.faces) {
        cv::putText(frame, face.toStdString(), cv::Point(10, y),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0), 1);
        y += 25;
    }

    if (m_lastDetection.hasQRCode) {
        cv::putText(frame, "QR: " + m_lastDetection.qrCodeData.toStdString(),
                    cv::Point(10, frame.rows - 10),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 1);
    }
}

void RTSVideoWorker::runDetectionModels(cv::Mat &frame)
{
    // ============================================================
    // YOUR DETECTION MODELS GO HERE
    // ============================================================
    //
    // Memory optimization tips:
    // - Reuse cascade classifiers instead of loading each frame
    // - Use static variables for models to avoid reloading
    // - Clear detection results when not needed to free memory
    //
    // Example face detection with memory optimization:
    /*
    static cv::CascadeClassifier faceCascade;
    static bool cascadeLoaded = false;

    if (!cascadeLoaded) {
        cascadeLoaded = faceCascade.load("/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml");
    }

    if (cascadeLoaded) {
        std::vector<cv::Rect> faces;
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);
        faceCascade.detectMultiScale(gray, faces, 1.1, 3, 0, cv::Size(30, 30));

        QMutexLocker lock(&m_detectionMutex);
        m_lastDetection.faces.clear();
        m_lastDetection.faceRects.clear();

        for (const auto &face : faces) {
            m_lastDetection.faces << QString("Face");
            m_lastDetection.faceRects << face;
            cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
        }

        if (!faces.empty()) {
            emit detectionResult(QString("Detected %1 face(s)").arg(faces.size()));
        }
    }
    */

    Q_UNUSED(frame)
}

void RTSVideoWorker::drawDetectionResults(cv::Mat &frame)
{
    QMutexLocker lock(&m_detectionMutex);

    for (const cv::Rect &face : m_lastDetection.faceRects) {
        cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
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

        // Decode JPEG using OpenCV
        cv::Mat raw = cv::imdecode(cv::Mat(1, jpeg.size(), CV_8UC1,
                                           (void*)jpeg.constData()),
                                   cv::IMREAD_COLOR);
        if (raw.empty()) continue;

        // Process the frame through the pipeline
        QImage processedImage = processFrame(raw);

        // Update FPS
        updateFPS();

        // Send to main thread for rendering
        emit frameReady(processedImage);

        // Allow other tasks to run
        QThread::yieldCurrentThread();
    }
    m_decoderRunning = 0;
}

void RTSVideoWorker::onFinished()
{
    qDebug() << "Stream finished. Error:" << (m_reply ? m_reply->errorString() : "No reply");
    if (m_reply && m_reply->error() != QNetworkReply::NoError) {
        emit error(m_reply->errorString());
    }
    stop();
}

void RTSVideoWorker::clearQueue()
{
    QMutexLocker lock(&m_queueMutex);
    m_jpegQueue.clear();

    QMutexLocker bufferLock(&m_bufferMutex);
    m_buffer.clear();
}