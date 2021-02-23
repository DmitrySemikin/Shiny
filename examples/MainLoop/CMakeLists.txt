add_executable (
    shiny-main-loop
        MainLoop.cpp
)

target_link_libraries (shiny-main-loop das-shiny)
