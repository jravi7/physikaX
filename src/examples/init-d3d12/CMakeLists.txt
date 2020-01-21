set(TARGET init-d3d12)

# Configure example project
phi_add_executable(${TARGET}
                    SOURCES 
                        main.cpp
                        d3d12-app.h
                        d3d12-app.cpp)

# link to app-framework static library
target_link_libraries(${TARGET} PRIVATE app-framework)