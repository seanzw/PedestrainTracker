#include <iostream>
#include <windows.h>
#include <stdio.h>

#include "PedestrainCounting.h"

using namespace std;

int main(int argc, char *argv[]) {

	//get the time of running program
	LARGE_INTEGER freq;
	LARGE_INTEGER start_t, stop_t;
	double exe_time = 0.0;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start_t);

    PedestrainCounter pedestrainCounter;
    pedestrainCounter.ParseParams(argc, argv);

	QueryPerformanceCounter(&stop_t);
	exe_time = double(stop_t.QuadPart - start_t.QuadPart) / freq.QuadPart;
	fprintf(stdout, "The program executed time is %fs.\n", exe_time);

	return 0;
}

