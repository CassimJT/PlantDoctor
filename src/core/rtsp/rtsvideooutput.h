#ifndef RTSVIDEOOUTPUT_H
#define RTSVIDEOOUTPUT_H

#include <QQuickItem>
#include <QImage>
#include <QMutex>
#include <QSGTexture>
#include <QPointer>

class RTSVideoWorker;
class QSGSimpleTextureNode;

class RTSVideoOutput : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString rtsUrl READ rtsUrl WRITE setRtsUrl NOTIFY rtsUrlChanged)
    Q_PROPERTY(bool processingEnabled READ isProcessingEnabled WRITE setProcessingEnabled NOTIFY processingEnabledChanged)
    Q_PROPERTY(QString overlayText READ overlayText WRITE setOverlayText NOTIFY overlayTextChanged)
    Q_PROPERTY(bool detectionEnabled READ isDetectionEnabled WRITE setDetectionEnabled NOTIFY detectionEnabledChanged)
    Q_PROPERTY(double fps READ fps NOTIFY fpsChanged)

public:
    explicit RTSVideoOutput(QQuickItem *parent = nullptr);
    ~RTSVideoOutput() override;

    QString rtsUrl() const { return m_rtsUrl; }
    void setRtsUrl(const QString &url);

    bool isProcessingEnabled() const { return m_processingEnabled; }
    void setProcessingEnabled(bool enabled);

    QString overlayText() const { return m_overlayText; }
    void setOverlayText(const QString &text);

    bool isDetectionEnabled() const { return m_detectionEnabled; }
    void setDetectionEnabled(bool enabled);

    double fps() const { return m_currentFps; }

signals:
    void rtsUrlChanged();
    void processingEnabledChanged();
    void overlayTextChanged();
    void detectionEnabledChanged();
    void fpsChanged();
    void detectionResult(const QString &result);  // Forward detection results to QML

protected:
    QSGNode* updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;
    void componentComplete() override;

private slots:
    void onFrameReady(const QImage &frame);
    void onWorkerError(const QString &message);
    void onFpsUpdated(double fps);
    void onDetectionResult(const QString &result);

private:
    void startProcessing();
    void stopProcessing();
    void updateTexture();
    void applyWorkerSettings();

    QString m_rtsUrl;
    QImage m_frame;
    QMutex m_frameMutex;

    // Configuration
    bool m_processingEnabled = true;
    QString m_overlayText = "ESP32-CAM";
    bool m_detectionEnabled = false;
    double m_currentFps = 0.0;

    // Worker management
    QPointer<RTSVideoWorker> m_worker;
    QThread *m_workerThread = nullptr;
    bool m_processing = false;

    // Texture caching
    QSGTexture *m_cachedTexture = nullptr;
    QSize m_textureSize;
};

#endif // RTSVIDEOOUTPUT_H