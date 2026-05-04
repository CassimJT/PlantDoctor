#include "cloudsynch.h"

#include "android_app_BuilderProxy.h"
#include "android_app_NotificationManagerProxy.h"
#include "android_app_NotificationChannelProxy.h"
#include "android_app_NotificationProxy.h"
#include "android_os_ContextProxy.h"

#if defined(Q_OS_ANDROID)
extern "C" JNIEXPORT void JNICALL
Java_com_plantdoctor_NativeBridge_nativeInvoked(JNIEnv* env, jclass clazz) {
    __android_log_print(ANDROID_LOG_DEBUG, "NativeBridge", "nativeInvoked called");

    // 1. Verify Qt environment
    if (!qApp) {
        __android_log_print(ANDROID_LOG_ERROR, "NativeBridge",
                            "Qt application instance not available");
        return;
    }

    // 2. Queue execution if not on main thread
    if (QThread::currentThread() != qApp->thread()) {
        __android_log_print(ANDROID_LOG_DEBUG, "NativeBridge",
                            "Queuing execution to main thread");
        QMetaObject::invokeMethod(qApp, [](){
            CloudSynch::instance()->invoked();
        }, Qt::QueuedConnection);
        return;
    }

    // 3. Direct execution
    try {
        __android_log_print(ANDROID_LOG_DEBUG, "NativeBridge",
                            "Calling CloudSynch::invoked()");
        CloudSynch::instance()->invoked();
    } catch (const std::exception& e) {
        __android_log_print(ANDROID_LOG_ERROR, "NativeBridge",
                            "Exception: %s", e.what());
    }
}
#endif

CloudSynch *CloudSynch::_instance = nullptr;

CloudSynch::CloudSynch(QObject *parent)
    : QObject{parent}
    , m_alarmScheduled(false)
{
    __android_log_print(ANDROID_LOG_DEBUG, "CloudSynch", "CloudSynch constructor");

    // Only schedule alarm ONCE when the singleton is created
    static bool alarmScheduled = false;
    if (!alarmScheduled) {
#if defined(Q_OS_ANDROID)
        StartSchedua();
        alarmScheduled = true;
#endif
    }

    // Connect signals
    QObject::connect(
        this,
        &CloudSynch::workerInvoked,
        this,
        &CloudSynch::showNotification
        );
}

CloudSynch *CloudSynch::instance()
{
    if(_instance == nullptr) {
        __android_log_print(ANDROID_LOG_DEBUG, "CloudSynch", "Creating CloudSynch instance");
        _instance = new CloudSynch();
    }
    return _instance;
}

void CloudSynch::invoked()
{
    __android_log_print(ANDROID_LOG_DEBUG, "CloudSynch", "invoked() called - emitting workerInvoked");
    emit workerInvoked();
}

void CloudSynch::StartSchedua()
{
#if defined(Q_OS_ANDROID)
    if (m_alarmScheduled) {
        __android_log_print(ANDROID_LOG_DEBUG, "CloudSynch", "Alarm already scheduled, skipping");
        return;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "CloudSynch", "StartSchedua called");

    QJniObject context = QNativeInterface::QAndroidApplication::context();
    if(context.isValid()) {
        __android_log_print(ANDROID_LOG_DEBUG, "CloudSynch", "Context is valid, scheduling alarm");
        QJniObject::callStaticMethod<void>(
            "com/plantdoctor/AlarmHelper",
            "scheduleExactAlarm",
            "(Landroid/content/Context;)V",
            context.object<jobject>()
            );
        m_alarmScheduled = true;
    } else {
        __android_log_print(ANDROID_LOG_ERROR, "CloudSynch", "Context is invalid");
    }
#endif
}

void CloudSynch::printLog()
{
#if defined(Q_OS_ANDROID)
    __android_log_print(ANDROID_LOG_DEBUG, "AlarmHelper", "Hello from AlarmManager!!");
#endif
}

void CloudSynch::showNotification()
{
#if defined(Q_OS_ANDROID)
    __android_log_print(ANDROID_LOG_DEBUG, "CloudSynch", "showNotification called");

    using namespace android::app;

    QJniObject context = QNativeInterface::QAndroidApplication::context();
    if (!context.isValid())
        return;

    QString channelId = "plantdoctor_channel";

    // ─────────────────────────────
    // Channel
    // ─────────────────────────────
    QJniObject channelObj(
        "android/app/NotificationChannel",
        "(Ljava/lang/String;Ljava/lang/CharSequence;I)V",
        QJniObject::fromString(channelId).object<jstring>(),
        QJniObject::fromString("Plant Doctor Alerts").object<jstring>(),
        5
        );

    NotificationChannelProxy channel(channelObj);

    // ─────────────────────────────
    // Manager
    // ─────────────────────────────
    QJniObject managerObj = context.callObjectMethod(
        "getSystemService",
        "(Ljava/lang/String;)Ljava/lang/Object;",
        QJniObject::fromString("notification").object<jstring>()
        );

    NotificationManagerProxy manager(managerObj);
    manager.createNotificationChannel(channel);

    // ─────────────────────────────
    // Builder
    // ─────────────────────────────
    QJniObject builderObj(
        "android/app/Notification$Builder",
        "(Landroid/content/Context;Ljava/lang/String;)V",
        context.object(),
        QJniObject::fromString(channelId).object<jstring>()
        );

    BuilderProxy builder(builderObj);
    builder.setContentTitle(QJniObject::fromString("PlantDoctor").object<jstring>());
    builder.setContentText(QJniObject::fromString("Syncing Data Started").object<jstring>());
    builder.setSmallIcon(17301659);
    builder.setAutoCancel(true);

    // ─────────────────────────────
    // Notify
    // ─────────────────────────────
    NotificationProxy notification = builder.build();
    manager.notify(1, notification);

    __android_log_print(ANDROID_LOG_DEBUG, "CloudSynch", "Emitting startSynchInvoked");
    emit startSynchInvoked();
#endif
}