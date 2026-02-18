cmake_minimum_required(VERSION 3.16)

project(PlantDoctor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appPlantDoctor
    main.cpp
)

qt_add_qml_module(appPlantDoctor
    URI PlantDoctor
    QML_FILES
        Main.qml
        RESOURCES
        QML_FILES ui/app/MainStackView.qml
        QML_FILES ui/features/home/HomeScreen.qml
        QML_FILES ui/features/home/HomePage.qml
        QML_FILES ui/features/camera/CameraScreen.qml
        QML_FILES ui/features/camera/CameraPage.qml
        QML_FILES ui/features/camera/RTSCameraPage.qml
        QML_FILES ui/features/inference/InferenceHistoryPage.qml
        QML_FILES ui/features/inference/InferenceResultPage.qml
        QML_FILES ui/features/inference/model/HistoryModel.qml
        QML_FILES ui/features/inference/delegate/HistoryDelegate.qml
        QML_FILES ui/features/sensors/SensorPage.qml
        QML_FILES ui/features/sensors/model/SensorSelectionModel.qml
        QML_FILES ui/features/sensors/delegate/SensorSelectionDelegate.qml
        QML_FILES ui/features/sensors/componets/DHTMeter.qml
        QML_FILES ui/features/settings/SystemSettings.qml

        RESOURCES
        QML_FILES ui/components/ImagePreview.qml ui/components/CircularProgressBar.qml ui/components/AvataDrawerTools.qml
        QML_FILES ui/utils/Utils.js
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appPlantDoctor PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appPlantDoctor
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appPlantDoctor
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appPlantDoctor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ----------------------------------------
# Android package directory
# ----------------------------------------
set_property(TARGET appPlantDoctor APPEND PROPERTY
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
)
