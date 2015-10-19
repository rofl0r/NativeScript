#include "measure.h"
#include <stdio.h>
#include "Windows.h"

namespace measure {

	// TODO: get rid of the globals
	LARGE_INTEGER time1, time2, freq;
	FILETIME cpuTime1, sysTime1, createTime, exitTime;
	FILETIME cpuTime2, sysTime2;

	void cpuStart()
	{
		GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &sysTime1, &cpuTime1);
	}

	void cpuStop()
	{
		GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &sysTime2, &cpuTime2);
	}

	void cpuDisplayResults()
	{
		double sTime1 = (double)(sysTime1.dwLowDateTime | ((unsigned long long)sysTime1.dwHighDateTime << 32)) * 0.0001;
		double sTime2 = (double)(sysTime2.dwLowDateTime | ((unsigned long long)sysTime2.dwHighDateTime << 32)) * 0.0001;
		double uTime1 = (double)(cpuTime1.dwLowDateTime | ((unsigned long long)cpuTime1.dwHighDateTime << 32)) * 0.0001;
		double uTime2 = (double)(cpuTime2.dwLowDateTime | ((unsigned long long)cpuTime2.dwHighDateTime << 32)) * 0.0001;
#if defined(SB_MEASURE_PRINT_HUMAN)
		printf("Total system-time: %f\n", sTime2 - sTime1);
		printf("Total user-time: %f\n\n", uTime2 - uTime1);
#else
		printf("%f;", sTime2 - sTime1);
		printf("%f;", uTime2 - uTime1);
#endif
	}

	void wallStart()
	{
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&time1);

	}

	void wallStop()
	{
		QueryPerformanceCounter(&time2);
	}

	void wallDisplayResults()
	{
		double time = (double)(time2.QuadPart - time1.QuadPart) * 1000.0 / freq.QuadPart;
#if defined(SB_MEASURE_PRINT_HUMAN)
		printf("Total wall-time: %f\n\n", time);
#else
		printf("%f;", time);
#endif
	}
}
