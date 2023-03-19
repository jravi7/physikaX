set(TARGET timer)

set(SOURCES timer.cpp
            include/timer/timer.h
)

phi_add_library(${TARGET} 
                STATIC 
                SOURCES
                    ${SOURCES})

target_include_directories(${TARGET} PUBLIC include)

#TEST
set(TEST_SOURCES test/timer-test.cpp)

phi_add_gtest(${TARGET}-test SOURCES ${TEST_SOURCES})
target_link_libraries(${TARGET}-test PRIVATE ${TARGET})