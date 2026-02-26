cmake_minimum_required(VERSION 3.16)

# Force clean cache and correct ABI
if(ANDROID)
    # Force these to be set before anything else
    set(ANDROID_ABI "arm64-v8a" CACHE STRING "Android ABI" FORCE)
    set(ANDROID_PLATFORM "android-34" CACHE STRING "Android Platform" FORCE)
    set(CMAKE_ANDROID_ARCH_ABI "arm64-v8a" CACHE STRING "Android ABI" FORCE)

    # Clear any cached variables that might cause issues
    unset(CMAKE_TOOLCHAIN_FILE CACHE)
    set(CMAKE_TOOLCHAIN_FILE "/media/csociety/Backup/CISociety/Android/Sdk/ndk/27.2.12479018/build/cmake/android.toolchain.cmake" CACHE PATH "NDK toolchain" FORCE)
endif()

project(PlantDoctor VERSION 0.1 LANGUAGES CXX)

# -------------------------
# C++ / Qt setup
# -------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

# -------------------------
# Qt setup
# -------------------------
find_package(Qt6 REQUIRED COMPONENTS Quick Multimedia Concurrent Core)
qt_standard_project_setup()

# -------------------------
# QtJenny generation (only for Android)
# -------------------------
if(ANDROID)
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        set(gradlew_cmd "gradlew.bat")
    else()
        set(gradlew_cmd "./gradlew")
    endif()

    set(gradlew_arg "--rerun-tasks")
    set(gradlew_task "kaptReleaseKotlin")

    # Run in your generator dir
    execute_process(
        COMMAND ${gradlew_cmd} ${gradlew_arg} ${gradlew_task}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/qtjenny_generator"
        RESULT_VARIABLE gradle_result
    )

    if(NOT gradle_result EQUAL 0)
        message(WARNING "QtJenny generation task failed with code ${gradle_result}. Check Gradle output manually.")
    endif()

    set(JENNY_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/qtjenny_output/jenny/proxy")

    file(GLOB JENNY_GENERATED_SOURCES "${JENNY_OUTPUT_DIR}/*.h")

    if(NOT JENNY_GENERATED_SOURCES)
        message(WARNING "No QtJenny proxy headers found in ${JENNY_OUTPUT_DIR}. Ensure annotations and build.gradle args are correct.")
    endif()
else()
    message(STATUS "QtJenny generation skipped (not Android)")
    set(JENNY_OUTPUT_DIR "")
    set(JENNY_GENERATED_SOURCES "")
endif()

# -------------------------
# Platform-specific configuration
# -------------------------
if(ANDROID)
    message(STATUS ">>> Building for Android <<<")

    # Android NDK toolchain
    set(CMAKE_TOOLCHAIN_FILE "/media/csociety/Backup/CISociety/Android/Sdk/ndk/27.2.12479018/build/cmake/android.toolchain.cmake")

    # Prefer 64-bit ABI to avoid ExecuTorch ARMv7 issues
    set(ANDROID_ABI "arm64-v8a")
    set(ANDROID_PLATFORM "android-34")

    # -------------------------
    # ExecuTorch minimal configuration
    # -------------------------
    set(EXECUTORCH_BUILD_EXTENSION_LLM OFF CACHE BOOL "Disable LLM extension")
    set(EXECUTORCH_BUILD_EXTENSION_EVALUATION OFF CACHE BOOL "Disable evaluation extensions")
    set(EXECUTORCH_BUILD_EXTENSION_RUNNER OFF CACHE BOOL "Disable runner extensions")
    set(EXECUTORCH_BUILD_EXAMPLES OFF CACHE BOOL "Disable examples")
    set(EXECUTORCH_BUILD_TESTS OFF CACHE BOOL "Disable tests")
    set(EXECUTORCH_BUILD_PORTABLE_OPS OFF CACHE BOOL "Disable portable ops for Android")
    set(EXECUTORCH_ENABLE_PORTABLE_KERNELS OFF CACHE BOOL "Disable portable kernels for Android")

    # Enable minimal backends
    set(EXECUTORCH_BUILD_XNNPACK ON CACHE BOOL "Enable XNNPACK backend for Android")
    set(EXECUTORCH_BUILD_QNN OFF CACHE BOOL "")
    set(EXECUTORCH_BUILD_VULKAN OFF CACHE BOOL "")

    # Core high-level extensions
    set(EXECUTORCH_BUILD_EXTENSION_MODULE ON CACHE BOOL "Enable Module API extension")
    set(EXECUTORCH_BUILD_EXTENSION_TENSOR ON CACHE BOOL "Enable Tensor convenience APIs")
    set(EXECUTORCH_BUILD_EXTENSION_DATA_LOADER ON CACHE BOOL "Req for Module")
    set(EXECUTORCH_BUILD_EXTENSION_FLAT_TENSOR ON CACHE BOOL "Req for Module")
    set(EXECUTORCH_BUILD_EXTENSION_NAMED_DATA_MAP ON CACHE BOOL "Req for Module")
    set(EXECUTORCH_ENABLE_LOGGING OFF CACHE BOOL "Disable logging")

else()
    message(STATUS ">>> Building for Desktop Linux <<<")
    set(OpenCV_DIR "/media/csociety/Backup/CISociety/Libs/opencv-install/lib/cmake/opencv4")
endif()

# -------------------------
# Add subdirectories
# -------------------------
add_subdirectory(src/core/qtmqtt)

# -------------------------
# Executable target
# -------------------------
qt_add_executable(appPlantDoctor
    main.cpp
    ${JENNY_GENERATED_SOURCES}
)

# -------------------------
# Android model packaging (after creating the target)
# -------------------------
if(ANDROID)
    set(ANDROID_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android/src/main/assets")
    file(MAKE_DIRECTORY "${ANDROID_ASSETS_DIR}/model")

    # Copy model to source assets
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/model/plant_disease_model.pte"
         DESTINATION "${ANDROID_ASSETS_DIR}/model")

    # Copy model to the build-target assets directory for APK packaging
    set(ANDROID_BUILD_ASSETS_DIR "${CMAKE_BINARY_DIR}/android-build-appPlantDoctor/assets")
    file(MAKE_DIRECTORY "${ANDROID_BUILD_ASSETS_DIR}/model")

    add_custom_command(
        TARGET appPlantDoctor PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${ANDROID_ASSETS_DIR}/model/plant_disease_model.pte"
            "${ANDROID_BUILD_ASSETS_DIR}/model/plant_disease_model.pte"
        COMMENT ">>> Copying model to build-target assets for APK packaging"
    )

    message(STATUS ">>> Model copied to both source and build-target assets directories <<<")
endif()

# -------------------------
# QML module
# -------------------------
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
        ui/utils/Utils.js
)

# Mark Utils.js to suppress warning
set_source_files_properties(ui/utils/Utils.js PROPERTIES
    QT_QML_SKIP_QMLDIR_ENTRY TRUE
)

# -------------------------
# OpenCV setup
# -------------------------
if(ANDROID)
    message(STATUS ">>> Linking OpenCV for Android <<<")
    set(OpenCV_DIR "/media/csociety/Backup/CISociety/Libs/OpenCV-android-sdk/sdk/native/jni")
    find_package(OpenCV REQUIRED)

    target_include_directories(appPlantDoctor PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(appPlantDoctor PRIVATE ${OpenCV_LIBS})

    # Add QtJenny includes for Android
    target_include_directories(appPlantDoctor PRIVATE ${JENNY_OUTPUT_DIR})
endif()

# -------------------------
# Link Qt libraries
# -------------------------
target_link_libraries(appPlantDoctor
    PRIVATE
        Qt6::Quick
        Qt6::Multimedia
        Qt6::Concurrent
        Qt6::Core
        Qt6::Mqtt
)

target_include_directories(appPlantDoctor PRIVATE
    ${LIBTORCH_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

# -------------------------
# ExecuTorch minimal build
# -------------------------
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/src/core/executorch
    ${CMAKE_BINARY_DIR}/executorch_build
    EXCLUDE_FROM_ALL
)

target_link_libraries(appPlantDoctor
    PRIVATE
        executorch
        executorch::backends
        executorch::extensions
)

target_include_directories(appPlantDoctor PRIVATE ${CMAKE_SOURCE_DIR}/src/core/executorch)

# -------------------------
# Android packaging
# -------------------------
if(ANDROID)
    set_property(TARGET appPlantDoctor APPEND PROPERTY
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
    )
endif()

# -------------------------
# Set target properties
# -------------------------
set_target_properties(appPlantDoctor PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# -------------------------
# Install rules
# -------------------------
include(GNUInstallDirs)
install(TARGETS appPlantDoctor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# -------------------------
# Print summary
# -------------------------
message(STATUS "========================================")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
if(ANDROID)
    message(STATUS "Platform: Android (${ANDROID_ABI})")
    if(JENNY_GENERATED_SOURCES)
        message(STATUS "QtJenny: Enabled (${JENNY_OUTPUT_DIR})")
    else()
        message(STATUS "QtJenny: No generated sources found")
    endif()
else()
    message(STATUS "Platform: Desktop Linux")
endif()
message(STATUS "Qt version: ${Qt6_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "========================================")
