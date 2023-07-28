set(TARGET d3d12-lights)

# Configure example project
phi_add_executable(${TARGET}
                    SOURCES 
                        main.cpp
                        d3d12-lights.h
                        d3d12-lights.cpp
                        frame-resource.h
                        frame-resource.cpp
                    SHADERS
                        lighting.hlsl
)

# link to app-framework static library
target_link_libraries(${TARGET} PRIVATE 
                            app-framework
                            logger
                            common
                            d3d12-common
                            utility
                            timer
                            ${D3D12_LIBRARIES}
                    )
target_include_directories(${TARGET} PRIVATE ${D3D12_INCLUDE_DIRS})