cmake_minimum_required(VERSION 3.16)
project(PlantDoctor VERSION 0.1 LANGUAGES CXX)

# ─────────────────────────────────────────────────────────────
# C++ / Qt setup (moved early, unconditional)
# ─────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

# Qt find_package and standard setup first — this is what makes wizard happy
find_package(Qt6 REQUIRED COMPONENTS Quick Multimedia Concurrent Core)
qt_standard_project_setup(REQUIRES 6.8)

# ─────────────────────────────────────────────────────────────
# Main targets FIRST (wizard needs these to be simple/top-level)
# ─────────────────────────────────────────────────────────────
qt_add_executable(appPlantDoctor
    main.cpp
    Resources.qrc
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
        ui/features/settings/SystemSettings.qml
        ui/components/ImagePreview.qml
        ui/components/CircularProgressBar.qml
        ui/components/AvataDrawerTools.qml
        ui/app/AppBar.qml
    SOURCES
        src/core/mqtt/mqttclient.cpp
        src/core/mqtt/mqttclient.h

)

# ─────────────────────────────────────────────────────────────
# QtJenny generation (only for Android) - MANUAL CONTROL
# (moved after targets — safe, doesn't affect wizard)
# ─────────────────────────────────────────────────────────────
set(QTJENNY_GENERATE FALSE CACHE BOOL "Set to TRUE to regenerate QtJenny proxies (requires Gradle)")

set(JENNY_OUTPUT_DIR "")
set(JENNY_GENERATED_SOURCES "")

if(ANDROID)
    message(STATUS ">>> Building for Android <<<")

    if(QTJENNY_GENERATE)
        message(STATUS ">>> QtJenny generation ENABLED (manual flag) <<<")
        set(GENERATOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/qtjenny_generator")
        if(NOT EXISTS "${GENERATOR_DIR}")
            message(FATAL_ERROR "QtJenny generator directory not found: ${GENERATOR_DIR}")
        endif()
        if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
            set(gradlew_cmd "gradlew.bat")
        else()
            set(gradlew_cmd "./gradlew")
        endif()
        if(NOT EXISTS "${GENERATOR_DIR}/${gradlew_cmd}" AND NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
            message(FATAL_ERROR "gradlew not found in ${GENERATOR_DIR}")
        endif()
        set(gradlew_arg "--rerun-tasks")
        set(gradlew_task "kaptReleaseKotlin")
        execute_process(
            COMMAND ${gradlew_cmd} ${gradlew_arg} ${gradlew_task}
            WORKING_DIRECTORY "${GENERATOR_DIR}"
            RESULT_VARIABLE gradle_result
        )
        if(NOT gradle_result EQUAL 0)
            message(WARNING "QtJenny generation task failed with code ${gradle_result}. Check Gradle output manually.")
        else()
            message(STATUS ">>> QtJenny generation completed successfully <<<")
        endif()
    else()
        message(STATUS ">>> QtJenny generation SKIPPED (manual flag is FALSE) <<<")
    endif()

    set(JENNY_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/qtjenny_output/jenny/proxy")
    if(EXISTS "${JENNY_OUTPUT_DIR}")
        file(GLOB JENNY_GENERATED_SOURCES "${JENNY_OUTPUT_DIR}/*.h")
        if(JENNY_GENERATED_SOURCES)
            message(STATUS ">>> Using QtJenny proxies from ${JENNY_OUTPUT_DIR} <<<")
        else()
            message(WARNING "No QtJenny proxy headers found in ${JENNY_OUTPUT_DIR}.")
        endif()
    else()
        message(STATUS "QtJenny output directory not found: ${JENNY_OUTPUT_DIR}")
    endif()
else()
    message(STATUS ">>> Building for Desktop Linux <<<")
    set(OpenCV_DIR "/media/csociety/Backup/CISociety/Libs/opencv-install/lib/cmake/opencv4" CACHE PATH "OpenCV installation directory")
endif()

# Add generated sources to executable (after definition)
if(JENNY_GENERATED_SOURCES)
    target_sources(appPlantDoctor PRIVATE ${JENNY_GENERATED_SOURCES})
endif()

# ─────────────────────────────────────────────────────────────
# Add subdirectories
# ─────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/core/qtmqtt")
    add_subdirectory(src/core/qtmqtt)
else()
    message(WARNING "qtmqtt subdirectory not found")
endif()

# ─────────────────────────────────────────────────────────────
# Android model packaging
# ─────────────────────────────────────────────────────────────
if(ANDROID)
    set(ANDROID_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android/src/main/assets")
    set(MODEL_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/assets/model/plant_disease_model.pte")
    if(EXISTS "${MODEL_SOURCE}")
        file(MAKE_DIRECTORY "${ANDROID_ASSETS_DIR}/model")
        file(COPY "${MODEL_SOURCE}" DESTINATION "${ANDROID_ASSETS_DIR}/model")

        set(ANDROID_BUILD_ASSETS_DIR "${CMAKE_BINARY_DIR}/android-build-appPlantDoctor/assets")
        add_custom_command(
            TARGET appPlantDoctor POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ANDROID_BUILD_ASSETS_DIR}/model"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MODEL_SOURCE}"
                "${ANDROID_BUILD_ASSETS_DIR}/model/plant_disease_model.pte"
            COMMENT ">>> Copying model to build-target assets for APK packaging"
        )
        message(STATUS ">>> Model setup complete <<<")
    else()
        message(WARNING "Model file not found: ${MODEL_SOURCE}")
    endif()
endif()

# ─────────────────────────────────────────────────────────────
# OpenCV setup
# ─────────────────────────────────────────────────────────────
if(ANDROID)
    message(STATUS ">>> Setting up OpenCV for Android <<<")
    set(OpenCV_DIR "/media/csociety/Backup/CISociety/Libs/OpenCV-android-sdk/sdk/native/jni" CACHE PATH "OpenCV Android directory")
    if(EXISTS "${OpenCV_DIR}")
        find_package(OpenCV REQUIRED)
        target_include_directories(appPlantDoctor PRIVATE ${OpenCV_INCLUDE_DIRS})
        target_link_libraries(appPlantDoctor PRIVATE ${OpenCV_LIBS})
    else()
        message(FATAL_ERROR "OpenCV Android directory not found: ${OpenCV_DIR}")
    endif()
    if(EXISTS "${JENNY_OUTPUT_DIR}")
        target_include_directories(appPlantDoctor PRIVATE ${JENNY_OUTPUT_DIR})
    endif()
endif()

# ─────────────────────────────────────────────────────────────
# ExecuTorch setup
# ─────────────────────────────────────────────────────────────
set(EXECUTORCH_DIR "${CMAKE_SOURCE_DIR}/src/core/executorch")
if(EXISTS "${EXECUTORCH_DIR}")
    if(ANDROID)
        set(EXECUTORCH_BUILD_EXTENSION_LLM OFF CACHE BOOL "Disable LLM extension")
        set(EXECUTORCH_BUILD_EXTENSION_EVALUATION OFF CACHE BOOL "Disable evaluation extensions")
        set(EXECUTORCH_BUILD_EXTENSION_RUNNER OFF CACHE BOOL "Disable runner extensions")
        set(EXECUTORCH_BUILD_EXAMPLES OFF CACHE BOOL "Disable examples")
        set(EXECUTORCH_BUILD_TESTS OFF CACHE BOOL "Disable tests")
        set(EXECUTORCH_BUILD_PORTABLE_OPS OFF CACHE BOOL "Disable portable ops for Android")
        set(EXECUTORCH_ENABLE_PORTABLE_KERNELS OFF CACHE BOOL "Disable portable kernels for Android")
        set(EXECUTORCH_BUILD_XNNPACK ON CACHE BOOL "Enable XNNPACK backend for Android")
        set(EXECUTORCH_BUILD_QNN OFF CACHE BOOL "")
        set(EXECUTORCH_BUILD_VULKAN OFF CACHE BOOL "")
        set(EXECUTORCH_BUILD_EXTENSION_MODULE ON CACHE BOOL "Enable Module API extension")
        set(EXECUTORCH_BUILD_EXTENSION_TENSOR ON CACHE BOOL "Enable Tensor convenience APIs")
        set(EXECUTORCH_BUILD_EXTENSION_DATA_LOADER ON CACHE BOOL "Req for Module")
        set(EXECUTORCH_BUILD_EXTENSION_FLAT_TENSOR ON CACHE BOOL "Req for Module")
        set(EXECUTORCH_BUILD_EXTENSION_NAMED_DATA_MAP ON CACHE BOOL "Req for Module")
        set(EXECUTORCH_ENABLE_LOGGING OFF CACHE BOOL "Disable logging")
    endif()
    add_subdirectory(
        ${EXECUTORCH_DIR}
        ${CMAKE_BINARY_DIR}/executorch_build
        EXCLUDE_FROM_ALL
    )
    target_link_libraries(appPlantDoctor
        PRIVATE
            executorch
            executorch::backends
            executorch::extensions
    )
    target_include_directories(appPlantDoctor PRIVATE ${EXECUTORCH_DIR})
else()
    message(WARNING "ExecuTorch directory not found: ${EXECUTORCH_DIR}")
endif()

# ─────────────────────────────────────────────────────────────
# Link Qt libraries (after all targets/subdirs)
# ─────────────────────────────────────────────────────────────
target_link_libraries(appPlantDoctor
    PRIVATE
        Qt6::Quick
        Qt6::Multimedia
        Qt6::Concurrent
        Qt6::Core
        Qt6::Mqtt
)

# ─────────────────────────────────────────────────────────────
# Android packaging
# ─────────────────────────────────────────────────────────────
if(ANDROID)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/android")
        set_property(TARGET appPlantDoctor APPEND PROPERTY
            QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
        )
    else()
        message(WARNING "Android package source directory not found")
    endif()
endif()

# ─────────────────────────────────────────────────────────────
# Set target properties
# ─────────────────────────────────────────────────────────────
set_target_properties(appPlantDoctor PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# ─────────────────────────────────────────────────────────────
# Install rules
# ─────────────────────────────────────────────────────────────
include(GNUInstallDirs)
install(TARGETS appPlantDoctor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ─────────────────────────────────────────────────────────────
# Print summary
# ─────────────────────────────────────────────────────────────
message(STATUS "========================================")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
if(ANDROID)
    message(STATUS "Platform: Android")
    if(DEFINED ANDROID_ABI)
        message(STATUS "ABI: ${ANDROID_ABI}")
    endif()
    if(JENNY_GENERATED_SOURCES)
        message(STATUS "QtJenny: Enabled (${JENNY_OUTPUT_DIR})")
    else()
        message(STATUS "QtJenny: Disabled or no sources found")
    endif()
else()
    message(STATUS "Platform: Desktop Linux")
endif()
message(STATUS "Qt version: ${Qt6_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "========================================")
