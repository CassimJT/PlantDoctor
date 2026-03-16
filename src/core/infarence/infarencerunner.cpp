#include "infarencerunner.h"
#include <QCoreApplication>

// Add these using directives to fix namespace issues
using namespace executorch::extension;
using executorch::runtime::EValue;

InfarenceRunner::InfarenceRunner(QObject *parent)
    : QObject{parent}
    , isModelLoaded(false)
    , m_diseaseName("")
    , m_description("")
    , m_cure("")
    , m_confidence(0.0)
    , m_classIndex(-1)
    , m_currentLanguage("en")
{
    // Initialize the disease info manager
    DiseaseInfoManager::instance().loadLanguage("en");

    // Connect to language changes from manager
    connect(&DiseaseInfoManager::instance(), &DiseaseInfoManager::languageChanged,
            this, [this](const QString& language) {
                qDebug() << "Language changed in manager:" << language;
                m_currentLanguage = language;
                emit languageChanged(language);

                // If we have a current classification, refresh the display
                if (m_classIndex >= 0) {
                    updateDiseaseInfo(m_classIndex);
                }
            });

    // Load the model
    loadModel();
}

InfarenceRunner::~InfarenceRunner()
{
    // Clean up if needed
}

void InfarenceRunner::clearResults()
{
    setDiseaseName("");
    setDescription("");
    setCure("");
    setConfidence(0.0);
    setClassIndex(-1);
}

bool InfarenceRunner::setLanguage(const QString& languageCode)
{
    return DiseaseInfoManager::instance().loadLanguage(languageCode);
}

QStringList InfarenceRunner::availableLanguages() const
{
    return DiseaseInfoManager::instance().availableLanguages();
}

QString InfarenceRunner::currentLanguage() const
{
    return DiseaseInfoManager::instance().currentLanguage();
}

void InfarenceRunner::updateDiseaseInfo(int classId)
{
    DiseaseInfo info = DiseaseInfoManager::instance().getDiseaseInfo(classId);

    setDiseaseName(info.name);
    setDescription(info.description);
    setCure(info.cure);
}

void InfarenceRunner::classifyImage(const QString &imageDataBase64)
{
    // Clear previous results
    clearResults();

    QString filePath;

    // Handle base64 image data
    if (imageDataBase64.startsWith("data:image")) {
        QByteArray data = QByteArray::fromBase64(imageDataBase64.split(',')[1].toUtf8());
        filePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/temp_image.png";
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            QString error = "Failed to write temp file";
            qDebug() << error;
            emit infarenceFailed(error);
            return;
        }
        file.write(data);
        file.close();
    } else {
        filePath = imageDataBase64;
    }

    // Handle file:// URLs (from gallery picker)
    qDebug() << "Image path:" << filePath;
    if(filePath.startsWith("file://")) {
        filePath = QUrl(filePath).toLocalFile();
    }

    // Load image with OpenCV
    cv::Mat img = cv::imread(filePath.toStdString());
    if (img.empty()) {
        QString error = "Failed to load image";
        qDebug() << error;
        emit infarenceFailed(error);
        return;
    }

    // Preprocess the image
    auto input = preprocess(img);
    if (!input) {
        QString error = "Preprocessing failed";
        qDebug() << error;
        emit infarenceFailed(error);
        return;
    }

    // Check if model is loaded
    if (!isModelLoaded || !module) {
        QString error = "Model not loaded";
        qDebug() << error;
        emit infarenceFailed(error);
        return;
    }

    // Run inference
    std::vector<EValue> inputs;
    inputs.emplace_back(*input);
    const auto result = module->forward(inputs);

    if (!result.ok()) {
        QString error = "Inference failed";
        qDebug() << error;
        emit infarenceFailed(error);
        return;
    }

    // Process output tensor
    const auto& outputTensor = result->at(0).toTensor();
    const float* output = outputTensor.const_data_ptr<float>();
    int numOutputs = outputTensor.numel();

    if (numOutputs != NUM_CLASSES) {
        QString error = QString("Unexpected output size: %1 (expected %2)")
        .arg(numOutputs).arg(NUM_CLASSES);
        qDebug() << error;
        emit infarenceFailed(error);
        return;
    }

    qDebug() << "Num elements:" << numOutputs;

    // Apply softmax
    std::vector<float> probs(numOutputs);
    float maxLogit = *std::max_element(output, output + numOutputs);
    float sumExp = 0.0f;

    for (int i = 0; i < numOutputs; ++i) {
        probs[i] = std::exp(output[i] - maxLogit);
        sumExp += probs[i];
    }

    for (int i = 0; i < numOutputs; ++i) {
        probs[i] /= sumExp;
    }

    // Find top class
    auto maxIt = std::max_element(probs.begin(), probs.end());
    int topClassIdx = std::distance(probs.begin(), maxIt);
    float topConfidence = *maxIt * 100.0f;

    qDebug() << "Top class index:" << topClassIdx << "Confidence:" << topConfidence << "%";

    // Update results
    setConfidence(topConfidence);
    setClassIndex(topClassIdx);
    updateDiseaseInfo(topClassIdx);

    emit infarenceFinished();
}

// Setter implementations
void InfarenceRunner::setDiseaseName(const QString &newDiseaseName)
{
    if (m_diseaseName == newDiseaseName)
        return;
    m_diseaseName = newDiseaseName;
    emit diseaseNameChanged();
}

void InfarenceRunner::setDescription(const QString &newDescription)
{
    if (m_description == newDescription)
        return;
    m_description = newDescription;
    emit descriptionChanged();
}

void InfarenceRunner::setCure(const QString &newCure)
{
    if (m_cure == newCure)
        return;
    m_cure = newCure;
    emit cureChanged();
}

void InfarenceRunner::setConfidence(float newConfidence)
{
    if (qFuzzyCompare(m_confidence, newConfidence))
        return;
    m_confidence = newConfidence;
    emit confidenceChanged();
}

void InfarenceRunner::setClassIndex(int newClassIndex)
{
    if (m_classIndex == newClassIndex)
        return;
    m_classIndex = newClassIndex;
    emit classIndexChanged();
}

void InfarenceRunner::loadModel()
{
    QString path = prepareModelFile();
    if (path.isEmpty()) {
        qDebug() << "Model not found!";
        isModelLoaded = false;
        emit isModelLoadedChanged();
        return;
    }

    try {
        module.reset(new executorch::extension::Module(path.toStdString()));
        isModelLoaded = true;
        qDebug() << "Model loaded successfully from:" << path;
    } catch (const std::exception &e) {
        qDebug() << "Failed to load model:" << e.what();
        isModelLoaded = false;
    }

    emit isModelLoadedChanged();
}

void InfarenceRunner::reloadModel()
{
    loadModel();
}

TensorPtr InfarenceRunner::preprocess(const cv::Mat &img)
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

    auto tensor = executorch::extension::from_blob(
        data->data(),
        shape,
        executorch::aten::ScalarType::Float
        );

    if (!tensor) {
        qDebug() << "Failed to create tensor";
        return nullptr;
    }

    // Simple memory management - store in static map
    static QMap<TensorPtr, std::shared_ptr<std::vector<float>>> tensorDataMap;
    tensorDataMap[tensor] = data;

    return tensor;
}

QImage InfarenceRunner::matToQImage(const cv::Mat &mat)
{
    switch (mat.type()) {
    case CV_8UC3: {
        QImage img(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    case CV_8UC4: {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
    }
    case CV_8UC1: {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
    }
    default:
        qWarning() << "Unsupported cv::Mat format:" << mat.type();
        return QImage();
    }
}

cv::Mat InfarenceRunner::qImageToMat(const QImage &image)
{
    switch (image.format()) {
    case QImage::Format_RGB888: {
        cv::Mat mat(image.height(), image.width(), CV_8UC3,
                    const_cast<uchar*>(image.bits()), image.bytesPerLine());
        cv::Mat matBGR;
        cv::cvtColor(mat, matBGR, cv::COLOR_RGB2BGR);
        return matBGR;
    }
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
    case QImage::Format_RGB32: {
        cv::Mat mat(image.height(), image.width(), CV_8UC4,
                    const_cast<uchar*>(image.bits()), image.bytesPerLine());
        cv::Mat matBGR;
        cv::cvtColor(mat, matBGR, cv::COLOR_BGRA2BGR);
        return matBGR;
    }
    case QImage::Format_Grayscale8: {
        cv::Mat mat(image.height(), image.width(), CV_8UC1,
                    const_cast<uchar*>(image.bits()), image.bytesPerLine());
        return mat.clone();
    }
    default:
        qWarning() << "Unsupported QImage format:" << image.format();
        return cv::Mat();
    }
}

QString InfarenceRunner::prepareModelFile()
{
    QString targetPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
    + "/plant_disease_model.pte";

    if (QFile::exists(targetPath)) {
        return targetPath;
    }

#ifdef Q_OS_ANDROID
    QString sourcePath = "assets:/model/plant_disease_model.pte";
#else
    QString sourcePath = ":/model/plant_disease_model.pte";
#endif

    QFile source(sourcePath);
    if (!source.exists()) {
        qDebug() << "Model file not found at:" << sourcePath;

#ifndef Q_OS_ANDROID
        QString alternativePath = QCoreApplication::applicationDirPath()
                                  + "/plant_disease_model.pte";
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
