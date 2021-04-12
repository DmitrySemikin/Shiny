project(example-01-trivial)

add_executable (
    ${PROJECT_NAME}
        example-01.cpp
        ../sleep-milliseconds/sleepMilliseconds.cpp
)

target_include_directories(
    ${PROJECT_NAME}
        PRIVATE "../sleep-milliseconds"
)

target_link_libraries (
    ${PROJECT_NAME}
        das-shiny
)