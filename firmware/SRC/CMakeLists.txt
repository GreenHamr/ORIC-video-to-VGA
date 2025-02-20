# cmake version
cmake_minimum_required(VERSION 3.13)

# include the sdk.cmake file
include(pico_sdk_import.cmake)

# give the project a name (anything you want)
project(vga-graphics-project)

# initialize the sdk
pico_sdk_init()

#######

# name anything you want
add_executable(green_vga_upscaler)

# must match with pio filename and executable name from above
pico_generate_pio_header(green_vga_upscaler ${CMAKE_CURRENT_LIST_DIR}/pio/hsync.pio)
pico_generate_pio_header(green_vga_upscaler ${CMAKE_CURRENT_LIST_DIR}/pio/vsync.pio)
pico_generate_pio_header(green_vga_upscaler ${CMAKE_CURRENT_LIST_DIR}/pio/rgb.pio)
pico_generate_pio_header(green_vga_upscaler ${CMAKE_CURRENT_LIST_DIR}/pio/RGB_Capture.pio)

# must match with executable name and source file names
target_sources(green_vga_upscaler PRIVATE green_vga_upscaler.c vga_graphics.c)

# must match with executable name
target_link_libraries(green_vga_upscaler PRIVATE pico_stdlib hardware_pio hardware_dma hardware_adc hardware_irq pico_multicore)

# must match with executable name
pico_add_extra_outputs(green_vga_upscaler)
