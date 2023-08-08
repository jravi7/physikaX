set(TARGET core)

set(SOURCES logger.cpp
            timer.cpp
            application-win32.cpp
            include/core/logger.h
            include/core/timer.h
            include/core/application.h
            include/core/application-win32.h
            include/core/input.h
)

phi_add_library(${TARGET} STATIC SOURCES ${SOURCES})

target_include_directories(${TARGET} PUBLIC include)

#target_compile_definitions(app-framework PUBLIC WIN32_LEAN_AND_MEAN)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
