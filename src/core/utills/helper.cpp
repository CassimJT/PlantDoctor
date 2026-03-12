#include "helper.h"
// For Android

Helper::Helper(QObject *parent)
    : QObject{parent},
    m_localPath(""),
    m_imagePath(""),
    isHompage(false)        // Initialize the member
{
   //setStatusBarAppearance(QColor(Qt::transparent), true);
}

/**
 * @brief Helper::imageToDataUrl
 * @param image
 */
void Helper::imageToDataUrl(const QImage &image)
{
    QtConcurrent::run([this, image]() {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG");

        QString encoded = "data:image/png;base64," + ba.toBase64();
        QMetaObject::invokeMethod(this, [this, encoded]() {
            m_imagePath = encoded;
            m_localPath.clear();
            emit imageReady();
        }, Qt::QueuedConnection);
    });
}

/**
 * @brief Helper::loadImageFromContentUriAsync
 * @param uriString
 */
void Helper::loadImageFromContentUri(const QString &uriString)
{
    // Run heavy work in background
    QtConcurrent::run([this, uriString]() {
#if defined(Q_OS_ANDROID)
        // 1) call Java helper to get Base64 string
        QJniObject activity = QNativeInterface::QAndroidApplication::context();
        if (!activity.isValid()) {
            qWarning() << "Helper: no Android activity";
            return;
        }

        QJniObject base64Str = QJniObject::callStaticObjectMethod(
            "com/plantGPT/GalleryHelper",
            "loadContentUri",
            "(Landroid/app/Activity;Ljava/lang/String;)Ljava/lang/String;",
            activity.object<jobject>(),
            QJniObject::fromString(uriString).object<jstring>()
            );

        if (!base64Str.isValid()) {
            qWarning() << "Helper: Java returned invalid string";
            return;
        }

        QString encoded = base64Str.toString();
        if (encoded.isEmpty()) {
            qWarning() << "Helper: Java returned empty Base64";
            return;
        }

        // 2) decode Base64 to raw bytes
        QByteArray imageData = QByteArray::fromBase64(encoded.toUtf8());
#else
        // On desktop, uriString may already be a local path or file URL
        QByteArray imageData;
        if (uriString.startsWith("file://")) {
            QString filePath = QUrl(uriString).toLocalFile();
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly)) {
                imageData = file.readAll();
                file.close();
            }
        } else {
            QFile file(uriString);
            if (file.open(QIODevice::ReadOnly)) {
                imageData = file.readAll();
                file.close();
            }
        }
#endif

        if (imageData.isEmpty()) {
            qWarning() << "Helper: decoded image data is empty";
            return;
        }

        // 3) write to a temp file in cache
        QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        if (cacheDir.isEmpty()) {
            // fallback to writableLocation Temp
            cacheDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        }
        QDir dir(cacheDir);
        if (!dir.exists())
            dir.mkpath(".");

        // create a unique filename
        QString fileName = QString("picked_%1.png").arg(QUuid::createUuid().toString());
        QString fullPath = dir.filePath(fileName);

        QFile outFile(fullPath);
        if (!outFile.open(QIODevice::WriteOnly)) {
            qWarning() << "Helper: failed to open file for writing:" << fullPath;
            return;
        }
        outFile.write(imageData);
        outFile.close();

        // 4) update m_localPath on the GUI thread and emit signal
        QMetaObject::invokeMethod(this, [this, fullPath]() {
            // Use file:// URL in QML Image source or raw path depending on your UI
            m_localPath = QUrl::fromLocalFile(fullPath).toString(); // "file:///..."
            m_imagePath.clear();
            emit imageReady();
        }, Qt::QueuedConnection);
    });
}

/**
 * @brief Helper::imagePreview
 * @return
 */
QString Helper::imagePreview()
{
    return m_imagePath;
}
/**
 * @brief Helper::setAnAndroidSystemBarTextColor
 */
void Helper::setStatusBarAppearance(const QColor &backgroundColor, bool darkIcons)
{
#if defined(Q_OS_ANDROID)
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([=]() -> QVariant {
        QJniObject activity = QNativeInterface::QAndroidApplication::context();
        if (!activity.isValid()) {
            qWarning() << "setStatusBarAppearance: No valid Android activity";
            return QVariant(false);
        }

        QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
        if (!window.isValid()) {
            qWarning() << "setStatusBarAppearance: No valid window";
            return QVariant(false);
        }

        jint sdkVersion = QJniObject::getStaticField<jint>("android/os/Build$VERSION", "SDK_INT");

        // 1. Enable drawing behind system bars + make status bar TRANSPARENT (best for edge-to-edge)
        window.callMethod<void>("addFlags", "(I)V", 0x80000000);           // FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS
        window.callMethod<void>("clearFlags", "(I)V", 0x04000000);         // clear FLAG_TRANSLUCENT_STATUS

        // Use the color you pass (ARGB format for JNI)
        jint statusColor = backgroundColor.alpha() << 24 |
                           backgroundColor.red()   << 16 |
                           backgroundColor.green() << 8  |
                           backgroundColor.blue();

        window.callMethod<void>("setStatusBarColor", "(I)V", statusColor);

        // 2. Force icon/text appearance (dark icons = light status bar appearance)
        if (sdkVersion >= 30) {
            // Preferred modern way (API 30+ / Android 11+)
            QJniObject insetsController = window.callObjectMethod("getInsetsController", "()Landroid/view/WindowInsetsController;");
            if (insetsController.isValid()) {
                jint appearance = darkIcons ? 0x00000008 : 0x00000000;  // APPEARANCE_LIGHT_STATUS_BARS = 0x08
                insetsController.callMethod<void>("setSystemBarsAppearance", "(II)V", appearance, 0x00000008);
            }
        }

        if (sdkVersion >= 23 && sdkVersion < 30) {
            // Legacy fallback (API 23–29)
            QJniObject decorView = window.callObjectMethod("getDecorView", "()Landroid/view/View;");
            if (decorView.isValid()) {
                jint uiFlags = decorView.callMethod<jint>("getSystemUiVisibility", "()I");
                if (darkIcons) {
                    uiFlags |= 0x00002000;  // SYSTEM_UI_FLAG_LIGHT_STATUS_BAR
                } else {
                    uiFlags &= ~0x00002000;
                }
                decorView.callMethod<void>("setSystemUiVisibility", "(I)V", uiFlags);
            }
        }

        return QVariant(true);
    }).waitForFinished();
#else
    Q_UNUSED(backgroundColor);
    Q_UNUSED(darkIcons);
#endif
}

/**
 * @brief Helper::localFilePath
 * @return
 */
QString Helper::localFilePath()
{
    return m_localPath;
}

/**
 * @brief Helper::getIsHompage
 * @return
 */
bool Helper::getIsHompage() const
{
    return isHompage;
}

/**
 * @brief Helper::setIsHompage
 * @param newIsHompage
 */
void Helper::setIsHompage(bool newIsHompage)
{
    if (isHompage == newIsHompage)
        return;
    isHompage = newIsHompage;
    emit isHompageChanged();
}
