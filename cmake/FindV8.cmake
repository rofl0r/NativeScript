# - Find V8
#
# User has to set the variables manually:
# V8_DIR either to root v8 build or to directory with the libraries
# V8_INC_DIR optional, if the header location is custom
#
# Output either fatal error or:
# V8_INC_DIR include header locations
# V8_LIBRARIES libraries to link

IF(V8_DIR)

# Add standard paths for windows (32 bit) and linux (32 and 64 bit)
	set(V8_LIB_PATHS 
		${V8_DIR}
		${V8_DIR}/build/Release
		${V8_DIR}/build/Release/lib
		${V8_DIR}/out/x64.release/obj.target/tools/gyp
		${V8_DIR}/out/ia32.release/obj.target/tools/gyp
	)

	find_library(V8_LIB NAMES v8 HINTS ${V8_LIB_PATHS})
	find_library(V8_LIB_PLAT NAMES v8_libplatform HINTS ${V8_LIB_PATHS})

	# For some reason some symbols are missing even though they should be in the shared lib on windows.
	# Those are all in this additional lib (which should be included in the dll, but for some reason it has to be still added)
	IF(WIN32)
		find_library(V8_LIB_LBASE NAMES v8_libbase HINTS ${V8_LIB_PATHS})
	ENDIF(WIN32)

	IF(NOT V8_LIB OR NOT V8_LIB_PLAT OR (WIN32 AND NOT V8_LIB_LBASE))
		MESSAGE(FATAL_ERROR "V8 libraries not not found in the specified location.")
	ELSE(NOT V8_LIB OR NOT V8_LIB_PLAT OR (WIN32 AND NOT V8_LIB_LBASE))
		IF(WIN32)
			SET(V8_LIBRARIES ${V8_LIB} ${V8_LIB_PLAT} ${V8_LIB_LBASE})
		ELSE(WIN32)
			SET(V8_LIBRARIES ${V8_LIB} ${V8_LIB_PLAT})
		ENDIF(WIN32)

		IF(NOT V8_INC_DIR)
			SET(V8_INC_DIR ${V8_DIR})
		ENDIF(NOT V8_INC_DIR)

	ENDIF(NOT V8_LIB OR NOT V8_LIB_PLAT OR (WIN32 AND NOT V8_LIB_LBASE))
ELSE(V8_DIR)
    MESSAGE(FATAL_ERROR "Set V8_DIR either to root directory of v8 build or to directory with the libraries. If necessary, specific include headers location can be set in V8_INC_DIR.")
ENDIF(V8_DIR)
