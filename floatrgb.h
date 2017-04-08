/*
 * floatrgb.h
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 * HW 4
 *
 * Function declarations for floatrgb
 */

#ifndef FLOATRGB
#define FLOATRGB

#include <pnm.h>
#include <a2methods.h>
#include "uarray2.h"

/* struct of floats representing the red, green, and blue values of a pixel */
typedef struct Float_rgb {
        float r, g, b;
} Float_rgb;

/* Given a Pnm_ppm formatted image, converts it to a 2D array of Float_rgb 
 * structures. 
 * CRE for Pnm_ppm to be NULL. */
UArray2_T Pnm_ppmToFloat_rgbs( Pnm_ppm image);

/* Given a UArray2_T of Float_rgb structures, converts it to a Pnm_ppm and 
 * returns it. 
 * CRE for UArray2_T to be NULL. */
Pnm_ppm Float_rgbsToPnm_ppm( UArray2_T Float_rgbs);

#endif
