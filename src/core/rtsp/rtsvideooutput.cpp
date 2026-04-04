#include "rtsvideooutput.h"
#include "rtsvideoworker.h"
#include <QDebug>
#include <QMutexLocker>
#include <QQuickWindow>
#include <QDir>
#include <QStandardPaths>
#include <QFile>

RTSVideoOutput::RTSVideoOutput(QQuickItem *parent)
    : QQuickItem(parent)
    , m_processingEnabled(true)
    , m_overlayText("ESP32-CAM")
    , m_detectionEnabled(false)
    , m_currentFps(0.0)
    , m_isConnected(false)
    , m_worker(nullptr)
    , m_workerThread(nullptr)
    , m_processing(false)
    , m_cachedTexture(nullptr)
    , m_modelLoaded(false)
{
    setFlag(ItemHasContents, true);
    qDebug() << "RTSVideoOutput created on thread:" << QThread::currentThread();
}

RTSVideoOutput::~RTSVideoOutput()
{
    stopProcessing();
    delete m_cachedTexture;
    if (m_modelLoaderThread.isRunning()) {
        m_modelLoaderThread.quit();
        m_modelLoaderThread.wait();
    }
}

void RTSVideoOutput::componentComplete()
{
    QQuickItem::componentComplete();
    qDebug() << "RTSVideoOutput componentComplete, URL:" << m_rtsUrl;

    // Load the pest detection model internally
    loadModel();

    if (!m_rtsUrl.isEmpty())
        startProcessing();
}

void RTSVideoOutput::loadModel()
{
    qDebug() << "=== Loading pest detection model from assets ===";

    QString modelPath = prepareModelFile();
    if (modelPath.isEmpty()) {
        qDebug() << "ERROR: Failed to prepare model file";
        emit modelLoadingFailed("Model file not found in assets");
        return;
    }

    qDebug() << "Model prepared at:" << modelPath;

    // Create temporary worker to load model
    RTSVideoWorker* tempWorker = new RTSVideoWorker();
    tempWorker->moveToThread(&m_modelLoaderThread);

    connect(&m_modelLoaderThread, &QThread::finished, tempWorker, &QObject::deleteLater);
    connect(tempWorker, &RTSVideoWorker::modelloaded, this, [this, tempWorker]() {
        qDebug() << "Model loaded in temporary worker";
        m_modelLoaded = true;
        emit modelloaded();
        tempWorker->deleteLater();
    });
    connect(tempWorker, &RTSVideoWorker::modelLoadingFailed, this, [this, tempWorker](const QString& error) {
        qDebug() << "Model loading failed:" << error;
        emit modelLoadingFailed(error);
        tempWorker->deleteLater();
    });

    m_modelLoaderThread.start();
    QMetaObject::invokeMethod(tempWorker, "loadModelFromPath",
                              Qt::QueuedConnection,
                              Q_ARG(QString, modelPath));
}

QString RTSVideoOutput::prepareModelFile()
{
    QString targetPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
    + "/pest_detection.pte";

    qDebug() << "Target model path:" << targetPath;

    // Check if already exists
    if (QFile::exists(targetPath)) {
        qDebug() << "Model already exists at:" << targetPath;
        return targetPath;
    }

    // Copy from assets
#ifdef Q_OS_ANDROID
    QString sourcePath = "assets:/model/pest_detection.pte";
#else
    QString sourcePath = ":/model/pest_detection.pte";
#endif

    qDebug() << "Source model path:" << sourcePath;

    QFile source(sourcePath);
    if (!source.exists()) {
        qDebug() << "ERROR: Model file not found at:" << sourcePath;

        // Try alternative paths for Android
#ifdef Q_OS_ANDROID
        QStringList altPaths = {
            "assets:/pest_detection.pte",
            "assets:/assets/model/pest_detection.pte"
        };
        for (const QString& altPath : altPaths) {
            qDebug() << "Trying alternative:" << altPath;
            source.setFileName(altPath);
            if (source.exists()) {
                qDebug() << "Found model at:" << altPath;
                break;
            }
        }
#endif

        if (!source.exists()) {
            qDebug() << "ERROR: Model not found in any location";
            return QString();
        }
    }

    // Create target directory
    QDir().mkpath(QFileInfo(targetPath).path());

    // Copy file
    if (!source.copy(targetPath)) {
        qDebug() << "ERROR: Failed to copy model! Error:" << source.errorString();
        return QString();
    }

    // Set permissions
    QFile::setPermissions(targetPath,
                          QFile::ReadOwner | QFile::WriteOwner |
                              QFile::ReadGroup | QFile::ReadOther);

    qDebug() << "SUCCESS: Model copied to:" << targetPath;
    return targetPath;
}

void RTSVideoOutput::setRtsUrl(const QString &url)
{
    if (m_rtsUrl == url) return;

    bool wasProcessing = m_processing;
    stopProcessing();

    m_rtsUrl = url;
    emit rtsUrlChanged();
    qDebug() << "URL changed to:" << url;

    if (wasProcessing && isComponentComplete() && !m_rtsUrl.isEmpty())
        startProcessing();
}

void RTSVideoOutput::setProcessingEnabled(bool enabled)
{
    if (m_processingEnabled == enabled) return;
    m_processingEnabled = enabled;
    emit processingEnabledChanged();
    applyWorkerSettings();
}

void RTSVideoOutput::setOverlayText(const QString &text)
{
    if (m_overlayText == text) return;
    m_overlayText = text;
    emit overlayTextChanged();
    applyWorkerSettings();
}

void RTSVideoOutput::setDetectionEnabled(bool enabled)
{
    if (m_detectionEnabled == enabled) return;
    m_detectionEnabled = enabled;
    emit detectionEnabledChanged();
    applyWorkerSettings();
}

void RTSVideoOutput::applyWorkerSettings()
{
    if (!m_worker) return;

    QMetaObject::invokeMethod(m_worker, "setProcessingEnabled",
                              Qt::QueuedConnection,
                              Q_ARG(bool, m_processingEnabled));
    QMetaObject::invokeMethod(m_worker, "setOverlayText",
                              Qt::QueuedConnection,
                              Q_ARG(QString, m_overlayText));
    QMetaObject::invokeMethod(m_worker, "setDetectionEnabled",
                              Qt::QueuedConnection,
                              Q_ARG(bool, m_detectionEnabled));
}

void RTSVideoOutput::startProcessing()
{
    qDebug() << "RTSVideoOutput::startProcessing called with URL:" << m_rtsUrl;

    if (m_rtsUrl.isEmpty() || m_processing) {
        qDebug() << "Cannot start - URL empty or already processing";
        return;
    }

    stopProcessing();

    m_worker = new RTSVideoWorker();
    m_workerThread = new QThread(this);
    m_worker->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::started, []() { qDebug() << "Worker thread started"; });
    connect(m_workerThread, &QThread::finished, [this]() {
        qDebug() << "Worker thread finished";
        m_workerThread = nullptr;
    });
    connect(m_worker, &RTSVideoWorker::frameReady, this, &RTSVideoOutput::onFrameReady);
    connect(m_worker, &RTSVideoWorker::error, this, &RTSVideoOutput::onWorkerError);
    connect(m_worker, &RTSVideoWorker::fpsUpdated, this, &RTSVideoOutput::onFpsUpdated);
    connect(m_worker, &RTSVideoWorker::detectionResult, this, &RTSVideoOutput::onDetectionResult);
    connect(m_worker, &RTSVideoWorker::connectionStatusChanged, this, &RTSVideoOutput::onConnectionChanged);
    connect(m_worker, &RTSVideoWorker::modelloaded, this, &RTSVideoOutput::onModelLoaded);
    connect(m_worker, &RTSVideoWorker::modelLoadingFailed, this, &RTSVideoOutput::onModelLoadingFailed);

    m_workerThread->start();
    applyWorkerSettings();

    // If model is already loaded, pass it to worker
    if (m_modelLoaded) {
        QString modelPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/pest_detection.onnx";
        QMetaObject::invokeMethod(m_worker, "loadModelFromPath",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, modelPath));
    }

    QMetaObject::invokeMethod(m_worker, "start",
                              Qt::QueuedConnection,
                              Q_ARG(QString, m_rtsUrl));

    m_processing = true;
    qDebug() << "Processing started";
}

void RTSVideoOutput::stopProcessing()
{
    if (!m_processing) return;

    qDebug() << "Stopping processing";

    if (m_worker) {
        QMetaObject::invokeMethod(m_worker, "stop", Qt::QueuedConnection);
        m_worker->deleteLater();
        m_worker = nullptr;
    }

    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait(2000);
        delete m_workerThread;
        m_workerThread = nullptr;
    }

    {
        QMutexLocker lock(&m_frameMutex);
        m_frame = QImage();
    }

    m_processing = false;
    m_isConnected = false;
    update();
    qDebug() << "Processing stopped";
}

void RTSVideoOutput::onFrameReady(const QImage &frame)
{
    static int frameCount = 0;
    if (++frameCount % 30 == 0) {
        qDebug() << "Frame received in output, size:" << frame.size();
    }

    if (!frame.isNull()) {
        QMutexLocker lock(&m_frameMutex);
        m_frame = frame;
        update();
    }
}

void RTSVideoOutput::onWorkerError(const QString &message)
{
    qDebug() << "Worker error:" << message;
    emit detectionResult("Error: " + message);
}

void RTSVideoOutput::onFpsUpdated(double fps)
{
    m_currentFps = fps;
    emit fpsChanged();
}

void RTSVideoOutput::onDetectionResult(const QString &result)
{
    emit detectionResult(result);
}

void RTSVideoOutput::onConnectionChanged(bool connected)
{
    if (m_isConnected != connected) {
        m_isConnected = connected;
        emit isConnectedChanged(connected);
    }
}

void RTSVideoOutput::onModelLoaded()
{
    qDebug() << "Model loaded signal received in output";
    m_modelLoaded = true;
    emit modelloaded();
}

void RTSVideoOutput::onModelLoadingFailed(const QString &error)
{
    qDebug() << "Model loading failed:" << error;
    emit modelLoadingFailed(error);
}

void RTSVideoOutput::updateTexture()
{
    QImage frame;
    {
        QMutexLocker lock(&m_frameMutex);
        frame = m_frame;
    }

    if (frame.isNull()) {
        delete m_cachedTexture;
        m_cachedTexture = nullptr;
        return;
    }

    if (window()) {
        delete m_cachedTexture;
        m_cachedTexture = window()->createTextureFromImage(frame);

        if (m_cachedTexture) {
            m_cachedTexture->setFiltering(QSGTexture::Linear);
        }
    }
}

QSGNode* RTSVideoOutput::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    auto *node = static_cast<QSGSimpleTextureNode*>(oldNode);

    updateTexture();

    if (!m_cachedTexture) {
        delete node;
        return nullptr;
    }

    if (!node) {
        node = new QSGSimpleTextureNode();
        node->setOwnsTexture(false);
    }

    node->setTexture(m_cachedTexture);
    node->setRect(boundingRect());
    node->setFiltering(QSGTexture::Linear);
    node->markDirty(QSGNode::DirtyMaterial);

    return node;
}