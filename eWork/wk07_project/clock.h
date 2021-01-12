#ifndef CLOCK_H_
#define CLOCK_H_

/* Routines for using cycle counter */
#define MAX_CYC 10000000000
/* Start the counter */
void start_counter();

/* Get # cycles since counter started */
double get_counter();

/* Measure overhead for counter */
double ovhd();

/* Determine clock rate of processor (using a default sleeptime) */
double mhz(int verbose);

/* Determine clock rate of processor, having more control over accuracy */
double mhz_full(int verbose, int sleeptime);

/** Special counters that compensate for timer interrupt overhead */

void start_comp_counter();

double get_comp_counter();

#endif 