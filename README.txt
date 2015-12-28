LINUX BUILD GUIDE


First you have to build all dependencies:

LLVM
- get sources (3.7.0, e.g. http://llvm.org/releases/download.html)
- create separate build directory, and cd into it
- cmake -DCMAKE_BUILD_TYPE=MinSizeRel <path_to_llvm_src>
- cmake --build .
- cmake --build . --target install (might need sudo)
- apt-get might also work (and be way faster), but we haven't tested this yet

v8
- git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
- export PATH=`pwd`/depot_tools:"$PATH"
- fetch v8
- git checkout tags/4.4.9.1 (last build that doesn't cause unsolvable seg fault on linux https://code.google.com/p/v8/issues/detail?id=4192)
- make x64.release i18nsupport=off component=shared_library

Mono
- do what they say here http://www.mono-project.com/docs/getting-started/install/linux/
- install the complete version - we are done :)

Lua
- you can even get it by apt-get, but the compilation takes just few seconds, so you can build whatever you want yourself, e.g.:
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