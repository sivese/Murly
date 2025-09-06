cmake_minimum_required(VERSION 3.16)
project(Murly)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

set(WEBRTC_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries/webrtc")
# WebRTC include directories
set(WEBRTC_INCLUDE_DIRS
    "${WEBRTC_ROOT_DIR}"
    "${WEBRTC_ROOT_DIR}/third_party/abseil-cpp"
    "${WEBRTC_ROOT_DIR}/third_party/libyuv/include"
)
set(WEBRTC_LIB_DIR "${WEBRTC_ROOT_DIR}/out/Default/obj")


# Core WebRTC libraries
set(WEBRTC_LIBRARIES
    "${WEBRTC_LIB_DIR}/libwebrtc.a"
    "${WEBRTC_LIB_DIR}/api/libaudio_codecs_api.a"
    "${WEBRTC_LIB_DIR}/api/libvideo_codecs_api.a"
    "${WEBRTC_LIB_DIR}/api/libcreate_peerconnection_factory.a"
    "${WEBRTC_LIB_DIR}/pc/libpeerconnection.a"
    "${WEBRTC_LIB_DIR}/media/librtc_media.a"
    "${WEBRTC_LIB_DIR}/modules/libvideo_capture_module.a"
    "${WEBRTC_LIB_DIR}/modules/libaudio_device.a"
    "${WEBRTC_LIB_DIR}/modules/libvideo_coding.a"
    "${WEBRTC_LIB_DIR}/modules/libaudio_coding.a"
    "${WEBRTC_LIB_DIR}/rtc_base/librtc_base.a"
    "${WEBRTC_LIB_DIR}/system_wrappers/libsystem_wrappers.a"
    "${WEBRTC_LIB_DIR}/common_video/libcommon_video.a"
    "${WEBRTC_LIB_DIR}/common_audio/libcommon_audio.a"
)

foreach(LIB ${WEBRTC_LIBRARIES})
    if(NOT EXISTS ${LIB})
        message(WARNING "WebRTC library not found: ${LIB}")
    endif()
endforeach()

# Additional dependencies often needed with WebRTC
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(
    ${WEBRTC_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Source files
set(SOURCES
    main.cc
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${WEBRTC_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    OpenSSL::SSL
    OpenSSL::Crypto
    dl
    rt
    pthread
)

# Compiler flags for WebRTC
target_compile_definitions(${PROJECT_NAME} PRIVATE
    WEBRTC_POSIX
    WEBRTC_LINUX
)

# Debug/Release configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -g -O0)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -O2)
endif()

# Create src directory structure
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)