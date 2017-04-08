/*
 * meansquarediff.h
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 * HW 4
 *
 * A function to compare two images.
 */

#ifndef IMAGECOMPARE
#define IMAGECOMPARE

#include <pnm.h>

/* 
 * Given two Pnm_ppms that have widths and heights that are at most,
 * different by one, returns their difference calculated with
 * mean square difference of their pixel values.
 * Returns 100.0 if widths or heights differ by more than 1 each.
 * CRE if given NULL Pnm_ppms.
 * CRE if denominator, width, or height are non natural
 */
float meanSquareDiff( Pnm_ppm image1, Pnm_ppm image2);

#endif
