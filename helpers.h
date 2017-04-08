/*
 * helpers.h
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 * HW 4
 *
 * Function declarations for helper functions
 */

#ifndef HELPERS
#define HELPERS

/*
 * Given an unsigned value, minimum value and maximum value,
 * returns the value if it is in bounds and moves to the closest
 * value if it is out of bounds.
 */
unsigned moveInBoundsUint(   unsigned value, unsigned min, unsigned max);

/*
 * Given an int value, minimum value and maximum value,
 * returns the value if it is in bounds and moves to the closest
 * value if it is out of bounds.
 */
int      moveInBoundsInt(   int value, int min, int max);

/*
 * Given a float value, minimum value and maximum value,
 * returns the value if it is in bounds and moves to the closest
 * value if it is out of bounds.
 */
float    moveInBoundsFloat( float value, float min, float max);

#endif
