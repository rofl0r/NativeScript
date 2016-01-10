#ifndef SS_CALLBACK_H
#define SS_CALLBACK_H

#define SS_C_DEFAULT_CYCLES 50000000
#define SS_C_DEFAULT_PARAM_COUNT 0
// if SS_C_MAX_PARAMS is changed, getParamCallString() implementation must be updated
// (and SS_C_PARAM_CALL_STRING_MAX_LENGTH as well)
#define SS_C_MAX_PARAMS 4
#define SS_C_PARAM_CALL_STRING_MAX_LENGTH 22

namespace callback {
	/* returns whether arguments were successfully read */
	bool readArgs(int c, char** v);
	int getParamCount();
	const char* getParamCallString();  // all values are i+0.1
	long getCycleCount();
	void processCallback(double val);
	bool validateResults();
}

#endif
