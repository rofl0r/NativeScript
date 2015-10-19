#ifndef SB_EXPRESSION_H
#define SB_EXPRESSION_H

// static settings
#define SB_E_DEFAULT_CYCLES 1000000

namespace expression {
	bool isRunOptimized(int c, char** v);
	void validateResult(double result);
}

#endif
