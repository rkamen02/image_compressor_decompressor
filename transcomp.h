/*
 * transcomp.h
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 * HW 4
 *
 * Function declarations for transcomp
 */

#ifndef TRANSCOMP
#define TRANSCOMP

#include "uarray2.h"

/* struct of floats representing a cosine-transformed 2x2 block of pixels */
typedef struct TransComp {
        float pb, pr, a, b, c, d;
} TransComp;

/*
 * Given a UArray2_T of Component pixels, converts each 2x2 box of Components 
 * into a TransComp struct via cosine transform. Returns a UArray2_T of 
 * TransComps.
 * CRE if 2D array is NULL.
 */
UArray2_T ComponentsToTransComps( UArray2_T Components);

/*
 * Given a UArray2_T of TransComps, converts each TransComps' values into 
 * four Components via inverse cosine transform. Returns a UArray2_T of 
 * Components.
 * CRE if 2D array is NULL.
 */
UArray2_T TransComptsToComponents( UArray2_T TransComps);

#endif
