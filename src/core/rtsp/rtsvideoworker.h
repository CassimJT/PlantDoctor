#ifndef RTSVIDEOWORKER_H
#define RTSVIDEOWORKER_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQueue>
#include <QMutex>
#include <QAtomicInt>
#include <QElapsedTimer>
#include <QThreadPool>
#include <QRunnable>
#include <functional>
#include <opencv2/opencv.hpp>
// Executorch includes
#include <executorch/extension/module/module.h>
#include <executorch/extension/tensor/tensor.h>
using executorch::extension::TensorPtr;

// Detection Task for async inference using ExecuTorch
class DetectionTask : public QRunnable
{
public:
    DetectionTask(std::shared_ptr<executorch::extension::Module> module,
                  cv::Mat frame, cv::Size originalShape,
                  const QStringList& classNames, float confThreshold = 0.20f);
    ~DetectionTask();

    void run() override;

    using Callback = std::function<void(const QList<QMap<QString, QVariant>>&, const cv::Size&)>;
    void setCallback(Callback callback) { m_callback = callback; }

private:
    std::shared_ptr<executorch::extension::Module> m_module;
    cv::Mat m_frame;
    cv::Size m_originalShape;
    QStringList m_classNames;
    float m_confThreshold;
    float m_nmsThreshold;
    int m_inputSize;
    Callback m_callback;
};

class RTSVideoWorker : public QObject
{
    Q_OBJECT

public:
    explicit RTSVideoWorker(QObject *parent = nullptr);
    ~RTSVideoWorker();

public slots:
    void start(const QString &url);
    void stop();
    void setProcessingEnabled(bool enabled);
    void setOverlayText(const QString &text);
    void setDetectionEnabled(bool enabled);
    void loadModel();

signals:
    void frameReady(const QImage &frame);
    void error(const QString &message);
    void detectionResult(const QString &result);
    void fpsUpdated(double fps);
    void connectionStatusChanged(bool connected);
    void modelloaded();
    void modelLoadingFailed(const QString &error);

private slots:
    void onData();
    void onFinished();

private:
    void parseStreamData();
    void decodeLoop();
    QImage processFrame(const cv::Mat &rawFrame);
    void applyOverlay(cv::Mat &frame);
    void updateFPS();
    void runDetection(cv::Mat &frame);
    void drawDetections(cv::Mat &frame);
    void detectionCallback(const QList<QMap<QString, QVariant>>& detections, const cv::Size& originalShape);
    TensorPtr preprocess(const cv::Mat &img);
    QString prepareModelFile();  // Same as InfarenceRunner

    // Network
    QNetworkAccessManager *m_nam;
    QNetworkReply *m_reply;
    QString m_url;
    QByteArray m_buffer;
    QMutex m_bufferMutex;

    // Decoding
    QQueue<QByteArray> m_jpegQueue;
    QMutex m_queueMutex;
    QAtomicInt m_running;
    QAtomicInt m_decoderRunning;

    // Processing
    bool m_processingEnabled;
    QString m_overlayText;
    cv::Mat m_latestFrame;
    QMutex m_frameMutex;

    // FPS
    int m_frameCount;
    QElapsedTimer m_fpsTimer;
    double m_currentFps;

    // ExecuTorch Model (same as InfarenceRunner)
    std::shared_ptr<executorch::extension::Module> m_module;
    bool m_isModelLoaded;
    bool m_detectionEnabled;
    float m_confThreshold;
    float m_nmsThreshold;
    int m_detectionInterval;
    qint64 m_lastDetectionTime;
    bool m_detectionInProgress;
    QMutex m_detectionMutex;
    QThreadPool *m_threadPool;
    QStringList m_classNames;
    static constexpr int MODEL_INPUT_SIZE = 640;
    static constexpr int NUM_CLASSES = 12;

    // Detection persistence
    struct Detection {
        cv::Rect rect;
        QString label;
        float confidence;
        qint64 timestamp;
    };
    QList<Detection> m_lastDetection;
    qint64 m_lastGoodDetectionTime;
    const qint64 PERSISTENCE_TIME = 5000;
};

#endif // RTSVIDEOWORKER_H