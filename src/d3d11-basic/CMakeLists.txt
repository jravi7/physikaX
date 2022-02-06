set(TARGET d3d11-basic)

# Configure example project
phi_add_executable(${TARGET}
                    SOURCES 
                        main.cpp
                        d3d11-basic.h
                        d3d11-basic.cpp)

# link to app-framework static library
target_link_libraries(${TARGET} PRIVATE 
                            app-framework
                            logger
                    )
