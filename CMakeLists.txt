cmake_minimum_required(VERSION 3.16)
project(PlantDoctor VERSION 0.1 LANGUAGES CXX)

# ─────────────────────────────────────────────────────────────
# Check for available storage locations
# ─────────────────────────────────────────────────────────────
# Define possible base directories in order of preference
set(POSSIBLE_BASE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}"  # Your friend's location (limited space)
    "/media/csociety/Backup/CISociety"  # Your location (more space)
)

# Find first existing directory
set(WORKSPACE_BASE_DIR "")
foreach(dir IN LISTS POSSIBLE_BASE_DIRS)
    if(EXISTS "${dir}")
        set(WORKSPACE_BASE_DIR "${dir}")
        message(STATUS "Using workspace base directory: ${WORKSPACE_BASE_DIR}")
        break()
    endif()
endforeach()

if(NOT WORKSPACE_BASE_DIR)
    message(FATAL_ERROR "No valid workspace base directory found. Tried: ${POSSIBLE_BASE_DIRS}")
endif()

# ─────────────────────────────────────────────────────────────
# C++ / Qt setup (moved early, unconditional)
# ─────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

# Qt find_package and standard setup first — this is what makes wizard happy
find_package(Qt6 REQUIRED COMPONENTS Quick Multimedia Concurrent Core Network Charts)
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
        ui/features/home/screens/HomeScreen.qml
        ui/features/inference/delegate/HistoryDelegate.qml
        ui/features/sensors/model/SensorSelectionModel.qml
        ui/features/sensors/delegate/SensorSelectionDelegate.qml
        ui/features/settings/Settings.qml
        ui/components/ImagePreview.qml
        ui/components/CircularProgressBar.qml
        ui/components/AvataDrawerTools.qml
        ui/components/AppBar.qml
        ui/features/home/componetes/SideBar.qml
        ui/components/AppBarItemes.qml
        ui/utils/Utils.js
        ui/features/inference/screens/InfarenceResultScreen.qml
        ui/features/inference/screens/InfarenceHistoryScreen.qml
        ui/features/sensors/screens/SensorDashboard.qml
        ui/features/sensors/screens/LiveMonitorScreen.qml
        ui/features/sensors/screens/Hotlines.qml
        ui/features/home/screens/CameraScreen.qml
        ui/features/auth/screens/SignInScreen.qml
        ui/features/auth/screens/SignUpScreen.qml
        ui/components/DHTMeter.qml
        ui/features/devices/DevicesPage.qml
        ui/features/devices/componets/DeviceManu.qml
        ui/features/devices/delegate/DeviceDelegate.qml
        ui/features/settings/screens/SystemSettings.qml

    SOURCES
        src/core/mqtt/mqttclient.cpp
        src/core/mqtt/mqttclient.h
        src/core/mqtt/pnddevice.h
        src/core/mqtt/pnddevice.cpp
        src/core/mqtt/pnddevicemodel.h
        src/core/mqtt/pnddevicemodel.cpp
        src/core/mqtt/pnddeviceconfigurator.h
        src/core/mqtt/pnddeviceconfigurator.cpp
        src/core/utills/helper.h
        src/core/utills/helper.cpp
        src/core/mqtt/pnddevicestate.h
        src/core/mqtt/pndtopics.h
        src/core/mqtt/pndtopics.cpp
        src/core/infarence/infarencerunner.h
        src/core/infarence/infarencerunner.cpp
        src/core/infarence/diseaseinfomanager.h
        src/core/infarence/diseaseinfomanager.cpp
        src/core/synch/cloudsynch.h
        src/core/synch/cloudsynch.cpp
        src/core/rtsp/rtsvideoworker.h
        src/core/rtsp/rtsvideoworker.cpp
        src/core/rtsp/rtsvideooutput.h
        src/core/rtsp/rtsvideooutput.cpp
        QML_FILES ui/features/home/screens/ImagePreviewScreen.qml
        SOURCES src/core/synch/apiclient.h src/core/synch/apiclient.cpp
        SOURCES src/core/synch/apiclient.h src/core/synch/apiclient.cpp



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

        # Check multiple possible locations for qtjenny_generator
        set(POSSIBLE_GENERATOR_DIRS
            "${CMAKE_CURRENT_SOURCE_DIR}/qtjenny_generator"
            "${WORKSPACE_BASE_DIR}/qtjenny_generator"
        )

        set(GENERATOR_DIR "")
        foreach(dir IN LISTS POSSIBLE_GENERATOR_DIRS)
            if(EXISTS "${dir}")
                set(GENERATOR_DIR "${dir}")
                message(STATUS "Found QtJenny generator at: ${GENERATOR_DIR}")
                break()
            endif()
        endforeach()

        if(NOT GENERATOR_DIR)
            message(FATAL_ERROR "QtJenny generator directory not found. Tried: ${POSSIBLE_GENERATOR_DIRS}")
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

    # Check multiple possible locations for qtjenny_output
    set(POSSIBLE_OUTPUT_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/qtjenny_output"
        "${WORKSPACE_BASE_DIR}/qtjenny_output"
    )

    set(JENNY_OUTPUT_DIR "")
    foreach(dir IN LISTS POSSIBLE_OUTPUT_DIRS)
        if(EXISTS "${dir}")
            set(JENNY_OUTPUT_DIR "${dir}/jenny/proxy")
            if(EXISTS "${JENNY_OUTPUT_DIR}")
                message(STATUS "Found QtJenny output at: ${JENNY_OUTPUT_DIR}")
                break()
            endif()
        endif()
    endforeach()

    if(JENNY_OUTPUT_DIR AND EXISTS "${JENNY_OUTPUT_DIR}")
        file(GLOB JENNY_GENERATED_SOURCES "${JENNY_OUTPUT_DIR}/*.h")
        if(JENNY_GENERATED_SOURCES)
            message(STATUS ">>> Using QtJenny proxies from ${JENNY_OUTPUT_DIR} <<<")
        else()
            message(WARNING "No QtJenny proxy headers found in ${JENNY_OUTPUT_DIR}.")
        endif()
    else()
        message(STATUS "QtJenny output directory not found in any location.")
    endif()
else()
    message(STATUS ">>> Building for Desktop Linux <<<")

    # Check multiple possible locations for OpenCV
    set(POSSIBLE_OPENCV_DIRS
        "/media/csociety/Backup/CISociety/Libs/opencv-install/lib/cmake/opencv4"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Libs/opencv-install/lib/cmake/opencv4"
    )

    set(OpenCV_DIR "" CACHE PATH "OpenCV installation directory")
    foreach(dir IN LISTS POSSIBLE_OPENCV_DIRS)
        if(EXISTS "${dir}")
            set(OpenCV_DIR "${dir}" CACHE PATH "OpenCV installation directory" FORCE)
            message(STATUS "Found OpenCV at: ${OpenCV_DIR}")
            break()
        endif()
    endforeach()
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
# Android assets packaging
# ─────────────────────────────────────────────────────────────
if(ANDROID)
    set(ANDROID_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android/src/main/assets")
    set(ANDROID_BUILD_ASSETS_DIR "${CMAKE_BINARY_DIR}/android-build-appPlantDoctor/assets")

    # Create assets directories
    file(MAKE_DIRECTORY "${ANDROID_ASSETS_DIR}/model")
    file(MAKE_DIRECTORY "${ANDROID_ASSETS_DIR}/languages")

    # ─────────────────────────────────────────────────────────
    # 1. Plant Disease Model file (original)
    # ─────────────────────────────────────────────────────────
    set(POSSIBLE_MODEL_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/model"
        "${WORKSPACE_BASE_DIR}/assets/model"
        "${CMAKE_CURRENT_SOURCE_DIR}/model"
    )

    set(MODEL_FOUND FALSE)
    foreach(dir IN LISTS POSSIBLE_MODEL_DIRS)
        set(model_path "${dir}/plant_disease_model.pte")
        if(EXISTS "${model_path}")
            message(STATUS "Found plant disease model at: ${model_path}")

            # Copy to source assets
            file(COPY "${model_path}" DESTINATION "${ANDROID_ASSETS_DIR}/model")

            # Setup post-build copy
            add_custom_command(
                TARGET appPlantDoctor POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory "${ANDROID_BUILD_ASSETS_DIR}/model"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${model_path}"
                    "${ANDROID_BUILD_ASSETS_DIR}/model/plant_disease_model.pte"
                COMMENT ">>> Copying plant disease model to build assets"
            )
            set(MODEL_FOUND TRUE)
            break()
        endif()
    endforeach()

    if(NOT MODEL_FOUND)
        message(WARNING "Plant disease model file not found. Tried: ${POSSIBLE_MODEL_DIRS}")
    endif()

    # ─────────────────────────────────────────────────────────
    # 2. Pest Detection Model file (new)
    # ─────────────────────────────────────────────────────────
    set(POSSIBLE_PEST_MODEL_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/model"
        "${WORKSPACE_BASE_DIR}/assets/model"
        "${CMAKE_CURRENT_SOURCE_DIR}/model"
        "${CMAKE_CURRENT_SOURCE_DIR}/pest_detection"
    )

    set(PEST_MODEL_FOUND FALSE)
    foreach(dir IN LISTS POSSIBLE_PEST_MODEL_DIRS)
        set(pest_model_path "${dir}/pest_detection.pte")
        if(EXISTS "${pest_model_path}")
            message(STATUS "Found pest detection model at: ${pest_model_path}")

            # Copy to source assets
            file(COPY "${pest_model_path}" DESTINATION "${ANDROID_ASSETS_DIR}/model")

            # Setup post-build copy
            add_custom_command(
                TARGET appPlantDoctor POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${pest_model_path}"
                    "${ANDROID_BUILD_ASSETS_DIR}/model/pest_detection.pte"
                COMMENT ">>> Copying pest detection model to build assets"
            )
            set(PEST_MODEL_FOUND TRUE)
            break()
        endif()
    endforeach()

    if(NOT PEST_MODEL_FOUND)
        message(WARNING "Pest detection model file not found. Tried: ${POSSIBLE_PEST_MODEL_DIRS}")
    endif()

    # ─────────────────────────────────────────────────────────
    # 3. Language files
    # ─────────────────────────────────────────────────────────
    set(POSSIBLE_LANG_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/languages"
        "${CMAKE_CURRENT_SOURCE_DIR}/languages"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/core/infarence/languages"
    )

    set(LANGUAGE_FILES
        diseases_en.json
        diseases_ny.json
    )

    set(LANG_FILES_FOUND "")

    foreach(LANG_FILE IN LISTS LANGUAGE_FILES)
        set(FILE_FOUND FALSE)
        foreach(dir IN LISTS POSSIBLE_LANG_DIRS)
            set(lang_path "${dir}/${LANG_FILE}")
            if(EXISTS "${lang_path}")
                message(STATUS "Found language file: ${lang_path}")

                # Copy to source assets
                file(COPY "${lang_path}" DESTINATION "${ANDROID_ASSETS_DIR}/languages")

                # Track for post-build copy
                list(APPEND LANG_FILES_FOUND "${lang_path}")
                set(FILE_FOUND TRUE)
                break()
            endif()
        endforeach()

        if(NOT FILE_FOUND)
            message(WARNING "Language file not found: ${LANG_FILE}")
        endif()
    endforeach()

    # Post-build copy for language files
    if(LANG_FILES_FOUND)
        add_custom_command(
            TARGET appPlantDoctor POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ANDROID_BUILD_ASSETS_DIR}/languages"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${LANG_FILES_FOUND}
                "${ANDROID_BUILD_ASSETS_DIR}/languages/"
            COMMENT ">>> Copying language files to build assets"
        )
    endif()

    # ─────────────────────────────────────────────────────────
    # Summary
    # ─────────────────────────────────────────────────────────
    message(STATUS ">>> Android assets setup complete <<<")
    if(MODEL_FOUND)
        message(STATUS " Plant Disease Model: plant_disease_model.pte")
    else()
        message(STATUS " Plant Disease Model: NOT FOUND")
    endif()

    if(PEST_MODEL_FOUND)
        message(STATUS " Pest Detection Model: pest_detection.onnx")
    else()
        message(STATUS " Pest Detection Model: NOT FOUND")
    endif()

    if(LANG_FILES_FOUND)
        message(STATUS " Languages: ${LANGUAGE_FILES}")
    else()
        message(STATUS " Languages: NOT FOUND")
    endif()
endif()

# ─────────────────────────────────────────────────────────────
# OpenCV setup
# ─────────────────────────────────────────────────────────────
if(ANDROID)
    message(STATUS ">>> Setting up OpenCV for Android <<<")

    # Check multiple possible locations for OpenCV Android SDK
    set(POSSIBLE_OPENCV_ANDROID_DIRS
        "/media/csociety/Backup/CISociety/Libs/OpenCV-android-sdk/sdk/native/jni"
        "${WORKSPACE_BASE_DIR}/src/Libs/OpenCV-android-sdk/sdk/native/jni"
    )

    set(OpenCV_DIR "" CACHE PATH "OpenCV Android directory")
    foreach(dir IN LISTS POSSIBLE_OPENCV_ANDROID_DIRS)
        if(EXISTS "${dir}")
            set(OpenCV_DIR "${dir}" CACHE PATH "OpenCV Android directory" FORCE)
            message(STATUS "Found OpenCV Android SDK at: ${OpenCV_DIR}")
            break()
        endif()
    endforeach()

    if(EXISTS "${OpenCV_DIR}")
        find_package(OpenCV REQUIRED)
        target_include_directories(appPlantDoctor PRIVATE ${OpenCV_INCLUDE_DIRS})
        target_link_libraries(appPlantDoctor PRIVATE ${OpenCV_LIBS})
    else()
        message(FATAL_ERROR "OpenCV Android directory not found. Tried: ${POSSIBLE_OPENCV_ANDROID_DIRS}")
    endif()

    if(EXISTS "${JENNY_OUTPUT_DIR}")
        target_include_directories(appPlantDoctor PRIVATE ${JENNY_OUTPUT_DIR})
    endif()
endif()

# ─────────────────────────────────────────────────────────────
# ExecuTorch setup
# ─────────────────────────────────────────────────────────────
# Check multiple possible locations for ExecuTorch
set(POSSIBLE_EXECUTORCH_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/executorch"
    "${WORKSPACE_BASE_DIR}/executorch"
)

set(EXECUTORCH_DIR "")
foreach(dir IN LISTS POSSIBLE_EXECUTORCH_DIRS)
    if(EXISTS "${dir}")
        set(EXECUTORCH_DIR "${dir}")
        message(STATUS "Found ExecuTorch at: ${EXECUTORCH_DIR}")
        break()
    endif()
endforeach()

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
    message(WARNING "ExecuTorch directory not found. Tried: ${POSSIBLE_EXECUTORCH_DIRS}")
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
        Qt6::Network
        Qt6::qtchartsqml2
)

# ─────────────────────────────────────────────────────────────
# Android packaging
# ─────────────────────────────────────────────────────────────
if(ANDROID)
    # ─────────────────────────────────────────────
    # OpenSSL discovery (Qt 6 Android - team safe)
    # ─────────────────────────────────────────────

    # Allow manual override (highest priority)
    set(OPENSSL_ROOT "" CACHE PATH "Path to android_openssl root")

    # Known locations (team + local + fallback)
    set(POSSIBLE_OPENSSL_DIRS
        "${OPENSSL_ROOT}"
        "$ENV{OPENSSL_ANDROID_ROOT}"
        "${CMAKE_CURRENT_SOURCE_DIR}/android/openssl"
        "${WORKSPACE_BASE_DIR}/android_openssl"
        "/media/csociety/Backup/CISociety/Android/Sdk/android_openssl"
    )

    set(FOUND_OPENSSL_ROOT "")

    foreach(dir IN LISTS POSSIBLE_OPENSSL_DIRS)

        # Case 1: flat ABI layout (Qt standard)
        if(EXISTS "${dir}/arm64-v8a/libssl_3.so")
            set(FOUND_OPENSSL_ROOT "${dir}")
            message(STATUS "Found OpenSSL (flat ABI) at: ${FOUND_OPENSSL_ROOT}")
            break()
        endif()

        # Case 2: ssl_3 wrapper layout (your current setup)
        if(EXISTS "${dir}/ssl_3/arm64-v8a/libssl_3.so")
            set(FOUND_OPENSSL_ROOT "${dir}/ssl_3")
            message(STATUS "Found OpenSSL (ssl_3 layout) at: ${FOUND_OPENSSL_ROOT}")
            break()
        endif()

    endforeach()

    if(NOT FOUND_OPENSSL_ROOT)
        message(FATAL_ERROR
            "OpenSSL not found for Android build.\n\n"
            "Expected one of:\n"
            "  - <path>/arm64-v8a/libssl_3.so\n"
            "  - <path>/ssl_3/arm64-v8a/libssl_3.so\n\n"
            "Fix options:\n"
            "  1) Set -DOPENSSL_ROOT=/path/to/android_openssl\n"
            "  2) export OPENSSL_ANDROID_ROOT=/path/to/android_openssl\n"
            "  3) Put in project: android/openssl\n"
        )
    endif()

    # ─────────────────────────────────────────────
    # Required OpenSSL libraries
    # ─────────────────────────────────────────────

    set(OPENSSL_LIBS
        ${FOUND_OPENSSL_ROOT}/arm64-v8a/libssl_3.so
        ${FOUND_OPENSSL_ROOT}/arm64-v8a/libcrypto_3.so
        ${FOUND_OPENSSL_ROOT}/armeabi-v7a/libssl_3.so
        ${FOUND_OPENSSL_ROOT}/armeabi-v7a/libcrypto_3.so
    )

    # Validate all required binaries exist
    foreach(lib IN LISTS OPENSSL_LIBS)
        if(NOT EXISTS ${lib})
            message(FATAL_ERROR "Missing OpenSSL library: ${lib}")
        endif()
    endforeach()

    # ─────────────────────────────────────────────
    # Attach OpenSSL to Android APK packaging
    # ─────────────────────────────────────────────

    set_target_properties(appPlantDoctor PROPERTIES
        QT_ANDROID_EXTRA_LIBS "${OPENSSL_LIBS}"
    )
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
    QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
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
message(STATUS "Workspace base: ${WORKSPACE_BASE_DIR}")
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
    message(STATUS "OpenCV dir: ${OpenCV_DIR}")
endif()
message(STATUS "Qt version: ${Qt6_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "========================================")
