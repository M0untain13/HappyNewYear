cmake_minimum_required(VERSION 3.20)
project(NewYearCountdown VERSION 1.0.0 DESCRIPTION "New Year Countdown Widget")

# Настройка компилятора
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Для Windows специфичные настройки
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
    set(CMAKE_RC_COMPILER_INIT windres)
    enable_language(RC)
endif()

# Создаем исполняемый файл
add_executable(NewYearCountdown WIN32 src/main.cpp)

# Настройки для Windows
if(WIN32)
    # Ссылки на WinAPI библиотеки
    target_link_libraries(NewYearCountdown
        kernel32
        user32
        gdi32
        winspool
        shell32
        ole32
        oleaut32
        uuid
        comdlg32
        advapi32
    )
    
    # Устанавлием иконку (опционально)
    # set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/icon.ico)
    # if(EXISTS ${APP_ICON})
    #     set_target_properties(NewYearCountdown PROPERTIES RC_ICON ${APP_ICON})
    # endif()
endif()

# Настройки установки (опционально)
install(TARGETS NewYearCountdown
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)