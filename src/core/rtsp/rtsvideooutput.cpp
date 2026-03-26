#include "rtsvideooutput.h"
#include "rtsvideoworker.h"
#include <QDebug>
#include <QMutexLocker>
#include <QSGSimpleTextureNode>
#include <QQuickWindow>
#include <QThread>

RTSVideoOutput::RTSVideoOutput(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
    qDebug() << "RTSVideoOutput created on thread:" << QThread::currentThread();
}

RTSVideoOutput::~RTSVideoOutput()
{
    stopProcessing();
}

void RTSVideoOutput::componentComplete()
{
    QQuickItem::componentComplete();
    qDebug() << "RTSVideoOutput componentComplete, URL:" << m_rtsUrl;
    if (!m_rtsUrl.isEmpty())
        startProcessing();
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
    // more settings if needed
}

void RTSVideoOutput::startProcessing()
{
    qDebug() << "RTSVideoOutput::startProcessing called with URL:" << m_rtsUrl;

    if (m_rtsUrl.isEmpty() || m_processing) {
        qDebug() << "Cannot start - URL empty or already processing";
        return;
    }

    stopProcessing();

    // Create worker and thread
    m_worker = new RTSVideoWorker();
    m_workerThread = new QThread(this);
    m_worker->moveToThread(m_workerThread);

    // Connect signals
    connect(m_workerThread, &QThread::started, []() { qDebug() << "Worker thread started"; });
    connect(m_workerThread, &QThread::finished, [this]() {
        qDebug() << "Worker thread finished";
        m_workerThread = nullptr;
    });
    connect(m_worker, &RTSVideoWorker::frameReady, this, &RTSVideoOutput::onFrameReady);
    connect(m_worker, &RTSVideoWorker::error, this, &RTSVideoOutput::onWorkerError);
    connect(m_worker, &RTSVideoWorker::fpsUpdated, this, &RTSVideoOutput::onFpsUpdated);
    connect(m_worker, &RTSVideoWorker::detectionResult, this, &RTSVideoOutput::onDetectionResult);

    m_workerThread->start();

    // Apply settings before starting
    applyWorkerSettings();

    // Start the worker
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

    delete m_cachedTexture;
    m_cachedTexture = window()->createTextureFromImage(frame);

    if (m_cachedTexture) {
        m_cachedTexture->setFiltering(QSGTexture::Linear);
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