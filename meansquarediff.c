/*
 * meansquarediff.c
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 * HW 4
 *
 * The implementation of a function to compare two images.
 */

#include "meansquarediff.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

/*
 * closure structure
 * used to carry information for the image difference calculation
 *
 * sum: running sum of square differences
 * denom1: rgb denominator of image1
 * denom2: rgb denominator of image2
 * small_width:  the smaller of the two image widths
 * small_height: the smaller of the two image heights
 * Pnm_ppm: the second image
 */
typedef struct closure {
        float    *sum;
        unsigned *denom1;
        unsigned *denom2;
        unsigned *small_width;
        unsigned *small_height;
        Pnm_ppm  *img2;
} *closure;

/*
 * sumDiffs 
 *      purp: an A2Methods_applyfun that is meant to be called on an
 *              and given a properly created and populated closure structure.
 *              Adds the squared differences of the pixels' rgb value to sum.
 *              Compares the pixel its on to the pixel at (x,y) in img2
 *              from clousre.
 *      args: (x,y) of *pix in array2, a UArray2 of rgb pixels. 
 *              void *cl is a pointer to a closure structure that has been
 *              properly formatted and populated
 *      rets: void
 */
void sumDiffs( int x, 
                     int y, 
                     A2Methods_UArray2 array2, 
                     A2Methods_Object *pix, 
                     void *cl)
{
        (void) array2;
        
        unsigned width  = *( ((closure)cl) -> small_width  );
        unsigned height = *( ((closure)cl) -> small_height );
        
        /* If out of the bounds of either photo, do nothing */
        if (( (unsigned)x >= width) || ( (unsigned)y >= height)) {
                return;
        }
        
        struct Pnm_rgb *pix1 = (struct Pnm_rgb *)pix;
        
        Pnm_ppm        *img2 = ((closure)cl) -> img2;
        A2Methods_T  methods = (A2Methods_T)( (*img2) -> methods);
        struct Pnm_rgb *pix2 = (*methods).at( (*img2) -> pixels, x, y);

        float denom1 = *( ((closure)cl) -> denom1);
        float denom2 = *( ((closure)cl) -> denom2);
        
        /* scale the rbg values */
        float r1 = pix1 -> red   / denom1;
        float r2 = pix2 -> red   / denom2;
        float g1 = pix1 -> green / denom1;
        float g2 = pix2 -> green / denom2;
        float b1 = pix1 -> blue  / denom1;
        float b2 = pix2 -> blue  / denom2;
        
        /* Add the sum of the squared differences in rgb values to sum */
        ( *((closure)cl) -> sum) += pow( r1 - r2, 2) + 
                                    pow( g1 - g2, 2) +
                                    pow( b1 - b2, 2);
}

/*
 * min
 *      purp: returns the minimum of the two given unsigned integers,
 *              CRE if either is NULL
 *      args: two unsigned integers
 *      rets: their minimum
 */
unsigned min( unsigned uint1, unsigned uint2)
{
        assert( uint1 && uint2);
        return uint1 < uint2 ? uint1 : uint2;
}

/*
 * meanSquareDiff
 *      purp: calculate the difference between the two given Pnm_ppms using
 *              mean square difference. returns 1.0 if either of the widths
 *              or heights differ by more than 1. CRE if either width, height
 *              or denominator is not natural. CRE if either image is null.
 *      args: two Pnm_ppms
 *      rets: a float representation of the mean square difference between
 *              the two images
 */
float meanSquareDiff( Pnm_ppm image1, Pnm_ppm image2)
{
        assert( image1 && image2);

        unsigned small_width  = min( (image1 -> width), (image2 -> width));
        unsigned small_height = min( (image1 -> height), (image2 -> height));

        assert( min( small_width, small_height) > 0);
        assert( min( image1 -> denominator, image2 -> denominator) > 0);

        int      width_diff   = (image1 -> width)  - (image2 -> width);
        int      height_diff  = (image1 -> height) - (image2 -> height);

        closure  cl;
        float    sum          = 0;
        A2Methods_T methods   = (A2Methods_T)image1 -> methods;
        
        if ( (abs(width_diff) > 1) || (abs(height_diff) > 1)) {
                fprintf( stderr, "Images not of compatible sizes.\n");
                return 1;
        }
        
        /* Create and populate the closure structure */ 
        cl  = malloc( sizeof(struct closure));  

        cl -> sum = &sum;

        cl -> small_width = &small_width;
        cl -> small_height = &small_height;

        cl -> denom1 = &(image1 -> denominator);
        cl -> denom2 = &(image2 -> denominator);
        
        cl -> img2 = &image2;
        
        /* 
         * Run the mapping function to calculate the sum of 
         * squared differences
         */
        methods -> map_default( image1 -> pixels, sumDiffs, cl);
                
        /* 
         * closure structs exclusively hold pointers so we can free it 
         * and still have the value calculated with sumDiffs
         */
        free( cl);
        
        return sqrtf( sum / ((float)( small_width * small_height * 3)));
}

