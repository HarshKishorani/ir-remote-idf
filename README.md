## Requires arduino-esp32 library of esp idf:
1. Go to your `esp-idf/components` location and run the following commands. 
     - `git clone https://github.com/espressif/arduino-esp32.git arduino`
     - `cd arduino`
     - `git submodule update --init --recursive`

All the code of IRremote library is in `src` folder.
So your `main/CmakeLists.txt` should look like : 
```
# Define where the source files are located
set(SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src")

# Collect all .cpp source files from the source directory
file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.cpp")

idf_component_register(SRCS "main.cpp" ${SOURCE_FILES}
                    INCLUDE_DIRS "." ${SOURCE_DIR}
                    REQUIRES "arduino")
```

NOTE : In `sdkconfig` of project set `CONFIG_FREERTOS_HZ=1000`.

### Refrences : 
- https://docs.espressif.com/projects/arduino-esp32/en/latest/migration_guides/2.x_to_3.0.html
- https://docs.espressif.com/projects/arduino-esp32/en/latest/esp-idf_component.html
- https://github.com/espressif/arduino-esp32
- https://github.com/crankyoldgit/IRremoteESP8266