#include "cloudsynch.h"
#if defined(Q_OS_ANDROID)
extern "C" JNIEXPORT void JNICALL
Java_com_salesmate_NativeBridge_nativeInvoked(JNIEnv* env, jclass clazz) {
    // 1. Verify Qt environment
    if (!qApp) {
        __android_log_print(ANDROID_LOG_ERROR, "NativeBridge",
                            "Qt application instance not available");
        return;
    }

    // 2. Queue execution if not on main thread
    if (QThread::currentThread() != qApp->thread()) {
        QMetaObject::invokeMethod(qApp, [](){
            CloudSynch::instance()->invoked();
        }, Qt::QueuedConnection);
        return;
    }

    // 3. Direct execution
    try {
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
{
#if defined(Q_OS_ANDROID)
    StartSchedua();
    connect(this, &CloudSynch::workerInvoked, this, &CloudSynch::printLog);
#endif
}

CloudSynch *CloudSynch::instance()
{
    if(_instance == nullptr) {
        _instance = new CloudSynch();
    }
    return _instance;
}

void CloudSynch::invoked()
{
    emit workerInvoked();
}
/**
 * @brief CloudSynch::StartSchedua
 * schedual the allam
 */
void CloudSynch::StartSchedua()
{
#if defined(Q_OS_ANDROID)
    QJniObject context = QNativeInterface::QAndroidApplication::context();
    if(context.isValid()) {
        QJniObject::callStaticMethod<void>(
            "com/plantdoctor/AlarmHelper",
            "scheduleExactAlarm",
            "(Landroid/content/Context;)V",
            context.object<jobject>()
            );
    }
#endif
}
/**
 * @brief CloudSynch::printLog
 */
void CloudSynch::printLog()
{
#if defined(Q_OS_ANDROID)
    __android_log_print(ANDROID_LOG_DEBUG, "AlarmHelper", "Hello from AlarmManager!!");
#endif
}
