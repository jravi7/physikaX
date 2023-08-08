set(TARGET graphics)

set(SOURCES helpers.cpp
            include/graphics/types.h
            include/graphics/helpers.h
            include/graphics/upload-buffer.h
)


phi_add_library(${TARGET} 
                STATIC 
                SOURCES ${SOURCES})
                
target_link_libraries(${TARGET} PRIVATE 
                core
                ${D3D12_LIBRARIES}
        )

target_include_directories(${TARGET} PRIVATE 
                                        ${D3D12_INCLUDE_DIRS} 
                                     PUBLIC 
                                        include
)