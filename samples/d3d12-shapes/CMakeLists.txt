set(TARGET d3d12-shapes)

# Configure example project
phi_add_executable(${TARGET}
                    SOURCES 
                        main.cpp
                        d3d12-shapes.h
                        d3d12-shapes.cpp
                        frame-resource.h
                        frame-resource.cpp
                    SHADERS
                        shapes.hlsl
)

# link to app-framework static library
target_link_libraries(${TARGET} PRIVATE 
                            core
                            graphics
                            renderer
                            ${D3D12_LIBRARIES}
                    )
target_include_directories(${TARGET} PRIVATE ${D3D12_INCLUDE_DIRS})