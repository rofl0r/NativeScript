////////////////////////////////////////// Settings /////////////////////////////////////////

// game mode
#define TEST 1 // no visualization, measurement is taking place
#define SHOW_TEST 2 // visualize test scenario
#define INTERACTIVE 3 // intractive mode to see scripted logic

#define GAME_MODE TEST // intractive mode to see scripted logic


// script mode (olny available if GAME_MODE = TEST)
#define NATIVE 1 // no scripting used
#define ALL_SCRIPT 2 // all point logic scripted
#define ALL_SCRIPT_MIN_CALLBACK 3 // all scripted, but callbacks to c++ minimized at cost of more script variables
#define SCRIPT_TO_NATIVE 4 // half scripted, half implemented in C++ and called from script

#define SCRIPT_MODE ALL_SCRIPT


// measure mode (olny available if GAME_MODE = TEST)
#define CPU 1
#define WALL 2

#define MEASURE_MODE WALL