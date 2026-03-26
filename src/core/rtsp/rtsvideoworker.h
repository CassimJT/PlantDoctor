#ifndef RTSVIDEOWORKER_H
#define RTSVIDEOWORKER_H

#include <QObject>
#include <QImage>
#include <QMutex>
#include <QQueue>
#include <QAtomicInteger>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QThreadPool>
#include <QElapsedTimer>
#include <opencv2/opencv.hpp>

class RTSVideoWorker : public QObject
{
    Q_OBJECT
public:
    explicit RTSVideoWorker(QObject *parent = nullptr);
    ~RTSVideoWorker() override;

    // Configuration methods
    void setProcessingEnabled(bool enabled);
    void setTargetSize(int width, int height);
    void setOverlayText(const QString &text);
    void setOverlayEnabled(bool enabled);

public slots:
    void start(const QString &url);
    void stop();

signals:
    void frameReady(const QImage &frame);
    void error(const QString &message);
    void started();
    void stopped();
    void fpsUpdated(double fps);
    void detectionResult(const QString &result);

private slots:
    void onData();
    void onFinished();

private:
    void parseStreamData();
    void decodeLoop();
    void clearQueue();

    // Image processing pipeline with memory optimization
    QImage processFrame(const cv::Mat &rawFrame);
    void applyOverlay(cv::Mat &frame);
    void runDetectionModels(cv::Mat &frame);
    void drawDetectionResults(cv::Mat &frame);

    // FPS calculation
    void updateFPS();

    QString m_url;
    QByteArray m_buffer;
    QAtomicInt m_running{0};

    // Optimized queue with memory management
    QQueue<QByteArray> m_jpegQueue;
    QMutex m_queueMutex;
    QAtomicInt m_decoderRunning{0};

    QNetworkAccessManager *m_nam = nullptr;
    QNetworkReply *m_reply = nullptr;

    // Processing configuration
    struct ProcessingConfig {
        bool enabled = true;
        int targetWidth = 640;
        int targetHeight = 480;
        bool overlayEnabled = true;
        QString overlayText = "ESP32-CAM";
        bool detectionEnabled = false;
        int detectionInterval = 5;
    } m_config;

    // Performance monitoring
    QElapsedTimer m_fpsTimer;
    int m_frameCount = 0;
    double m_currentFps = 0.0;
    int m_detectionCounter = 0;

    // Detection results
    struct DetectionResults {
        QStringList faces;
        QList<cv::Rect> faceRects;
        QStringList objects;
        bool hasQRCode = false;
        QString qrCodeData;
    } m_lastDetection;
    QMutex m_detectionMutex;

    // Memory pool for reusing buffers
    QByteArray m_reusableBuffer;
    QMutex m_bufferMutex;
};

#endif // RTSVIDEOWORKER_H