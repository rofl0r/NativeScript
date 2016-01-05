LINUX BUILD GUIDE


First you have to build all dependencies:

LLVM
- might be necesary to set LLVM repo: wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key|sudo apt-key add -
- apt-get install libllvm3.6 llvm-3.6-dev (TODO: test)
- OR build from source:
- get sources (3.7.0, e.g. http://llvm.org/releases/download.html)
- create separate build directory, and cd into it
- cmake -DCMAKE_BUILD_TYPE=MinSizeRel <path_to_llvm_src>
- cmake --build .
- cmake --build . --target install (might need sudo)

v8
- apt-get install libv8 libv8-dev (TODO: test, what configuration will be installed?)
- OR build from source:
- git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
- export PATH=`pwd`/depot_tools:"$PATH"
- fetch v8
- git checkout tags/4.4.9.1 (last build that doesn't cause unsolvable seg fault on linux https://code.google.com/p/v8/issues/detail?id=4192)
- make x64.release i18nsupport=off component=shared_library

Mono
- set mono repository, guide://www.mono-project.com/docs/getting-started/install/linux/
- install the complete version (apt-get install mono-complete)

Lua
- apt-get install lua5.3 (TODO: test)
- OR build from source:
- curl -R -O http://www.lua.org/ftp/lua-5.3.2.tar.gz
- tar zxf lua-5.3.2.tar.gz
- cd lua-5.3.2
- make linux

SDL
- apt-get install libsdl2-dev


Now you can build the benchmark

- create new build directory (in source build is prohibited), from there run:
- cmake <path_to_scriptSpeed> -DV8_DIR=<path_to_v8> (paths have to be absolute!)
- if needed, add -DLUA_DIR_MANUAL=<path_to_lua> (if lua wasn't installed in cmake compatible way)
- sdl can cause issues as well, specify -DSDL_INCLUDE_DIR="<path_to_sdl_include_dir>" and -DSDL_LIBRARY="<path_to_libsdl2.so>" (e.g. on x86 64-bit ubuntu: /usr/include/SDL2 and /usr/lib/x86_64-linux-gnu/libSDL2.so)
- make
- copy "script" folder from original location to the same folder the executable is (it contains tested script files)
- run test: "./scrSpeed lua expression" or "./scrSpeed cs pointSimul allScript interactive"


Run all tests using shipped script

- copy runTests.sh or runTests.bat from repository root to the same directory scrSpeed executable is in
- run it (make sure it has executable permissions)
- results will be in file output.csv
- you can generate mean and standard deviation (sample) by using file OutputProcessor.class:
- copy it the same way as runTests script
- run it using java with a single argument - name of the file with results (e.g. java OutputProcessor output.csv)
- file processed-<original-filename>.csv will be produced - first column is mean, second standard deviation
