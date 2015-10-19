#ifndef SB_MEASURE_H
#define SB_MEASURE_H

// prints in human readible format (otherwise generates results for csv output file)
//#define SB_MEASURE_PRINT_HUMAN

namespace measure
{
	void cpuStart();
	void cpuStop();
	void cpuDisplayResults();

	void wallStart();
	void wallStop();
	void wallDisplayResults();
}

#endif
