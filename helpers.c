/*
 * helpers.c
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 * HW 4
 *
 * Implementation for helper functions
 */

/*
 * moveInBoundsUint
 *      purp: Given an unsigned value, minimum value and maximum value,
 *              returns the value if it is in bounds and moves to the closest
 *              value if it is out of bounds.
 *      args: value, upper lower bound, upper bound
 *      rets: the value if in bounds, closest bound if out of bounds
*/
unsigned moveInBoundsUint( unsigned value, unsigned min, unsigned max)
{
        if (value < min) {
                return min;
        }

        if (value > max) {
                return max;
        }

        return value;
}

/*
 * moveInBoundsInt
 *      purp: Given an int value, minimum value and maximum value,
 *              returns the value if it is in bounds and moves to the closest
 *              value if it is out of bounds.
 *      args: value, upper lower bound, upper bound
 *      rets: the value if in bounds, closest bound if out of bounds
*/
int moveInBoundsInt( int value, int min, int max)
{
        if (value < min) {
                return min;
        }

        if (value > max) {
                return max;
        }

        return value;
}

/*
 * moveInBoundsFloat
 *      purp: Given a float value, minimum value and maximum value,
 *              returns the value if it is in bounds and moves to the closest
 *              value if it is out of bounds.
 *      args: value, upper lower bound, upper bound
 *      rets: the value if in bounds, closest bound if out of bounds
*/
float moveInBoundsFloat( float value, float min, float max)
{
        if (value < min) {
                return min;
        }

        if (value > max) {
                return max;
        }

        return value;
}
