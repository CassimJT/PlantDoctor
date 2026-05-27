#ifndef INFARENCERUNNER_H
#define INFARENCERUNNER_H

#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QUrl>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

// Executorch includes
#include <executorch/extension/module/module.h>
#include <executorch/extension/tensor/tensor.h>

#include "diseaseinfomanager.h"

// Use the correct namespaces
using TensorPtr = std::shared_ptr<executorch::aten::Tensor>;
using ModulePtr = std::unique_ptr<executorch::extension::Module>;

class InfarenceRunner : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString diseaseName READ diseaseName NOTIFY diseaseNameChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString cure READ cure NOTIFY cureChanged)
    Q_PROPERTY(float confidence READ confidence NOTIFY confidenceChanged)
    Q_PROPERTY(int classIndex READ classIndex NOTIFY classIndexChanged)
    Q_PROPERTY(bool isModelLoaded READ getIsModelLoaded NOTIFY isModelLoadedChanged)
    Q_PROPERTY(float riskLevel READ riskLevel WRITE setRiskLevel NOTIFY riskLevelChanged FINAL)

public:
    explicit InfarenceRunner(QObject *parent = nullptr);
    ~InfarenceRunner();

    // Getters
    QString diseaseName() const { return m_diseaseName; }
    QString description() const { return m_description; }
    QString cure() const { return m_cure; }
    float confidence() const { return m_confidence; }
    int classIndex() const { return m_classIndex; }
    bool getIsModelLoaded() const { return isModelLoaded; }

    // Language management
    Q_INVOKABLE bool setLanguage(const QString& languageCode);
    Q_INVOKABLE QStringList availableLanguages() const;
    Q_INVOKABLE QString currentLanguage() const;
    Q_INVOKABLE void loadHistoryResult(int classIndex);

    float riskLevel() const;
    void setRiskLevel(float newRiskLevel);

public slots:
    void classifyImage(const QString &imageDataBase64);
    void loadModel();
    void reloadModel();

signals:
    void diseaseNameChanged();
    void descriptionChanged();
    void cureChanged();
    void confidenceChanged(float value);
    void classIndexChanged();
    void isModelLoadedChanged();
    void infarenceFinished();
    void infarenceFailed(const QString& error);
    void languageChanged(const QString& language);

    void riskLevelChanged();


private:
    // Helper methods - declare these as private
    void setDiseaseName(const QString &newDiseaseName);
    void setDescription(const QString &newDescription);
    void setCure(const QString &newCure);
    void setConfidence(float newConfidence);
    void setClassIndex(int newClassIndex);
    void updateDiseaseInfo(int classId);
    void clearResults();

    // Processing methods
    TensorPtr preprocess(const cv::Mat &img);
    QImage matToQImage(const cv::Mat &mat);
    cv::Mat qImageToMat(const QImage &image);
    QString prepareModelFile();

    // Member variables
    bool isModelLoaded;
    QString m_diseaseName;
    QString m_description;
    QString m_cure;
    float m_confidence;
    float m_riskLevel = 0.0f;
    int m_classIndex;

    ModulePtr module;
    QString m_currentLanguage;

    // Constants
    static constexpr int MODEL_INPUT_SIZE = 224;
    static constexpr int NUM_CLASSES = 38;

};

#endif // INFARENCERUNNER_H
