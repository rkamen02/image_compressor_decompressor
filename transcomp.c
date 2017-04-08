/*
 * transcomp.c
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 * HW 4
 *
 * Function implementations for transcomp
 */

#include "component.h"
#include "helpers.h"
#include "transcomp.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


void CompToTransMap( int x,
                     int y,
                     UArray2_T Components,
                     void *component_ptr,
                     void *TransComps_ptr);
void TransToCompMap( int x,
                     int y,
                     UArray2_T TransComps,
                     void *transComp_ptr,
                     void *Components_ptr);
TransComp *CompToTransIndi( int x, int y, UArray2_T Components);
void       TransToCompIndi( int x, 
                            int y,      
                            UArray2_T TransComps, 
                            UArray2_T *Components);
void insertCompValues( UArray2_T *Components, 
                       int x, 
                       int y, 
                       float Y, 
                       float pb, 
                       float pr);


/* PUBLIC FUNCTIONS */

/*
 * ComponentsToTransComps
 *      purp: Given a UArray2_T of Component pixels, converts each 2x2 box 
 *            of Components into a TransComp struct via cosine transform. 
 *            Returns a UArray2_T of TransComps.
 *            CRE if 2D array is NULL.
 *      args: UArray2_T of Components
 *      rets: UArray2_T of TransComps
 */
UArray2_T ComponentsToTransComps( UArray2_T Components)
{
        assert( Components);

        UArray2_T TransComps = UArray2_new( UArray2_width (Components) / 2,
                                            UArray2_height(Components) / 2,
                                            sizeof( struct TransComp));

        UArray2_map_row_major( Components, CompToTransMap, &TransComps);

        return TransComps; 
}

/*
 * TransCompsToComponents
 *      purp: Given a UArray2_T of TransComps, converts each TransComps' 
 *            values into four Components via inverse cosine transform. 
 *            Inserts each Component into a UArray2_T and returns the array.
 *            CRE if 2D array is NULL.
 *      args: UArray2_T of TransComps
 *      rets: UArray2_T of Components
 */
UArray2_T TransComptsToComponents( UArray2_T TransComps)
{
        assert( TransComps);

        UArray2_T Components = UArray2_new( UArray2_width (TransComps) * 2,
                                            UArray2_height(TransComps) * 2,
                                            sizeof( struct Component));

        UArray2_map_row_major( TransComps, TransToCompMap, &Components);

        return Components;
}


/* PRIVATE FUNCTIONS */

/*
 * ComponentsToTransMap
 *      purp: An apply function to be called on a UArray2_T of Components, 
 *            given a closure pointer to a UArray2_T of TransComps. 
 *            Transforms a Component struct to a TransComp struct and 
 *            inserts it into a 2D array of TransComps.
 *      args: int x, int y, UArray2_T Components, void *component_ptr,
 *            void *TransComps_ptr
 *      rets: none
 */
void CompToTransMap( int x,
                     int y,
                     UArray2_T Components,
                     void *component_ptr,
                     void *TransComps_ptr)
{
        (void) component_ptr;
        
        /* only conduct transform on top left corner of each 2x2 box */
        if ( (x % 2 != 0) || (y % 2 != 0)) {
                return;
        }
        
        /* perform cosine transform on box of Components */
        TransComp *transComp = CompToTransIndi( x, y, Components);

        /* put new TransComp into array */
        UArray2_T TransComps = *((UArray2_T *) TransComps_ptr);
        TransComp *transComp_at = UArray2_at( TransComps, x / 2, y / 2);
        *transComp_at = *transComp;

        free( transComp);
}

/*
 * TransToCompMap
 *      purp: An apply function to be called on a UArray2_T of TransComps,
 *            given a closure pointer to a UArray2_T of Components.
 *            Converts a TransComp struct to four Component structs and 
 *            inserts each into a 2D array of Components.
 *      args: int x, int y, UArray2_T TransComps, void *transComp_ptr,
 *            void *Components_ptr
 *      rets: none
 */
void TransToCompMap( int x,
                     int y,
                     UArray2_T TransComps,
                     void *transComp_ptr,
                     void *Components_ptr)
{
        (void)transComp_ptr;

        TransToCompIndi( x, y, TransComps, (UArray2_T *)Components_ptr);
}

/*
 * CompToTransIndi
 *      purp: Performs a cosine transform on a 2x2 block of Component 
 *            structs. Inserts resulting values into a constructed TransComp 
 *            struct and returns a pointer to that struct.
 *      args: int x coord, int y coord, UArray2_T Components
 *      rets: TransComp *
 */
TransComp *CompToTransIndi( int x, int y, UArray2_T Components)
{
        TransComp *trans_ptr = malloc( sizeof( TransComp));
        assert( trans_ptr != NULL);
        
        Component *comp_ptr;
        float pb_sum = 0;
        float pr_sum = 0;
        float Y[4];
        int x_mod = 0;
        int y_mod = 0;

        /* iterate row-major through 2x2 block of Components */
        for (int i = 0; i < 4; i ++) {
                x_mod = i % 2;
                y_mod = ((i - 1) > 0) ? 1 : 0;
                comp_ptr = (Component *)UArray2_at( Components, 
                                                    x + x_mod, 
                                                    y + y_mod);
                pb_sum += comp_ptr -> pb;
                pr_sum += comp_ptr -> pr;
                Y[ i] = comp_ptr -> y;
        }
        
        trans_ptr -> pb = pb_sum / 4.0;
        trans_ptr -> pr = pr_sum / 4.0;

        trans_ptr -> a = (Y[ 3 ] + Y[ 2 ]  + Y[ 1 ] + Y[ 0 ]) / 4.0;
        trans_ptr -> b = (Y[ 3 ] + Y[ 2 ]  - Y[ 1 ] - Y[ 0 ]) / 4.0;
        trans_ptr -> c = (Y[ 3 ] - Y[ 2 ]  + Y[ 1 ] - Y[ 0 ]) / 4.0;
        trans_ptr -> d = (Y[ 3 ] - Y[ 2 ]  - Y[ 1 ] + Y[ 0 ]) / 4.0;
        
        /* make sure each value is within applicable bounds */
        trans_ptr -> a  = moveInBoundsFloat( trans_ptr -> a,   0,   1);
        trans_ptr -> pb = moveInBoundsFloat( trans_ptr -> pb, -0.5, 0.5);
        trans_ptr -> pr = moveInBoundsFloat( trans_ptr -> pr, -0.5, 0.5);
        trans_ptr -> b  = moveInBoundsFloat( trans_ptr -> b,  -0.5, 0.5);
        trans_ptr -> c  = moveInBoundsFloat( trans_ptr -> c,  -0.5, 0.5);
        trans_ptr -> d  = moveInBoundsFloat( trans_ptr -> d,  -0.5, 0.5);

        return trans_ptr;
}

/*
 * ComponentsToTransComps
 *      purp: Performs an inverse cosine transform on a TransComp struct to 
 *            generate a 2x2 block of Component structs. Inserts each 
 *            Component into a 2D array.
 *      args: int x, int y, UArray2_T TransComps, UArray2_T *Components
 *      rets: none
 */
void TransToCompIndi( int x, 
                      int y,    
                      UArray2_T TransComps, 
                      UArray2_T *Components)
{
        TransComp *transComp = UArray2_at( TransComps, x, y);
        
        float pb = transComp -> pb;
        float pr = transComp -> pr;
        
        float a  = transComp -> a;
        float b  = transComp -> b;
        float c  = transComp -> c;
        float d  = transComp -> d;

        float Y[4];

        Y[0] = a - b - c + d;
        Y[1] = a - b + c - d;
        Y[2] = a + b - c - d;
        Y[3] = a + b + c + d;
        
        insertCompValues( Components, 2 * x,     2 * y,     Y[0], pb, pr);
        insertCompValues( Components, 2 * x + 1, 2 * y,     Y[1], pb, pr);
        insertCompValues( Components, 2 * x,     2 * y + 1, Y[2], pb, pr);
        insertCompValues( Components, 2 * x + 1, 2 * y + 1, Y[3], pb, pr);
}

/*
 * insertCompValues
 *      purp: Inserts given values into a Component struct located at (x,y) 
 *            in a UArray2_T.
 *      args: UArray2_T *Components, int x, int y,
 *            float Y, float pb, float pr
 *      rets: none
 */
void insertCompValues( UArray2_T *Components, 
                       int x, 
                       int y, 
                       float Y, 
                       float pb, 
                       float pr)
{
        Component *component = (Component *)UArray2_at( *Components, x, y);
        component -> y  = Y;
        component -> pb = pb;
        component -> pr = pr;
        
        component -> y  = moveInBoundsFloat( component -> y,   0,   1);
        component -> pb = moveInBoundsFloat( component -> pb, -0.5, 0.5);
        component -> pr = moveInBoundsFloat( component -> pr, -0.5, 0.5);
}
