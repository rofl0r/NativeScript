#ifndef SB_EXPRESSION_H
#define SB_EXPRESSION_H

// static settings
#define SB_E_DEFAULT_CYCLES 2000000

#define SB_EXPRESSION_MAX_LENGTH 50
#define SB_EXPRESSION_PARAM_2 "(x*y)+(x+y)*(x-y)"
#define SB_EXPRESSION_PARAM_3 "(x*y)+(z+y)*(x-z)"
#define SB_EXPRESSION_PARAM_4 "(x*y)+(z+u)*(x-y)"
#define SB_EXPRESSION_PARAM_5 "(x*y)+(z+u)*(v-y)"
#define SB_EXPRESSION_PARAM_6 "(x*y)+(z+u)*(v-w)"
#define SB_EXPRESSION_PARAM_NAMES {"x","y","z","u","v","w"}

namespace expression {
	/* returns whether arguments were successfully read */
	bool readArgs(int c, char** v);
	/* returns value read from arguments or default (false) */
	bool isRunOptimized();
	/* returns value read from arguments or default (expr. param 2) */
	int getParamCount();
	char* getExpression();
	/* validate result using current configuration */
	void validateResult(double result);
}

#endif
