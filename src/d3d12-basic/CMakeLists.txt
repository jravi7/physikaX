set(TARGET d3d12-basic)

# Configure example project
phi_add_executable(${TARGET}
                    SOURCES 
                        main.cpp
                        d3d12-basic.h
                        d3d12-basic.cpp
                        vertex-data.h
                        primitive-generator.h
                        primitive-generator.cpp
                        
)

# link to app-framework static library
target_link_libraries(${TARGET} PRIVATE 
                            app-framework
                            logger
                            ${D3D12_LIBRARIES}
                    )
target_include_directories(${TARGET} PRIVATE ${D3D12_INCLUDE_DIRS})