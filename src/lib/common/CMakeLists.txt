set(TARGET common)

set(SOURCES primitive-generator.cpp
            include/common/mesh-data.h
            include/common/primitive-generator.h
)

phi_add_library(${TARGET} STATIC 
                    SOURCES
                    ${SOURCES})

target_link_libraries(${TARGET} PRIVATE DirectXTK12)

target_include_directories(${TARGET} PUBLIC include)
