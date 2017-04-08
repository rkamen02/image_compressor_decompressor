/*
 * floatrgb.c
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 * HW 4
 *
 * Function implementations for floatrgb.
 */

#include "floatrgb.h"
#include <stdlib.h>
#include <assert.h>
#include <a2plain.h>
#include <math.h>
#include "helpers.h"

const unsigned DENOMINATOR = 255;

typedef struct PnmToFloat_cl {
        unsigned denom, width, height;
        UArray2_T *Float_rgbs;
} PnmToFloat_cl;



void PnmToFloatMap( int x,
                    int y,
                    A2Methods_UArray2 pixels,
                    A2Methods_Object *pixel,
                    void *UArray2_rgb);
void FloatToPnmMap( int x,
                    int y,
                    UArray2_T Float_rgbs,
                    void *pixel,
                    void *New_Pnm);
Float_rgb *PnmToFloatIndi( struct Pnm_rgb pnm_rgb, unsigned denom);
struct Pnm_rgb *FloatToPnmIndi( Float_rgb float_rgb, unsigned denom);



/* PUBLIC FUNCTIONS */

/*
 * Pnm_ppmToFloat_rgbs
 *      purp: Converts a Pnm_ppm object to a UArray2_T of Float_rgb structs.
 *            CRE for Pnm_ppm to be NULL.
 *      args: Pnm_ppm
 *      rets: UArray2_T of Float_rgb structs
 */
UArray2_T Pnm_ppmToFloat_rgbs( Pnm_ppm image)
{
        assert( image);
        
        int width  = (int)(image -> width);
        int height = (int)(image -> height);
        
        /* create the UArray2_T of Float_rgbs */
        UArray2_T Float_rgbs = UArray2_new( width, 
                                            height, 
                                            sizeof( Float_rgb));
        
        A2Methods_T methods = (A2Methods_T)(image -> methods);

        /* construct a closure pointer */
        PnmToFloat_cl *closure = malloc( sizeof( PnmToFloat_cl));
        closure -> denom  = image -> denominator;
        closure -> width  = image -> width;
        closure -> height = image -> height;
        closure -> Float_rgbs = &Float_rgbs;
        
        /* map through the image's pixels and put into Float_rgbs array */
        methods -> map_default( image -> pixels, PnmToFloatMap, closure);
        
        free( closure);

        return Float_rgbs;
}

/*
 * Float_rgbsToPnm_ppm
 *      purp: Converts a UArray2_T of Float_rgbs structs to a Pnm_ppm.
 *            CRE for UArray2_T to be NULL.
 *      args: UArray2_T of Float_rgb structs
 *      rets: Pnm_ppm
 */
Pnm_ppm Float_rgbsToPnm_ppm( UArray2_T Float_rgbs)
{
        assert( Float_rgbs);
        
        Pnm_ppm New_Pnm = malloc( sizeof( struct Pnm_ppm));
        assert( New_Pnm != NULL);

        New_Pnm -> denominator = DENOMINATOR;
        New_Pnm -> methods = uarray2_methods_plain;
        New_Pnm -> width = UArray2_width( Float_rgbs);
        New_Pnm -> height = UArray2_height( Float_rgbs);
        New_Pnm -> pixels = New_Pnm -> methods -> new( New_Pnm -> width, 
                                                       New_Pnm -> height, 
                                                     sizeof(struct Pnm_rgb));

        /* map through the Float_rgb structs and put them into the Pnm_ppm */
        UArray2_map_row_major(Float_rgbs, FloatToPnmMap, New_Pnm);

        return New_Pnm;
}

/* PRIVATE FUNCTIONS */

/*
 * Pnm_ppmToFloatMap
 *      purp: An apply function to be called on a Pnm_ppm, given a 
 *            UArray2_T of Float_rgbs in a closure pointer. Converts a 
 *            Pnm_rgb pixel of a Pnm_ppm to a Float_rgb struct and inserts 
 *            it into a 2D array of Float_rgbs.
 *      args: int x, int y, A2Methods_UArray2 pixels, 
 *            A2Methods_Object *pnm_rgb_ptr pointer to a Pnm_rgb, 
 *            void *closure pointer to a PnmToFloat_cl struct
 *      rets: none
 */
void PnmToFloatMap( int x,
                    int y,
                    A2Methods_UArray2 pixels,
                    A2Methods_Object *pnm_rgb_ptr,
                    void *closure)
{       
        (void)pixels;

        unsigned denom  = ((PnmToFloat_cl *)closure) -> denom;
        unsigned width  = ((PnmToFloat_cl *)closure) -> width;
        unsigned height = ((PnmToFloat_cl *)closure) -> height;

        /* if image has been "trimmed" */       
        if (((unsigned)x >= width) || ((unsigned)y >= height)) {
                return;
        }

        /* convert Pnm_rgb to Float_rgb */      
        struct Pnm_rgb pnm_rgb = *( (struct Pnm_rgb *) pnm_rgb_ptr);
        Float_rgb *float_rgb = PnmToFloatIndi( pnm_rgb, denom);

        /* put new Float_rgb into array */
        UArray2_T Float_rgbs = *(((PnmToFloat_cl *)closure) -> Float_rgbs);
        Float_rgb *float_rgb_at = UArray2_at( Float_rgbs, x, y);
        *float_rgb_at = *float_rgb;

        free( float_rgb);
}

/*
 * FloatToPnmMap
 *      purp: An apply function to be called on a UArray2_T of Float_rgbs, 
 *            given a closure pointer to an unpopulated Pnm_ppm. Converts a 
 *            Float_rgb to Pnm_rgb and inserts it into the Pnm_ppm.
 *      args: int x, int y, UArray2_T Float_rgbs, void *float_rgb_ptr,
 *            void *New_Pnm_ptr
 *      rets: none
 */
void FloatToPnmMap( int x,
                    int y,
                    UArray2_T Float_rgbs,
                    void *float_rgb_ptr,
                    void *New_Pnm_ptr)
{
        (void)Float_rgbs;

        /* convert Float_rgb to Pnm_rgb */
        Float_rgb float_rgb = *( (struct Float_rgb *) float_rgb_ptr);
        struct Pnm_rgb *pnm_rgb = FloatToPnmIndi( float_rgb, DENOMINATOR);

        Pnm_ppm New_Pnm = (Pnm_ppm)New_Pnm_ptr;
        A2Methods_T methods = (A2Methods_T)(New_Pnm -> methods);

        /* insert Pnm_rgb into Pnm_ppm */
        struct Pnm_rgb *pnm_rgb_at = methods -> at(New_Pnm -> pixels, x, y);
        *pnm_rgb_at = *pnm_rgb;

        free( pnm_rgb);
}

/*
 * PnmToFloatIndi
 *      purp: Given a Pnm_rgb and the denominator of its image: converts its 
 *            members to floats, standardizes them to the denominator,
 *            inserts them into a constructed Float_rgb, and returns a 
 *            pointer to the Float_rgb.
 *            CRE for any member of the Pnm_rgb to be greater than denom.
 *      args: Pnm_rgb, unsigned denom
 *      rets: Float_rgb *
 */
Float_rgb *PnmToFloatIndi( struct Pnm_rgb pnm_rgb, unsigned denom)
{
        Float_rgb *float_rgb = malloc( sizeof( struct Float_rgb));
        assert( float_rgb != NULL);

        assert(pnm_rgb.red   <= denom);
        assert(pnm_rgb.blue  <= denom);
        assert(pnm_rgb.green <= denom);

        if (denom == 0) {
                denom = 1;
        }

        /* standardize and insert Pnm_ppm members into Float_rgb */
        float_rgb -> r = (float)pnm_rgb.red   / (float)denom;
        float_rgb -> g = (float)pnm_rgb.green / (float)denom;
        float_rgb -> b = (float)pnm_rgb.blue  / (float)denom;
        
        /* make sure all floats are between 0 and 1 */
        float_rgb -> r = moveInBoundsFloat( float_rgb -> r, 0 ,1);
        float_rgb -> g = moveInBoundsFloat( float_rgb -> g, 0 ,1);
        float_rgb -> b = moveInBoundsFloat( float_rgb -> b, 0 ,1);

        return float_rgb;
}

/*
 * FloatToPnmIndi
 *      purp: Given a Float_rgb and the denominator of its destination 
 *            image, scales the floats of the rgb up to integers from zero to 
 *            denominator, inserts them into a constructed Pnm_rgb, and 
 *            returns a pointer to the Pnm_rgb.
 *      args: Float_rgb, unsigned denom
 *      rets: Pnm_rgb *
 */
struct Pnm_rgb *FloatToPnmIndi( Float_rgb float_rgb, unsigned denom)
{
        unsigned red, blue, green;

        struct Pnm_rgb *pnm_rgb_ptr = malloc( sizeof( struct Pnm_rgb));
        assert( pnm_rgb_ptr != NULL);

        if (denom == 0) {
                denom = 1;
        }

        /* make sure all floats are between 0 and 1 */
        float_rgb.r = moveInBoundsFloat( float_rgb.r, 0, 1);
        float_rgb.g = moveInBoundsFloat( float_rgb.g, 0, 1);
        float_rgb.b = moveInBoundsFloat( float_rgb.b, 0, 1);

        /* scale up Float_rgb members and insert into Pnm_rgb */
        pnm_rgb_ptr -> red   = (unsigned)(float_rgb.r * denom);
        pnm_rgb_ptr -> blue  = (unsigned)(float_rgb.b * denom);
        pnm_rgb_ptr -> green = (unsigned)(float_rgb.g * denom);

        red = pnm_rgb_ptr -> red;
        blue = pnm_rgb_ptr -> blue;
        green = pnm_rgb_ptr -> green;

        /* make sure all are less than the denominator */
        pnm_rgb_ptr -> red   = moveInBoundsUint( red,   0, denom);
        pnm_rgb_ptr -> blue  = moveInBoundsUint( blue,  0, denom);
        pnm_rgb_ptr -> green = moveInBoundsUint( green, 0, denom);

        return pnm_rgb_ptr;
}
