#include <stdio.h>
#include "settings.h"

#ifdef SS_PLATFORM_WINDOWS
#include "Windows.h"
#else
#include <time.h>
#endif

#include "measure.h"


#ifdef SS_PLATFORM_WINDOWS

#else

#endif

namespace measure {

#ifdef SS_PLATFORM_WINDOWS	
	LARGE_INTEGER time1, time2, freq;
	FILETIME cpuTime1, sysTime1, createTime, exitTime;
	FILETIME cpuTime2, sysTime2;
#else
	timespec time1, time2;
#endif

	void start()
	{
#ifdef SS_PLATFORM_WINDOWS
	#if SS_MEASURE_MODE == SS_MEASURE_CPU
		GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &sysTime1, &cpuTime1);
	#else
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&time1);
	#endif
#else
	#if SS_MEASURE_MODE == SS_MEASURE_CPU
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
	#else
		clock_gettime(CLOCK_MONOTONIC, &time1);
	#endif
#endif
	}

	void stop()
	{
#ifdef SS_PLATFORM_WINDOWS
	#if SS_MEASURE_MODE == SS_MEASURE_CPU
		GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &sysTime2, &cpuTime2); only 1/64s precision!!!! wtf
	#else
		QueryPerformanceCounter(&time2);
	#endif
#else
	#if SS_MEASURE_MODE == SS_MEASURE_CPU
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
	#else
		clock_gettime(CLOCK_MONOTONIC, &time2);
	#endif
#endif
	}

	double getResults()
	{
#ifdef SS_PLATFORM_WINDOWS
	#if SS_MEASURE_MODE == SS_MEASURE_CPU
		double sTime1 = (double)(sysTime1.dwLowDateTime | ((unsigned long long)sysTime1.dwHighDateTime << 32)) * 0.0001;
		double sTime2 = (double)(sysTime2.dwLowDateTime | ((unsigned long long)sysTime2.dwHighDateTime << 32)) * 0.0001;
		double uTime1 = (double)(cpuTime1.dwLowDateTime | ((unsigned long long)cpuTime1.dwHighDateTime << 32)) * 0.0001;
		double uTime2 = (double)(cpuTime2.dwLowDateTime | ((unsigned long long)cpuTime2.dwHighDateTime << 32)) * 0.0001;
		return sTime2 - sTime1 + uTime2 - uTime1;
	#else
		return (double)(time2.QuadPart - time1.QuadPart) * 1000.0 / freq.QuadPart;
	#endif
#else
		time_t sec = time2.tv_sec - time1.tv_sec;
		long nsec = time2.tv_nsec - time1.tv_nsec;
		return sec*1000 + (double)nsec*0.000001;
#endif
	}

	void displayResults()
	{
		double time = getResults();
		
#if defined(SS_MEASURE_PRINT_HUMAN)
	#if SS_MEASURE_MODE == SS_MEASURE_CPU
		printf("Total CPU-time: %f ms\n", time);
	#else
		printf("Total wall-time: %f ms\n", time);
	#endif
#else
		printf("%f;", time);
#endif
	}

}
