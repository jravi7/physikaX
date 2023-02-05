set(TARGET d3d12-common)

set(SOURCES d3d12-helpers.cpp
            include/d3d12-common/d3d12-helpers.h
            include/d3d12-common/d3d12-common.h
)


phi_add_library(${TARGET} 
                STATIC 
                SOURCES ${SOURCES})
                
target_link_libraries(${TARGET} PRIVATE 
                logger
                ${D3D12_LIBRARIES}
        )

target_include_directories(${TARGET} PRIVATE 
                                        ${D3D12_INCLUDE_DIRS} 
                                     PUBLIC 
                                        include
)