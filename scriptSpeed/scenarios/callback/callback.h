#ifndef SB_CALLBACK_H
#define SB_CALLBACK_H

#define SB_C_DEFAULT_CYCLES 10000000
#define SB_C_DEFAULT_PARAM_COUNT 0
// if SB_C_MAX_PARAMS is changed, getParamCallString() implementation must be updated
// (and SB_C_PARAM_CALL_STRING_MAX_LENGTH as well)
#define SB_C_MAX_PARAMS 4
#define SB_C_PARAM_CALL_STRING_MAX_LENGTH 22

namespace callback {
	/* returns whether arguments were successfully read */
	bool readArgs(int c, char** v);
	int getParamCount();
	char* getParamCallString();  // e.g. "i,i*0.7,i*0.49" for 3 params
	long getCycleCount();
	void processCallback(double val);
	bool validateResults();
}

#endif
