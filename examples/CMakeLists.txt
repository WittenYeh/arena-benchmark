# Examples CMakeLists.txt

# Automatically find all .cpp files in the current directory
file(GLOB EXAMPLE_SOURCES "*.cpp")

# Create an executable for each .cpp file
foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    # Get the filename without extension
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)

    # Create executable
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})

    # Link with arena_benchmark library
    target_link_libraries(${EXAMPLE_NAME} PRIVATE arena_benchmark)
endforeach()
