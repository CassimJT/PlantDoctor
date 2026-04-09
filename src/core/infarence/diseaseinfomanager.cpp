// diseaseinfomanager.cpp
#include "diseaseinfomanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>

const QString DiseaseInfoManager::DEFAULT_LANGUAGE = "ny";

DiseaseInfoManager::DiseaseInfoManager(QObject *parent)
    : QObject(parent)
    , m_currentLanguage(DEFAULT_LANGUAGE)
{
    qDebug() << "DiseaseInfoManager constructed";

    // Load default language
    loadLanguage(DEFAULT_LANGUAGE);
}

DiseaseInfoManager::~DiseaseInfoManager()
{
}

DiseaseInfoManager& DiseaseInfoManager::instance()
{
    static DiseaseInfoManager manager;
    return manager;
}

bool DiseaseInfoManager::loadLanguage(const QString& languageCode)
{
    qDebug() << "=== Attempting to load language:" << languageCode << "===";

    // Try multiple locations in order of preference
    QStringList searchPaths;

    // 1. Application data directory (writable)
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    searchPaths << appDataPath + "/languages/diseases_" + languageCode + ".json";

    // 2. Application directory
    searchPaths << QCoreApplication::applicationDirPath() + "/languages/diseases_" + languageCode + ".json";

    // 3. Resources (for embedded files)
    searchPaths << ":/languages/diseases_" + languageCode + ".json";

#ifdef Q_OS_ANDROID
    // 4. Android assets
    searchPaths << "assets:/languages/diseases_" + languageCode + ".json";

    // 5. Alternative Android asset path
    searchPaths << "file:///android_asset/languages/diseases_" + languageCode + ".json";
#endif

    for (const QString& path : searchPaths) {
        qDebug() << "Trying path:" << path;

        QFile file(path);
        if (file.exists()) {
            qDebug() << "File exists, attempting to open:" << path;

            if (file.open(QIODevice::ReadOnly)) {
                QByteArray data = file.readAll();
                file.close();

                if (!data.isEmpty()) {
                    qDebug() << "Read" << data.size() << "bytes from file";

                    QJsonParseError parseError;
                    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

                    if (parseError.error == QJsonParseError::NoError) {
                        if (doc.isArray()) {
                            QMap<int, DiseaseInfo> newInfo;
                            QJsonArray diseases = doc.array();

                            for (const QJsonValue& value : diseases) {
                                QJsonObject obj = value.toObject();

                                DiseaseInfo info;
                                info.name = obj["name"].toString();
                                info.riskLevel = static_cast<float>(obj["riskLevel"].toDouble(0.0));
                                // Handle description (can be string or array)
                                if (obj["description"].isArray()) {
                                    QStringList descParts;
                                    QJsonArray descArray = obj["description"].toArray();
                                    for (const auto& part : descArray) {
                                        descParts.append(part.toString());
                                    }
                                    info.description = descParts.join(" ");
                                } else {
                                    info.description = obj["description"].toString();
                                }

                                // Handle cure (can be string or array)
                                if (obj["cure"].isArray()) {
                                    QStringList cureParts;
                                    QJsonArray cureArray = obj["cure"].toArray();
                                    for (const auto& part : cureArray) {
                                        cureParts.append(part.toString());
                                    }
                                    info.cure = cureParts.join(" ");
                                } else {
                                    info.cure = obj["cure"].toString();
                                }

                                int classId = obj["class_id"].toInt(-1);

                                if (classId >= 0 && !info.name.isEmpty()) {
                                    newInfo[classId] = info;
                                }
                            }

                            if (!newInfo.isEmpty()) {
                                m_diseaseInfo = newInfo;
                                m_currentLanguage = languageCode;
                                emit languageChanged(languageCode);
                                qDebug() << "Successfully loaded" << newInfo.size() << "entries from:" << path;
                                return true;
                            } else {
                                qDebug() << "No valid disease entries found in JSON";
                            }
                        } else {
                            qDebug() << "JSON root is not an array";
                        }
                    } else {
                        qDebug() << "JSON parse error:" << parseError.errorString();
                    }
                } else {
                    qDebug() << "File is empty:" << path;
                }
            } else {
                qDebug() << "Failed to open file:" << path << "- Error:" << file.errorString();
            }
        } else {
            qDebug() << "File does not exist:" << path;
        }
    }

    qWarning() << "Failed to load language:" << languageCode;
    return false;
}

bool DiseaseInfoManager::loadLanguage(QLocale::Language language)
{
    QString languageCode = QLocale::languageToString(language);
    return loadLanguage(languageCode.toLower());
}

DiseaseInfo DiseaseInfoManager::getDiseaseInfo(int classId) const
{
    return m_diseaseInfo.value(classId, DiseaseInfo());
}

QString DiseaseInfoManager::getDiseaseName(int classId) const
{
    return m_diseaseInfo.value(classId).name;
}

QString DiseaseInfoManager::getDiseaseDescription(int classId) const
{
    return m_diseaseInfo.value(classId).description;
}

QString DiseaseInfoManager::getDiseaseCure(int classId) const
{
    return m_diseaseInfo.value(classId).cure;
}

float DiseaseInfoManager::getDiseaseRiskLevel(int classId) const
{
      return m_diseaseInfo.value(classId).riskLevel;
}

QStringList DiseaseInfoManager::availableLanguages() const
{
    return QStringList({"en", "ny"});
}


