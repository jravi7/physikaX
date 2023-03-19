set(TARGET logger)

set(SOURCES logger.cpp
            include/logger/logger.h
)


phi_add_library(${TARGET} 
                STATIC 
                SOURCES ${SOURCES})
                

target_include_directories(${TARGET} PUBLIC include)