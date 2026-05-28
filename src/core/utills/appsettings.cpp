#include "appsettings.h"

AppSettings::AppSettings(QObject *parent)
    : QObject{parent}
{
    //constructor
}

AppSettings &AppSettings::instance()
{
    static AppSettings appSettings;
    return appSettings;
}

//Survey
QString AppSettings::varietyForClass(int classId) const{
    return settings.value(
                       QString("variety/%1").arg(classId),
                       ""
                       ).toString();
}

void AppSettings::setVarietyForClass(int classId,const QString &variety){
    settings.setValue(
        QString("variety/%1").arg(classId),
        variety
        );
}

bool AppSettings::hasVarietyForClass(int classId) const{
    return !settings.value(
                        QString("variety/%1")
                            .arg(classId)
                        ).toString().isEmpty();
}

//language
QString AppSettings::language() const
{
    return settings.value("app/lan","en").toString();
}

void AppSettings::setLanguage(const QString &newLanguage)
{
    settings.setValue("app/lan", newLanguage);
}

//inference counter
int AppSettings::inferenceCounter() const
{
    return settings.value("app/inferenceCounter", 0).toInt();
}

void AppSettings::setInferenceCounter(int newInferenceCounter)
{
    settings.setValue(
        "app/inferenceCounter",
        newInferenceCounter
        );
}

//auth
bool AppSettings::isLoggedIn() const
{
    return settings.value("auth/isLoggedIn", false).toBool();
}

void AppSettings::setLoggedIn(bool loggedIn)
{
    settings.setValue("auth/isLoggedIn", loggedIn);
}

QString AppSettings::authToken() const
{
    return settings.value("auth/token", "").toString();
}

void AppSettings::setAuthToken(const QString &token)
{
    settings.setValue("auth/token", token);
}

// Survey completion methods
bool AppSettings::hasSurveyCompleted(int classId) const
{
    return settings.value(
                       QString("survey/completed/%1").arg(classId),
                       false
                       ).toBool();
}

void AppSettings::setSurveyCompleted(int classId, bool completed)
{
    settings.setValue(
        QString("survey/completed/%1").arg(classId),
        completed
        );
}

// User data methods
void AppSettings::setUserData(const QString& phoneNumber, const QString& district)
{
    settings.setValue("user/phoneNumber", phoneNumber);
    settings.setValue("user/district", district);
}

QString AppSettings::getUserPhone() const
{
    return settings.value("user/phoneNumber", "").toString();
}

QString AppSettings::getUserDistrict() const
{
    return settings.value("user/district", "").toString();
}

bool AppSettings::hasUserData() const
{
    return !getUserPhone().isEmpty() && !getUserDistrict().isEmpty();
}