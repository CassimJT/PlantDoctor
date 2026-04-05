#ifndef RTSVIDEOOUTPUT_H
#define RTSVIDEOOUTPUT_H

#include <QQuickItem>
#include <QImage>
#include <QMutex>
#include <QThread>
#include <QSGTexture>
#include <QSGSimpleTextureNode>

class RTSVideoWorker;

class RTSVideoOutput : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString rtsUrl READ rtsUrl WRITE setRtsUrl NOTIFY rtsUrlChanged)
    Q_PROPERTY(bool processingEnabled READ processingEnabled WRITE setProcessingEnabled NOTIFY processingEnabledChanged)
    Q_PROPERTY(QString overlayText READ overlayText WRITE setOverlayText NOTIFY overlayTextChanged)
    Q_PROPERTY(bool detectionEnabled READ detectionEnabled WRITE setDetectionEnabled NOTIFY detectionEnabledChanged)
    Q_PROPERTY(double fps READ fps NOTIFY fpsChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)

public:
    explicit RTSVideoOutput(QQuickItem *parent = nullptr);
    ~RTSVideoOutput();

    QString rtsUrl() const { return m_rtsUrl; }
    void setRtsUrl(const QString &url);

    bool processingEnabled() const { return m_processingEnabled; }
    void setProcessingEnabled(bool enabled);

    QString overlayText() const { return m_overlayText; }
    void setOverlayText(const QString &text);

    bool detectionEnabled() const { return m_detectionEnabled; }
    void setDetectionEnabled(bool enabled);

    double fps() const { return m_currentFps; }
    bool isConnected() const { return m_isConnected; }

public slots:
    void startProcessing();
    void stopProcessing();

signals:
    void rtsUrlChanged();
    void processingEnabledChanged();
    void overlayTextChanged();
    void detectionEnabledChanged();
    void fpsChanged();
    void isConnectedChanged(bool connected);
    void detectionResult(const QString &result);
    void modelloaded();
    void modelLoadingFailed(const QString &error);

protected:
    void componentComplete() override;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private slots:
    void onFrameReady(const QImage &frame);
    void onWorkerError(const QString &message);
    void onFpsUpdated(double fps);
    void onDetectionResult(const QString &result);
    void onConnectionChanged(bool connected);
    void onModelLoaded();
    void onModelLoadingFailed(const QString &error);

private:
    void applyWorkerSettings();
    void updateTexture();
    void loadModel();
    QString prepareModelFile();

    QString m_rtsUrl;
    QImage m_frame;
    QMutex m_frameMutex;
    bool m_processingEnabled;
    QString m_overlayText;
    bool m_detectionEnabled;
    double m_currentFps;
    bool m_isConnected;

    RTSVideoWorker *m_worker;
    QThread *m_workerThread;
    bool m_processing;

    QSGTexture *m_cachedTexture;
    bool m_modelLoaded;
    QThread m_modelLoaderThread;
};

#endif // RTSVIDEOOUTPUT_H