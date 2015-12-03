file(GLOB_RECURSE SS_HEADERS *.h *.hpp)
file(GLOB_RECURSE SS_SOURCES *.cpp *.c)


# gameScript (it is located in this project, no need to find)
include_directories(../gameScript/include)


# find v8, implemented in cmake/FindV8.cmake
find_package(V8 REQUIRED)
include_directories(${V8_INC_DIR})
# v8 is linked as shared library, windows requires this option (other platforms ignore it):
add_definitions(-DUSING_V8_SHARED)


# find mono, implemented in cmake/FindMONO.cmake
find_package(MONO REQUIRED)
include_directories(${MONO_INCLUDE_DIRS})
link_directories(${MONO_LIBRARY_DIRS})
IF (WIN32)
	add_definitions(-DSS_MONO_LIB_DIR="${MONO_LIB_DIR}")
	add_definitions(-DSS_MONO_ETC_DIR="${MONO_ETC_DIR}")
	add_definitions(-DSS_MONO_MCS_DIR="${MONO_MCS_DIR}")
ENDIF()


# Find Lua
# If specified by user, use that location
IF(LUA_DIR_MANUAL)
	set(LUA_LIB_PATHS 
		${LUA_DIR_MANUAL}
		${LUA_DIR_MANUAL}/src
		${LUA_DIR_MANUAL}/install/lib
		${LUA_DIR_MANUAL}/Release
	)

	FIND_PATH(LUA_INCLUDE_DIR lua.hpp
		${LUA_DIR_MANUAL}/src 
		${LUA_DIR_MANUAL}/include 
		${LUA_DIR_MANUAL}/install/include
	)
	find_library(LUA_LIBRARIES NAMES lua HINTS ${LUA_LIB_PATHS})
# If not specified, use standard lookup
ELSE(LUA_DIR_MANUAL)
	find_package(LUA)
	IF(NOT LUA_FOUND) 
		MESSAGE(FATAL_ERROR "Lua not found. If you don't have cmake version, specify its location in LUA_DIR_MANUAL.")
	ENDIF(NOT LUA_FOUND)
ENDIF(LUA_DIR_MANUAL)
include_directories(${LUA_INCLUDE_DIR})


# Create the application
add_executable(scriptSpeed ${SS_SOURCES})


# Link against libraries
target_link_libraries(scriptSpeed gameScript ${V8_LIBRARIES} ${MONO_LIBRARIES} ${LUA_LIBRARIES})

IF (WIN32)
 	target_link_libraries(scriptSpeed winmm)
ELSE()
	target_link_libraries(scriptSpeed pthread dl)
ENDIF()

# TODO: install ${MONO_DIR}/bin/mono-2.0.dll and v8.dll

