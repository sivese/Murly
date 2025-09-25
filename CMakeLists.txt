cmake_minimum_required(VERSION 3.16)
project(Murly VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable debug symbols in Debug mode
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set compiler flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# Find required packages
if(WIN32)
# For Windows, please install Boost libraries manually.
# Then set BOOST_ROOT and BOOST_LIBRARYDIR environment variables if Boost is not found
    find_package(Boost REQUIRED COMPONENTS thread filesystem)
else()
    find_package(Boost REQUIRED COMPONENTS thread filesystem system)
endif()
find_package(Threads REQUIRED)

# Define source files explicitly (better than globbing)
set(CHAT_SOURCES
    src/chat/message.cc
    src/chat/room.cc
    src/chat/session.cc
    src/chat/server.cc
    src/chat/client.cc

    src/web/request.cc
    src/web/response.cc
    src/web/utils.cc
    
    src/app/chat.cc
)

# Create the main executable
add_executable(chatter
    main.cc
    ${CHAT_SOURCES}
)

# Set target properties
set_target_properties(chatter PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Link libraries
target_link_libraries(chatter 
    PRIVATE
        ${Boost_LIBRARIES}
        Threads::Threads
)

# Include directories
target_include_directories(chatter 
    PRIVATE 
        include
        ${Boost_INCLUDE_DIRS}
)

# Add compile definitions
target_compile_definitions(chatter
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
)