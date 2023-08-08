set(TARGET renderer)

set(SOURCES 
            camera.cpp
            primitive-generator.cpp
            include/renderer/types.h
            include/renderer/constant-data.h
            include/renderer/camera.h
            include/renderer/primitive-generator.h
)

phi_add_library(${TARGET} STATIC 
                    SOURCES
                    ${SOURCES})

target_link_libraries(${TARGET} PRIVATE 
                                    graphics 
                                PUBLIC
                                    DirectXTK12)

target_include_directories(${TARGET} PUBLIC include)
