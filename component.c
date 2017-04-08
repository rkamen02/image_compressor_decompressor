/*
 * floatrgb.c
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 * HW 4
 *
 * Function implementations for floatrgb
 */

#include "component.h"
#include "floatrgb.h"
#include <assert.h>
#include <stdlib.h>
#include "helpers.h"


void FloatToComponentMap( int x,
                          int y,
                          UArray2_T Float_rgbs,
                          void *pixel,
                          void *Components);
void ComponentToFloatMap( int x,
                          int y,
                          UArray2_T Components,
                          void *pixel,
                          void *Float_rgbs);
Float_rgb *ComponentToFloatIndi( Component component);
Component *FloatToCompIndi( Float_rgb float_rgb);


/* PUBLIC FUNCTIONS */

/*
 * Float_rgbsToComponents
 *      purp: Given a UArray2_T of Float_rgb pixels, converts each pixel's 
 *            RGB values to an equivalent component form and stores those 
 *            values into a UArray2_T of Component structs. Returns a 
 *            UArray2_T of Components. 
 *            CRE if 2D array is NULL.
 *      args: UArray2_T of Float_rgb structs
 *      rets: UArray2_T of Component structs
 */
UArray2_T Float_rgbsToComponents( UArray2_T Float_rgbs)
{
        assert( Float_rgbs);

        UArray2_T Components = UArray2_new( UArray2_width( Float_rgbs),
                                            UArray2_height( Float_rgbs),
                                            sizeof( struct Component));

        UArray2_map_row_major( Float_rgbs, FloatToComponentMap, &Components);

        return Components;
}

/*
 * ComponentsToFloat_rgbs
 *      purp: Given a UArray2_T of Components, converts each pixel's 
 *            component values into their RGB representations and stores 
 *            those values in a UArray2_T of Pnm_rgb structs. Returns a 
 *            UArray2_T of Pnm_rgb's.
 *            CRE if 2D array is NULL.
 *      args: UArray2_T of Component structs
 *      rets: UArray2_T of Float_rgb structs
 */
UArray2_T ComponentsToFloat_rgbs( UArray2_T Components)
{
        assert( Components);

        UArray2_T Float_rgbs = UArray2_new( UArray2_width( Components),
                                            UArray2_height( Components),
                                            sizeof( struct Float_rgb));

        UArray2_map_row_major( Components, ComponentToFloatMap, &Float_rgbs);

        return Float_rgbs;
}

/* PRIVATE FUNCTIONS */

/*
 * FloatToComponentMap
 *      purp: Apply function to be called on a UArray2_T of Float_rgbs, 
 *            given a closure pointer to a UArray2_T of Components. Converts 
 *            a Float_rgb struct to a Component struct and inserts it into 
 *            the UArray2_T of Components.
 *      args: int x, int y, UArray2_T of Float_rgb structs, void *float_ptr, 
 *            void *Components_ptr to a UArray2_T of Component structs
 *      rets: none
 */
void FloatToComponentMap( int x,
                          int y,
                          UArray2_T Float_rgbs,
                          void *float_ptr,
                          void *Components_ptr)
{
        (void) Float_rgbs;

        /* convert Float_rgb to Component */
        Float_rgb float_rgb = *( (struct Float_rgb *) float_ptr);
        Component *component = FloatToCompIndi( float_rgb);

        /* insert new Component into array */
        UArray2_T Components = *((UArray2_T *)Components_ptr);
        Component *component_at = UArray2_at( Components, x, y);
        *component_at = *component;

        free( component);
}

/*
 * ComponentToFloatMap
 *      purp: Apply function to be called on a UArray2_T of Components, 
 *            given a closure pointer to a UArray2_T of Float_rgbs. Converts 
 *            a Component struct to a Float_rgb struct and inserts it into 
 *            the UArray2_T of Float_rgbs.
 *      args: int x, int y, UArray2_T of Component structs, void *comp_ptr, 
 *            void *Float_rgbs_ptr to a UArray2_T of Float_rgb structs
 *      rets: none
 */
void ComponentToFloatMap( int x,
                          int y,
                          UArray2_T Components,
                          void *comp_ptr,
                          void *Float_rgbs_ptr)
{
        (void) Components;

        /* convert Component to Float_rgb */
        Component component = *( (struct Component *) comp_ptr);
        Float_rgb *float_rgb = ComponentToFloatIndi( component);

        /* insert new Float_rgb into array */
        UArray2_T Float_rgbs = *((UArray2_T *)Float_rgbs_ptr);
        Float_rgb *float_rgb_at = UArray2_at( Float_rgbs, x, y);
        *float_rgb_at = *float_rgb;

        free( float_rgb);
}

/*
 * ComponentToFloatIndi
 *      purp: Given a Component, converts its members to their RGB 
 *            representations, inserts them into a constructed Float_rgb, 
 *            and returns the Float_rgb.
 *      args: Component
 *      rets: Float_rgb
 */
Float_rgb *ComponentToFloatIndi( Component component)
{
        Float_rgb *float_rgb_ptr = malloc( sizeof( struct Float_rgb));
        assert( float_rgb_ptr != NULL);

        float y = component.y;
        float pb = component.pb;
        float pr = component.pr;

        float_rgb_ptr -> r = 1.0 * y + 0.0     * pb + 1.402   * pr;
        float_rgb_ptr -> g = 1.0 * y - .344136 * pb - .714136 * pr;
        float_rgb_ptr -> b = 1.0 * y + 1.772   * pb + 0.0     * pr;
        
        float_rgb_ptr -> r = moveInBoundsFloat( float_rgb_ptr -> r, 0, 1);
        float_rgb_ptr -> g = moveInBoundsFloat( float_rgb_ptr -> g, 0, 1);
        float_rgb_ptr -> b = moveInBoundsFloat( float_rgb_ptr -> b, 0, 1);
        
        return float_rgb_ptr;
}

/*
 * FloatToComponentIndi
 *      purp: Given a Float_rgb, converts its members to their component 
 *            representations, inserts them into a constructed Component, 
 *            and returns the Component.
 *      args: Float_rgb
 *      rets: Component
 */
Component *FloatToCompIndi( Float_rgb float_rgb)
{
        Component *comp_ptr = malloc( sizeof( struct Component));
        assert( comp_ptr != NULL);

        float r = float_rgb.r;
        float g = float_rgb.g;
        float b = float_rgb.b;

        comp_ptr ->  y =  .299    * r + .587    * g + .114    * b;
        comp_ptr -> pb = -.168736 * r - .331264 * g + .5      * b;
        comp_ptr -> pr =  .5      * r - .418688 * g - .081312 * b;

        comp_ptr ->  y = moveInBoundsFloat( comp_ptr ->  y,  0,   1);
        comp_ptr -> pb = moveInBoundsFloat( comp_ptr -> pb, -0.5, 0.5);
        comp_ptr -> pr = moveInBoundsFloat( comp_ptr -> pr, -0.5, 0.5);

        return comp_ptr;
}
