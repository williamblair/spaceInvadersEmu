/*
 * main.cpp
 */

#include <cstdio>

#include "Cpu8080.h"

int main(int argc, char *argv[])
{
	Cpu8080 cpu;

	/* Main loop */
	for(;;) {
		cpu.run_next_op();
	}

	return 0;
}
