set(TARGET utility)

set(SOURCES camera.cpp
            include/utility/camera.h
)


phi_add_library(${TARGET} 
                STATIC 
                SOURCES ${SOURCES})
                

target_include_directories(${TARGET} PUBLIC include)