set(TARGET d3d12-compute-basic)

# Configure example project
phi_add_executable(${TARGET}
                    SOURCES 
                        main.cpp
                        d3d12-compute-basic.h
                        d3d12-compute-basic.cpp
                        
                    SHADERS
                        compute-shader.hlsl
)

# link to app-framework static library
target_link_libraries(${TARGET} PRIVATE 
                            core
                            graphics
                            ${D3D12_LIBRARIES}
                    )
target_include_directories(${TARGET} PRIVATE ${D3D12_INCLUDE_DIRS})