#ifndef SB_CALLBACK_H
#define SB_CALLBACK_H

// static settings
#define SB_C_DEFAULT_CYCLES 10000000

namespace callback {
	long getCycleCount(int c, char** v);
	void executeCallbackBody();
	void displayResults();
}

#endif
