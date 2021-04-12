add_executable (
    shiny-advanced
        Advanced.cpp
        Main.cpp
        Shared.h
)

target_link_libraries (shiny-advanced das-shiny)
