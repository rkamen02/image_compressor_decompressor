/*
 * compress40.c
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 *
 * Implementation for compress40.h
 */

#include "codeword.h"
#include "component.h"
#include "floatrgb.h"
#include "transcomp.h"

#include <assert.h>
#include <a2methods.h>
#include <a2plain.h>
#include <compress40.h>
#include <pnm.h>

void trim( Pnm_ppm *image);

/* PUBLIC FUNCTIONS */

/*
 * compress40
 *      purp: Takes an image file and writes it in compressed from to 
 *            standard output
 *            CRE for NULL file.
 *            CRE for an image with either a width or height less than zero.
 *      args: FILE *
 *      rets: none
 */
extern void compress40  (FILE *input)
{
        assert( input != NULL);

        A2Methods_T methods = uarray2_methods_plain;
        assert(methods);

        Pnm_ppm image = Pnm_ppmread( input, methods);
        trim( &image);
        
        assert( (image -> width > 1) && (image -> height > 1));
        
        /* compression */
        UArray2_T Float_rgbs = Pnm_ppmToFloat_rgbs( image);
        UArray2_T Components = Float_rgbsToComponents( Float_rgbs);
        UArray2_T TransComps = ComponentsToTransComps( Components);
        UArray2_T Codewords = TransCompsToCodewords( TransComps);
        
        writeCodewords( stdout, Codewords);
        
        Pnm_ppmfree( &image);
        UArray2_free( &Float_rgbs);
        UArray2_free( &Components);
        UArray2_free( &TransComps);
        UArray2_free( &Codewords);
}

/*
 * decompress40
 *      purp: takes a file containing a compressed image and writes the 
 *              decompressed image to standard output
 *            CRE for NULL file.
 *      args: FILE *
 *      rets: none
 */
extern void decompress40(FILE *input)
{
        assert( input != NULL);

        /* decompression */
        UArray2_T Codewords  = readCodewords( input);
        UArray2_T TransComps = CodewordsToTransComps( Codewords);
        UArray2_T Components = TransComptsToComponents( TransComps);
        UArray2_T Float_rgbs = ComponentsToFloat_rgbs( Components);
        Pnm_ppm   image      = Float_rgbsToPnm_ppm( Float_rgbs);
        
        Pnm_ppmwrite( stdout, image);
        
        Pnm_ppmfree( &image);
        UArray2_free( &Float_rgbs);
        UArray2_free( &Components);
        UArray2_free( &TransComps);
        UArray2_free( &Codewords);
}



/* PRIVATE FUNCTIONS */

/*
 * trim
 *      purp: "Trims" the last row and/or column of an image if either are 
 *            odd by decrementing the Pnm_ppm's width or height by 1 if 
 *            needed.
 *            CRE for NULL Pnm_ppm pointer or NULL Pnm_ppm.
 *            CRE for NULL Pnm_ppm methods.
 *      args: Pnm_ppm *
 *      rets: none
 */
void trim( Pnm_ppm *image)
{
        assert(  image != NULL);
        assert( *image != NULL);
        assert( ((*image) -> methods) != NULL );
                
        unsigned width  = (*image) -> width;
        unsigned height = (*image) -> height;
        
        (*image) -> width  = width  - (width % 2);
        (*image) -> height = height - (height % 2);
}
