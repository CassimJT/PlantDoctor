// diseaseinfomanager.h
#ifndef DISEASEINFOMANAGER_H
#define DISEASEINFOMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QLocale>

struct DiseaseInfo {
    QString name;
    QString description;
    QString cure;
    float riskLevel = 0.0f;

    bool isValid() const { return !name.isEmpty(); }
};

class DiseaseInfoManager : public QObject
{
    Q_OBJECT

public:
    static DiseaseInfoManager& instance();

    bool loadLanguage(const QString& languageCode);
    bool loadLanguage(QLocale::Language language);

    DiseaseInfo getDiseaseInfo(int classId) const;
    QString getDiseaseName(int classId) const;
    QString getDiseaseDescription(int classId) const;
    QString getDiseaseCure(int classId) const;
    float getDiseaseRiskLevel(int classId) const;
    QStringList availableLanguages() const;
    QString currentLanguage() const { return m_currentLanguage; }  // Inline implementation

signals:
    void languageChanged(const QString& languageCode);

private:
    explicit DiseaseInfoManager(QObject *parent = nullptr);
    ~DiseaseInfoManager();

    QMap<int, DiseaseInfo> m_diseaseInfo;
    QString m_currentLanguage;

    static const QString DEFAULT_LANGUAGE;
};

#endif // DISEASEINFOMANAGER_H
