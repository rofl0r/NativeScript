//////////////////////////////////////////////////
////////// INSTALLATION AND USER GUIDE ///////////
//////////////////////////////////////////////////

The installation mainly consist of getting all the dependencies. Once the dependencies are installed, cmake can be used to create makefile (Linux) or MSVC project (Windows).




//////////////////////////////////////////////////
///////////////// DEPENDENCIES ///////////////////

We present guide to getting the dependency binaries for Ubuntu 14.04 and Windows 7. If the binaries aren't available, we present compilation guide to create them manually.

When building for Windows, download 32bit version of the libraries, since Mono isn't easily available on Windows in 64bit version.

This guide describes getting the same versions we used. You can try to use newer versions, but we cannot guarantee compatibility.

We would like to note, that the dependency installation guide might not work if e.g. some of the binaries or underlying guides change. It should be taken only as an convenient help in getting the dependencies, since we cannot ensure its correctness.


///////////////// Flex and Bison /////////////////

On Ubuntu (and most other Linux versions), these libraries are available trough the package system:

> apt-get install flex bison

On Windows, binaries are available on sourcefoge: http://sourceforge.net/projects/winflexbison/. Add the path to the binaries to system variable PATH.


////////////////////// LLVM //////////////////////

Binaries for Windows and some other platforms are available on the llvm download site: http://llvm.org/releases/download.html. Download binaries for clang version 3.7.0 and install them. LLVM libraries are included.

On Ubuntu LLVM libraries are available trough packages, although it might be necessary to set LLVM repository first:

> wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key|sudo apt-key add -
> apt-get install libllvm3.7 llvm-3.7-dev


/////////////////////// V8 ///////////////////////

V8 libraries are available trough Ubuntu package system:

> apt-get install libv8 libv8-dev


On Windows, it must be built from source. The guide is available here https://github.com/v8/v8/wiki/Using%20Git. We present the necessary steps here:

1) First get depot_tools using this guide: http://dev.chromium.org/developers/how-tos/install-depot-tools
Cygwin isn't mandatory, but can be used if it is convenient for you.

2) Start cmd.exe} and run depot_tools to make sure it's up to date:

> gclient

3) Make sure you are in a directory you selected for building V8. To get V8 and go into its directory run:

> fetch v8
> cd v8

4) Generate MSVC project using GYP. Official guide is here: https://github.com/v8/v8/wiki/Building%20with%20Gyp#visual-studio. You need python for this step. Just run:

> python build\gyp_v8

5) To build only V8 libraries (nothing more is required for this work), go to folder tools\gyp and open v8.sln.

6) Select Release configuration and build the solution.


///////////////////// Mono ///////////////////////

On Linux, just follow the official guide: http://www.mono-project.com/docs/getting-started/install/linux/. Install the complete version using:

> apt-get install mono-complete

On Windows, download and install the 32bit version: http://www.mono-project.com/download/#download-win.


////////////////////// Lua ///////////////////////

On Linux, use package system:

> apt-get install lua5.3

On Windows, libraries are available here: http://sourceforge.net/projects/luabinaries/files/5.3/Windows%20Libraries/. Select static and download version lua-5.3_Win32_vc10_lib.zip.


////////////////////// SDL ///////////////////////

On Linux, SDL is available trough packaging system:

> apt-get install libsdl2-dev

On Windows, binaries can be downloaded from SDL web: https://www.libsdl.org/download-2.0.php. Get 32bit version.




//////////////////////////////////////////////////
////////////////// COMPILATION ///////////////////

After all dependencies have been acquired, NativeScript and the testing application can be built using CMake. CMake can be obtained from its website https://cmake.org/download/ or using package system:

> apt-get install cmake

1) First create build directory outside of the sources. In-source builds are prohibited. Then the steps differ based on the platform:


//////////////////// Linux ///////////////////////

2) Go into the build directory.

3) Run command

> cmake <path_to_scriptSpeed> -DV8_DIR=<path_to_v8>

CMake will try to find the other dependencies, however, some paths might need to be specified. Run the command to see which variables need to be set for your case.

4) After successfully generating makefile, build everything using command

> make

5) Copy directory scripts from the source directory to the directory with the executables you just built. It contains script source codes that are tested.


/////////////////// Windows //////////////////////

2) Run CMake gui executable.

3) Set paths to project source directory and your build directory (has to be different). Set generator to Visual Studio 12 2013 (you can try different version, however we tested only this option). Again, paths to dependencies that weren't found have to be specified manually. Run configuration to see what paths are missing and how to specify them.

4) Configure the build and then Generate the MSVC solution.

5) Open the MSVC solution, select MinSizeRel configuration and build the solution.

6) Copy directory scripts from the source directory to the directory with the executables you just built. It contains script source codes that are tested.

7) If not on your PATH, copy dll files v8.dll, mono-2.0.dll, SDL2.dll to the directory with the executables you just built (its name should be MinSizeRel).




//////////////////////////////////////////////////
/////////////// RUNNING THE TESTS ////////////////

To run the testing application simply run scrSpeed executable with appropriate arguments. You can try e.g.:

> ./scrSpeed lua expression

Result should be a number showing the measured time.

To execute all test scenarios, copy script runTests from the source directory to the directory with scrSpeed executable and run it. It might take around half an hour to execute all the tests, it depends on your proccessing power. Results will be written into file output.csv.

To generate statistics from the results, you can use file OutputProcessor.class. You need Java runtime version 1.6 or later to run it. Assuming you are in the same folder as the file, run it using command

> java OutputProcessor <results_file_name>

File processed-<original-filename> will be produced. First column represents mean, second standard deviation and third the test case name.




//////////////////////////////////////////////////
/////////////////// USER GUIDE ///////////////////

This section contains user guide to the testing application. We describe its possible run arguments and configurations.


//////////////// Configurations //////////////////

By default the application measures wall time and prints the result in a way appropriate for the runTests script. You can change these settings in file settings.h in directory scriptSpeed. Recompilation of the application is necessary for the changes to take effect.


////////////////// Arguments /////////////////////

First argument of the testing application is always target language. Run the application without any arguments to see the options. 

For each language, several scenarios are implemented. Run the application olny with the language argument to see which scenarios are available for given language. Scenario name is passed as the second argument.

Other arguments depend on the selected scenario:

EXPRESSION: Third argument represents number of arguments passed in the script invocation. Possible values are from 2 to 6, default is 2. As a fourth argument string "opt" can be passed to specify optimized version of invocation should be used. Otherwise, naive approach is used.

CALLBACK: Third argument represents number of parameters passed with the callback. Default is 0. Fourth argument can be used to specify the number of callbacks invoked.

POINT SIMULATION: Third argument represents specific configuration of the point simulation scenario. To see available configurations for given language, run the application without specifying the third argument. Fourth argument can make the application to visualize the testing scenario rather than measure it. You can select values "show" to see the point movements that are tested, or "interactive" to make the leading point follow mouse. Fifth argument can specify custom frame count and sixth custom point count.


There are several additional experimental configurations implemented, however they were not included into this work. We selected scenarios and configurations that we believed to be the best (and sufficient) for gathering the results. So e.g. in Lua or JavaScript, point simulation scenario has configuration with program flow control within the script. It is called loopControl. However, since these configurations weren't added to this work, they weren't profiled and cleaned as much as the other scenarios. They may however serve as an inspiration or a basis for extending this work.




//////////////////////////////////////////////////
////////////////// KNOWN ISSUES //////////////////

We believe we found a bug in LLVM that can cause program crash durig generation of IR on Windows.

It happens during function call of PHINode:Create within which overriden operator new is called. Inside the new operator implementation, fields of the not yet created class are set, specifically value of field HasHungOffUses is set to true. However, during constructor call, this memory can be erased - C specification states the memory is undefined. On Windows using MSVC build, we encountered cases when during constructor call all fields have been set to 0, thus resetting the field value to false. We believe this implementation style to be a bad practise that doesn't guarantee corect behavior.

As a workaround we added one statement to include file lvm/IR/Instructions.h to line 2327:

> this->HasHungOffUses = true;

This addition is the first line of method allocHungoffUses(unsigned N) so it basically makes sure that before any allocation the field is set correctly. This is by no means a robust bugfix. It is simply a fast solution that made this part of LLVM operational in our case. If you encounter issues during generation of IR, this solution might help you as well.
