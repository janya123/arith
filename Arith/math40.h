/*
 * math40.h
 * Purpose: Interface to perform calculations on floats during compression 
 *          and decompression process
 * Written by : Kenny Lin (klin04) and Janya Gambhir (jgambh01)
 *         on : 3/22/2021
 */
#ifndef MATH40_INCLUDED
#define MATH40_INCLUDED

/*
 * round_float
 * Rounds a float up or down depending on whether it is greater than 0
 */
int round_float(float num);

/*
 * check_range
 * Checks if float is in between specified maximum and minimum; if it is 
 * too large/small then updates it to fit the range
 */
float check_range(float num, float max, float min);

#endif