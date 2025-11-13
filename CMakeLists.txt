cmake_minimum_required(VERSION 3.14)

project(LLM)


list(APPEND CMAKE_PREFIX_PATH "F:/Qt/6.5.3/mingw_64/lib/cmake")

find_package(Qt6 COMPONENTS Widgets Network Multimedia REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)



file(GLOB SOURCES_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

file(GLOB HEADS_H ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set(FILES
        ${SOURCES_CPP}
        ${HEADS_H}
        widget.ui
)

add_executable(${PROJECT_NAME} ${FILES}
        Chat.h Chat.cpp
        TextSream.h TextSream.cpp

        resource.qrc
        welcomepage.h welcomepage.cpp welcomepage.ui
        loginpage.h loginpage.cpp loginpage.ui
        agent.h agent.cpp agent.ui
        meetingassistantdialog.h meetingassistantdialog.cpp meetingassistantdialog.ui
        llmchat.h llmchat.cpp llmchat.ui
        temp.ui
        historyuploader.h historyuploader.cpp)



target_link_libraries(LLM PRIVATE Qt6::Core)
target_link_libraries(LLM PRIVATE Qt6::Widgets)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Network Qt6::Multimedia)


list(APPEND CMAKE_PREFIX_PATH "E:/QtProject/LLM/thirdparty/mingw64")
find_package(Qt6Keychain REQUIRED)          # 大小写随意

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6Keychain::Qt6Keychain)



set(CMARK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cmark-gfm")

# 1. 源码头
set(CMARK_INC_SRC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cmark-gfm/src")
# 2. 生成头（cmake 生成的 cmark-gfm_export.h 在这里）
set(CMARK_INC_GEN "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cmark-gfm/build/src")

# 3. 合并
target_include_directories(LLM PRIVATE
    ${CMARK_INC_SRC}
    ${CMARK_INC_GEN}
)

# 4. 库照旧
target_link_libraries(LLM PRIVATE "${CMARK_INC_GEN}/libcmark-gfm.a")
target_link_libraries(LLM PRIVATE Qt6::Core Qt6::Widgets)
