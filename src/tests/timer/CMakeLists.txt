set(TARGET timer-test)

phi_add_gtest(${TARGET} SOURCES timer-test.cpp)

target_link_libraries(${TARGET} PRIVATE core)