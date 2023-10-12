cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)
# Include any extra cmake module here

project(cmake_workshop C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif ()

# Initialize the SDK
pico_sdk_init()

# Compile options, add your compile flags here!
add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
)


# Check if pico w is defined as build target device and not the pico
if (PICO_CYW43_SUPPORTED) # set by BOARD=pico-w
    if (NOT TARGET pico_cyw43_arch)
        message("Skipping build as support is not available")
    else ()
        # Include the library from the GitHub first!
        #
        # The github repo is: https://github.com/HANEmbedded/cmake_workshop_library.git
        #
        # Tip! Use the Fetchcontent module of CMake or download the repo in a subfolder
        # However when asked at end of workshop, there should be a valid reason to choose one over the other.
        #
        # Fetchcontent documentation: https://cmake.org/cmake/help/latest/module/FetchContent.html


        # Add main executable, 
        # determine the location of main source file first!
        # Then look at: https://cmake.org/cmake/help/latest/command/add_executable.html
        

        # enable usb output, disable uart output
        pico_enable_stdio_usb(cmake_workshop_main 1)
        pico_enable_stdio_uart(cmake_workshop_main 0)
	
	    # Link the main executable with the cmake_workshop_library library
        # Take a look at the cmake_workshop_library repo get the library name from the CMakeLists.txt
        # Then look at: https://cmake.org/cmake/help/latest/command/target_link_libraries.html
        
        # Add extra output files to the main executable
        pico_add_extra_outputs(cmake_workshop_main)
    endif ()
endif()
