cmake_minimum_required(VERSION 3.16)
project(PlantDoctor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.8)

if(ANDROID)
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        set(gradlew_cmd "gradlew.bat")
    else()
        set(gradlew_cmd "./gradlew")
    endif()

    set(gradlew_arg "--rerun-tasks")
    set(gradlew_task "kaptReleaseKotlin")

    # Run in your generator dir (adjust if you have multiple subdirs)
    execute_process(
        COMMAND ${gradlew_cmd} ${gradlew_arg} ${gradlew_task}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/qtjenny_generator"
        RESULT_VARIABLE gradle_result
    )

if(NOT gradle_result EQUAL 0)
    message(WARNING "QtJenny generation task failed with code ${gradle_result}. Check Gradle output manually.")
endif()

else()
    message(STATUS "QtJenny generation skipped (not Android)")
endif()
set(JENNY_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/qtjenny_output/jenny/proxy")

file(GLOB JENNY_GENERATED_SOURCES "${JENNY_OUTPUT_DIR}/*.h")

if(NOT JENNY_GENERATED_SOURCES)
    message(WARNING "No QtJenny proxy headers found in ${JENNY_OUTPUT_DIR}. Ensure annotations and build.gradle args are correct.")
endif()


qt_add_executable(appPlantDoctor
    main.cpp
    ${JENNY_GENERATED_SOURCES}
)

qt_add_qml_module(appPlantDoctor
    URI PlantDoctor
    VERSION 1.0
    QML_FILES
    Main.qml
    ui/app/MainStackView.qml
    ui/features/home/HomeScreen.qml
    ui/features/home/HomePage.qml
    ui/features/camera/CameraScreen.qml
    ui/features/camera/CameraPage.qml
    ui/features/camera/RTSCameraPage.qml
    ui/features/inference/InferenceHistoryPage.qml
    ui/features/inference/InferenceResultPage.qml
    ui/features/inference/model/HistoryModel.qml
    ui/features/inference/delegate/HistoryDelegate.qml
    ui/features/sensors/SensorPage.qml
    ui/features/sensors/model/SensorSelectionModel.qml
    ui/features/sensors/delegate/SensorSelectionDelegate.qml
    ui/features/sensors/componets/DHTMeter.qml
    ui/features/settings/SystemSettings.qml
    ui/components/ImagePreview.qml
    ui/components/CircularProgressBar.qml
    ui/components/AvataDrawerTools.qml
    ui/utils/Utils.js
)

# Silence Utils.js warning
set_source_files_properties(ui/utils/Utils.js PROPERTIES QT_QML_SKIP_QMLDIR_ENTRY TRUE)

set_target_properties(appPlantDoctor PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING $$   {PROJECT_VERSION_MAJOR}.   $${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appPlantDoctor PRIVATE Qt6::Quick)

if(ANDROID)
    target_include_directories(appPlantDoctor PRIVATE ${JENNY_OUTPUT_DIR})
endif()

set_property(TARGET appPlantDoctor APPEND PROPERTY
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
)

include(GNUInstallDirs)
install(TARGETS appPlantDoctor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
