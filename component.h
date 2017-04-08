/*
 * floatrgb.h
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 * HW 4
 *
 * Function declarations for floatrgb
 */

#ifndef COMPONENT
#define COMPONENT

#include "uarray2.h"

/* struct of floats representing component values for a pixel */
typedef struct Component {
        float y, pb, pr;
} Component;

/* Given a UArray2_T of Float_rgb pixels, converts each pixel's RGB values 
 * to an equivalent component form and stores those values into a UArray2_T of
 * Component structs. Returns a UArray2_T of Components.
 * CRE if 2D array is NULL. */
UArray2_T Float_rgbsToComponents( UArray2_T Float_rgbs);

/* Given a UArray2_T of Components, converts each pixel's component values 
 * into their RGB representations and stores those values in a UArray2_T of 
 * Pnm_rgb structs. Returns a UArray2_T of Pnm_rgb's.
 * CRE if 2D array is NULL. */
UArray2_T ComponentsToFloat_rgbs( UArray2_T Components);

#endif
