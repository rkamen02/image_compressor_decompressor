/*
 * codeword.h
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 * HW 4
 *
 * Function declarations for codeword
 */

#ifndef QUANTCODEWORD
#define QUANTCODEWORD

#include "uarray2.h"
#include <stdio.h>

/* Given a UArray2_T of TransComp structs, converts it into a 2D array of 
 * Codewords.
 * CRE if TransComps is NULL.
 * CRE if general Codeword format is invalid. */
UArray2_T TransCompsToCodewords( UArray2_T TransComps);

/* Given a UArray2_T of Codewords, converts it into a 2D array of TransComp
 * structs.
 * CRE if Codewords is NULL.
 * CRE if general Codeword format is invalid. */
UArray2_T CodewordsToTransComps( UArray2_T Codewords);


/* Given a UArray2_T of Codewords and a file destination, writes the Codewords
 * to the file. 
 * CRE if general Codeword format is invalid.
 * CRE if any Codeword is improperly formatted. */
void writeCodewords( FILE *out, UArray2_T Codewords);

/* Given a file source containing a list of codewords for an image, creates a 
 * UArray2_T of Codewords.
 * CRE if general Codeword format is invalid.
 * CRE if image header is improperly formatted.
 * CRE if any Codeword is improperly formatted.
 * CRE if width or height of the image is less than 1. */
UArray2_T readCodewords( FILE *in);



#endif
