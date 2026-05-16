cmake_minimum_required(VERSION 3.16)
project(simple-notepad-project VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport)

set(SOURCES
        main.cpp
        main_window.cpp
        text_transform.cpp
        find_replace_dialog.cpp
        word_frequency_dialog.cpp
        sort.cpp
        spell_checker.cpp
        line_number_area.cpp
)

set(HEADERS
        main_window.h
        text_transform.h
        find_replace_dialog.h
        word_frequency_dialog.h
        sort.h
        notepad_exception.h
        spell_checker.h
        line_number_area.h
)

set(UI_FILES
        find_replace_dialog.ui
        word_frequency_dialog.ui
)

qt6_wrap_ui(UI_HEADERS ${UI_FILES})

add_executable(notepad
        ${SOURCES}
        ${HEADERS}
        ${UI_FILES}
)

target_link_libraries(notepad PRIVATE Qt6::Widgets Qt6::PrintSupport)