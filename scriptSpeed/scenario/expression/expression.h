#ifndef SS_EXPRESSION_H
#define SS_EXPRESSION_H

// static settings
#define SS_E_DEFAULT_CYCLES 2000000

#define SS_EXPRESSION_MAX_LENGTH 50
#define SS_EXPRESSION_PARAM_2 "(x*y)+(x+y)*(x-y)"
#define SS_EXPRESSION_PARAM_3 "(x*y)+(z+y)*(x-z)"
#define SS_EXPRESSION_PARAM_4 "(x*y)+(z+u)*(x-y)"
#define SS_EXPRESSION_PARAM_5 "(x*y)+(z+u)*(v-y)"
#define SS_EXPRESSION_PARAM_6 "(x*y)+(z+u)*(v-w)"
#define SS_EXPRESSION_PARAM_NAMES {"x","y","z","u","v","w"}

namespace expression {
	/* returns whether arguments were successfully read */
	bool readArgs(int c, char** v);
	/* returns value read from arguments or default (false) */
	bool isRunOptimized();
	/* returns value read from arguments or default (expr. param 2) */
	int getParamCount();
	const char* getExpression();
	/* validate result using current configuration, for parameters having 
	values <cycle>+0.1, where <cycle> is indexed from 0 */
	void validateResult(double result, double cycles = SS_E_DEFAULT_CYCLES);
}

#endif
