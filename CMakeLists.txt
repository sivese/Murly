cmake_minimum_required(VERSION 3.16)
project(Murly)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Boost 라이브러리 찾기
find_package(Boost REQUIRED COMPONENTS system thread)

# 서버 실행파일
add_executable(chat_server 
    src/*.cc
)

# Boost 라이브러리 링크
target_link_libraries(chat_server ${Boost_LIBRARIES})

# 헤더 파일 경로 추가
target_include_directories(chat_server PRIVATE include ${Boost_INCLUDE_DIRS})