#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QLocale>
#include <QCoreApplication>
#include <QTimer>

#include "src/core/utills/helper.h"
#include "src/core/infarence/infarencerunner.h"
#include "src/core/infarence/diseaseinfomanager.h"
#include "src/core/rtsp/rtsvideooutput.h"
#include "src/core/mqtt/mqttclient.h"
#include "src/core/mqtt/pnddeviceconfigurator.h"
#include "src/core/mqtt/pnddevicemodel.h"
#include "src/core/mqtt/pnddevicestate.h"
#include "src/core/mqtt/pnddevice.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qputenv("QT_ANDROID_NO_EXIT_CALL", "true");

    QCoreApplication::setOrganizationName("PlantDoctor");
    QCoreApplication::setOrganizationDomain("PlantDoctor.com");
    QCoreApplication::setApplicationName("PlantDoctor");

    // Core Instances
    Helper helper;
    InfarenceRunner infarenceRunner;

    // Device Management
    PNDDeviceConfigurator deviceConfigurator;
    deviceConfigurator.setMqttBroker("192.168.8.130", 1883);
    // deviceConfigurator.setMqttCredentials("username", "password");

    PNDDeviceModel* deviceModel = deviceConfigurator.deviceModel();

    // Language Handling
    QString systemLang = QLocale::system().name().split("_").first();
    QString appLanguage = (systemLang == "ny" || systemLang == "mw") ? "ny" : "en";
    DiseaseInfoManager::instance().loadLanguage(appLanguage);

    // QML Engine
    QQmlApplicationEngine engine;

    // Context Properties
    engine.rootContext()->setContextProperty("Helper", &helper);
    engine.rootContext()->setContextProperty("InfarenceRunner", &infarenceRunner);
    engine.rootContext()->setContextProperty("CurrentLanguage", appLanguage);
    engine.rootContext()->setContextProperty("DeviceConfigurator", &deviceConfigurator);
    engine.rootContext()->setContextProperty("DeviceModel", deviceModel);

    // Register enum for QML
    qmlRegisterUncreatableType<PNDDeviceState>(
        "PlantDoctor", 1, 0,
        "DeviceState",
        "Enum only"
        );

    // RTSP
    qmlRegisterType<RTSVideoOutput>("RTSVideoOutput",1,0,"RTSVideoOutput");

    // Language Names
    QVariantMap languageNames;
    languageNames["en"] = "English";
    languageNames["ny"] = "Chichewa";
    engine.rootContext()->setContextProperty("LanguageNames", languageNames);

    // History Model
    qmlRegisterSingletonType(QUrl("qrc:/ui/features/inference/model/HistoryModel.qml"),
                             "HistoryModel", 1, 0, "HistoryModel");

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);

    engine.loadFromModule("PlantDoctor", "Main");

    return app.exec();
}