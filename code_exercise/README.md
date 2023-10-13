# CMake Workshop student
This is the cmake workshop repository with all the documentation and exercise code. 

## Objective
The objective is to assemble the last pieces of this RPI Pico W firmware. 
When loaded in to a Raspberry Pi Pico W it will connect through WiFi with an external MQTT server.
Another client connected to the server is the laptop of the teacher, running a basic program which keeps track of the messages published and decides who published first.
1. The Pico will then publish a message in a topic containing the groupname.
2. The basic application on the laptop will reply with the leaderboard `position` for this Pico.
3. Pico will blink the `position` times every 3 seconds.

To get you going, there is a quick start guide:

## Step 1: Install the pico toolchain
To be able to compile this code you need to install the pico toolchain, which consists of a code editor, compiler, linker and pico sdk. 
Usage of VS code is highly suggested, as most of the configuration files provided in these repo are preconfigured for VS code. 
However you can still use any other IDE, but you have to make your own launch configuration and environment variables file.

Steps:
1. Install VS code
2. Install JLink Software Suite (needed for debugging the pico w)
3. [Follow this guide for the remaining steps](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)

### Opening the IDE in Windows
Press the Windows key on your keyboard and search for the shortcut `Pico - VS code` which will launch a preconfigured VS code.
Then navigate to `File->Close Folder`, VS code will now close the currently opened project.
Then navigate to the source control tab (third icon down in the bar on the left): Clone this repository.
VS code will then ask to open the folder, say **YES**.


## Step 2: Collaborate with each other using the live share option in VS code
You can work together in one IDE using the live share option in VS code. 
This is highly recommended as it eases collaboration by not having to look with 2/3 people on one screen.
[See this guide for more info.](https://code.visualstudio.com/learn/collaboration/live-share)

## Step 3: Look at the CMakeLists.txt
As you might have noticed the CMakeLists.txt in the root of this folder is not complete yet.
In the CMakeLists.txt there are comment blocks which marks where to add what command. 
But to help visualize the process you're attributing to:

1. CMake script includes the Pico SDK to get the hal drivers and board configuration options `include(include(pico_sdk_import.cmake)`.
2. CMake script creates a new project with the name `cmake_workshop`
3. CMake script sets the c and c++ standard to be used in this project, C11 and C++17
```cmake
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
```
4. CMake script initializes the Pico SDK `pico_sdk_init()`
5. CMake script sets the compile options:
```cmake
add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
)
```
6. CMake script checks if Pico W is set as build target `if (PICO_CYW43_SUPPORTED)`
7. CMake script adds the external MQTT client library **Its your task to figure out how to do this!**
8. CMake script adds the main build target/executable with the name `cmake_workshop_main` **Its your task to figure out the right command**
9. CMake scripts links the library and main executable **Again your task :)**


Check if CMake configures without error, try to do a build right after. If build succeeds go to the next step...

## Step 4: Look at the src/board_definitions.h header file
When you got CMake compiling fill-in the board-definitions.h file. 
Comments state what needs to be added/changed. 
Which are:
- Add the right WiFi_SSID: `cmake_demonstration`
- Add the right WiFi_password: `demopassword!`
- Add your groupname: `somethingvalid`, make sure this uses lowercase characters and - instead of _ and spaces.

## Step 5: Upload
1. Head to the Debug tab (fourth icon on the bar left on screen, with play button and bug besides it). 
2. Select the green launch button at the top of screen with text `Debug J-Link` next to it.

## Step 6: Serial monitor
1. Select on the floating bar the play button, which will launch the application.
2. Select the tab Serial monitor on the bottom next to the output window option.
3. Select the right COM port and click on connect

## Step 7: Await the status
1. If you get PUB: ... ERR 0 it means publishing was a succes!
2. Then you will probably get the message: Got position: x 
Which means you're finished with end position x!


## License
This repository is licensed under the Apache 2.0 license. 
Which means you can do almost anything you want with the files in the repository.
The only requirement is that you mention big changes which can benefit other users, and leave the license banners intact.

Here is the license banner:
```
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
```

See the LICENSE file for the full version.

