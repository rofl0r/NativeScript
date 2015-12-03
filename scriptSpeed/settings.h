////////////////////////////////////////// Settings /////////////////////////////////////////

// measure mode (olny available if GAME_MODE = TEST)
#define CPU 1
#define WALL 2

#define MEASURE_MODE WALL

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
	#define SS_PLATFORM_WINDOWS
#elif defined(__APPLE__)
	#define SS_PLATFORM_MAC
#else
	#define SS_PLATFORM_UNIX
#endif