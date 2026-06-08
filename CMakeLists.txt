file(GLOB sources "*.cpp")
file(GLOB headers "*.h")

add_library(dataobj ${sources} ${headers})
target_include_directories(dataobj SYSTEM PRIVATE "../")
if (DATA_YAML)
    target_link_libraries(dataobj PRIVATE yaml-cpp)
endif ()

option(DATAOBJ_BUILD_TESTS "Build dataobj tests" ON)

if (DATAOBJ_BUILD_TESTS)
    enable_testing()

    add_executable(dataobj_tests
            tests/Tests.cpp
            tests/Mods.cpp
            tests/Mods.h
    )

    target_link_libraries(dataobj_tests
            PRIVATE
            dataobj
    )

    add_test(
            NAME dataobj_tests
            COMMAND dataobj_tests
    )
endif ()