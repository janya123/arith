/*
 * math40.c
 * Purpose: Perform calculations on floats during compression 
 *          and decompression process
 * Written by : Kenny Lin (klin04) and Janya Gambhir (jgambh01)
 *         on : 3/22/2021
 */

#include "math40.h"

/*
 * round_float
 * Rounds a float up or down depending on whether it is greater than 0
 * Input: float representing number to be rounded
 * Output: A correctly rounded float
 */
int round_float(float num)
{
    if (num >= 0) {
        return (int) num + 0.5;
    } else {
        return (int) num - 0.5;
    }
}

/*
 * check_range
 * Checks if float is in between specified maximum and minimum; if it is 
 * too large/small then updates it to fit the range
 * Input: float representing number of which to check range
 * Output: A float in the correct range
 */
float check_range(float num, float max, float min)
{
    if (num > max) {
        return max;
    } else if (num < min) {
        return min;
    } else {
        return num;
    }
}
