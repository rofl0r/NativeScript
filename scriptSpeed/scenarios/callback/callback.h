#ifndef SB_CALLBACK_H
#define SB_CALLBACK_H

// static settings
#define SB_C_DEFAULT_CYCLES 10000000
#define SB_C_DEFAULT_PARAM_COUNT 0
#define SB_C_PARAM_STRINGS_MAX_LENGTH 29

namespace callback {
	/* returns whether arguments were successfully read */
	bool readArgs(int c, char** v);
	int getParamCount();
	char* getParamDeclarationString(); // e.g. "a,b,c" for 3 params
	char* getParamCallString();  // e.g. "i,i*0.7,i*0.49" for 3 params
	long getCycleCount();
	double processCallback(double val);
	bool validateResults();
}

#endif
