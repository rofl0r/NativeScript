IF(NOT WIN32)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(MONO REQUIRED mono-2)
ELSE(NOT WIN32)

	# Try to guess where 32bit mono is. If unsuccessful, user can specify it manually
	set(MONO_DIR "c:/Program Files (x86)/Mono" CACHE PATH "Path to mono installation directory")
		
	# Verify correct mono location by veryfying the library is there
	find_library(MONO_LIB NAMES mono-2.0 HINTS ${MONO_DIR}/lib)
	IF(NOT MONO_LIB)
		MESSAGE(FATAL_ERROR "Mono wasn't found. Run Cmake again and specify root directory of its installation in MONO_DIR")
	ENDIF(NOT MONO_LIB)
		
	# Set all the needed variables
	SET(MONO_INCLUDE_DIRS ${MONO_DIR}/include/mono-2.0)
	SET(MONO_LIBRARY_DIRS ${MONO_DIR}/lib)
	SET(MONO_LIBRARIES mono-2.0.lib)
	SET(MONO_LIB_DIR ${MONO_DIR}/lib)
	SET(MONO_ETC_DIR ${MONO_DIR}/etc)
	SET(MONO_MCS_DIR ${MONO_DIR}/lib/mono/4.5)

ENDIF(NOT WIN32)
