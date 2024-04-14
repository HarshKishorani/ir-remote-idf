# Define where the source files are located
set(SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src")

# Collect all .cpp source files from the source directory
file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.cpp")

idf_component_register(SRCS "main.cpp" ${SOURCE_FILES}
                    INCLUDE_DIRS "." ${SOURCE_DIR}
                    REQUIRES "arduino")