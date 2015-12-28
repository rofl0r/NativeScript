////////////////////////////////////////// Settings /////////////////////////////////////////

// measure mode
#define SS_MEASURE_CPU 1
#define SS_MEASURE_WALL 2

// prints in human readible format (otherwise generates results for csv output file)
//#define SS_MEASURE_PRINT_HUMAN

#define SS_MEASURE_MODE SS_MEASURE_WALL

#if defined __WIN32__ || defined WIN32 || defined _WIN32
	#define SS_PLATFORM_WINDOWS
#elif defined __APPLE__
	#define SS_PLATFORM_MAC
#else
	#define SS_PLATFORM_UNIX
#endif