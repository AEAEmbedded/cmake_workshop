cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(cmake_workshop C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif ()

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
)

if (PICO_CYW43_SUPPORTED) # set by BOARD=pico-w
    if (NOT TARGET pico_cyw43_arch)
        message("Skipping build as support is not available")
    else ()
        # Include the library from the GitHub first!
        # See the lib/CMakeLists.txt
        # 
        # Tip! Use the Fetchcontent module of CMake

        
        # Add main executable, 
        # determine the location of main source file first!
 
        # enable usb output, disable uart output
        pico_enable_stdio_usb(pico_mqtt_client 1)
        pico_enable_stdio_uart(pico_mqtt_client 0)

		# Add the include directories for main executable!
        
		# Link the main executable with the rpi_pico_mqtt library

        pico_add_extra_outputs(pico_mqtt_client)


    endif ()
endif()
