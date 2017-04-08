/*
 * smalltest.c
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 * HW 4
 *
 * A series of small tests that test ideas and written code.
 * Includes a ppm creating function.
 */

#include "codeword.h"

#include <assert.h>
#include <a2methods.h>
#include <a2plain.h>
#include <bitpack.h>
#include <math.h>
#include <pnm.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Pnm_ppm Pnm_ppmcreate( unsigned width, unsigned height);
void bitstuff();

void bitpacktesting();
void fitsuTest();
void fitssTest();
void getuTest();
void getsTest();
void newuTest();
void newsTest();

/*
 * main
 *      purp: run other functions that conduct tests or write images
 *      args: none
 *      rets: 0 if an assert is not triggered
 */
int main()
{
        Pnm_ppm image = Pnm_ppmcreate(16, 16);
        Pnm_ppmwrite( stdout, image);
        Pnm_ppmfree( &image);

        /*bitpacktesting();     */

        /*bitstuff();*/
        return 0;
}

/*
 * Pnm_ppmcreate
 *      purp: create and populate a Pnm_ppm and return it,
 *              the r, g, and b values may be set with internal 
 *              formulas or set to constants
 *      args: the width and height of the desired Pnm_ppm image
 *      rets: a Pnm_ppm of the given width and height
 */
Pnm_ppm Pnm_ppmcreate( unsigned width, unsigned height)
{
        /* malloc space for the image */
        Pnm_ppm image = malloc( sizeof( struct Pnm_ppm));
        assert( image != NULL);

        /* variables used */
        struct Pnm_rgb *pixel;
        A2Methods_UArray2 pix;
        A2Methods_T methods;
        float r, g, b;
        int denom = 255;

        /* set member of the image */
        image -> width = width;
        image -> height = height;
        image -> denominator = denom;
        image -> methods = uarray2_methods_plain;

        /* set methods */
        methods = (A2Methods_T)(image -> methods);

        /* create the pixel map */
        pix = methods -> new( width, height, sizeof( struct Pnm_rgb));

        /* used a nested for loop to populate each pixel */
        for (unsigned i = 0; i < width; i++) {
                for (unsigned j = 0; j < height; j++){
                        r = 0;
                        g = 0;
                        b = 0;
                        /*r = denom;            */
                        /*g = denom;            */              
                        /*b = denom;            */
                        /*r = denom * 0.01;     */
                        /*g = denom * 0.01;     */
                        /*b = denom * 0.01;     */
                        r = denom * (1.0 - 
                                        ((float)i) / ((float)width));
                        /*g = denom * (1.0 - 
                          ((float)i) / ((float)width));         */
                        /*b = denom * (1.0 - 
                          ((float)i) / ((float)width));         */
                        /*r = denom * sqrt( pow( (0.5 - 
                          ((float)i) / ((float)width)), 2));    */
                        /*g = denom * sqrt( pow( (0.5 - 
                          ((float)i) / ((float)width)), 2));    */
                        /*b = denom * sqrt( pow( (1.0 - 2.0 * 
                          ((float)i) / ((float)width)), 2));    */

                        /* set the members of the pixel at (i, j) */
                        pixel = methods -> at( pix, i, j);
                        pixel -> red   = (unsigned)r;
                        pixel -> green = (unsigned)g;
                        pixel -> blue  = (unsigned)b;
                }
        }

        image -> pixels = pix;
        return image;
}

/*
 * printbytes
 *      purp: a function from lecture that prints the byte pairs
 *              of the given void * in endian order to stdout
 *      args: a void * pointing to some data and the data's size
 *      rets: nothing, prints to stdout
 */
void printbytes(void *p, unsigned int len)
{
        unsigned int i;
        unsigned char *cp = (unsigned char *)p; 

        for (i = 0; i < len; i++) {
                printf("%02X", cp[i]);
        }
}

/*
 * bitstuff
 *      purp: creates a code, prints it in big endian order starting with 
 *              the first two bytes, then first four, then first six and
 *              on until the whole thing is being printed then prints the 
 *              byte pairs in default endian order. 
 *      args: nothing
 *      rets: nothing, prints to stdout
 */
void bitstuff()
{
        printf("\n");

        uint32_t code = 10;
        uint32_t mask = ~0;
        int temp;

        printf( "%u %u\n", code, mask);

        for ( int i = 8 * (sizeof( code)-1); i >= 0; i -= 8) {
                temp = (code >> i) & mask;
                printf("%02X\n", temp);
        }

        printbytes( &code, sizeof( code));
        printf("\n");
        printf("\n");
}

/*
 * bitpacktesting
 *      purp: runs tests on bitpack.c and prints the results to stdout
 *      args: nothing
 *      rets: nothing, runs tests that print to stdout
 */
void bitpacktesting()
{
        printf("\n");
        /*
        fitsuTest();    
        fitssTest();
        */
        /*
        getuTest();
        */
        /*
        getsTest();
        */
        /*      
        newuTest();
        */
        newsTest();
        
        printf("\n");
}

/*
 * fitsuTest
 *      purp: tests Bitpack_fitsu by seeing how many bits certain
 *              numbers need to be represented with
 *      args: nothing
 *      rets: nothing, prints to stdout
 */
void fitsuTest()
{
        printf( "Bitpack_fitsu\n\n");

        bool fits;
        unsigned width;
        
        /* Try the numbers [0, 9] */
        for (int num = 0; num < 10; num ++) {
                width = 0;
                /*while num can't fit in width bits, increment width*/
                do {
                        fits = Bitpack_fitsu( num, width);
                        if ( fits) {
                                printf("%d fits in %d bits\n", num, width);
                        }else{
                                printf("%d does not fit in %d bits\n", 
                                       num,
                                       width);
                        }
                        width ++;
                } while ( !fits);
                printf("\n");
        }
}

/*
 * fitssTest
 *      purp: tests Bitpack_fitss by seeing how many bits certain
 *              numbers need to be represented with
 *      args: nothing
 *      rets: nothing, prints to stdout
 */
void fitssTest()
{
        printf( "Bitpack_fitsu\n\n");

        bool fits;
        unsigned width;
        
        /* Try the numbers [-9, 9] */
        for (int num = -9; num < 10; num ++) {
                width = 0;
                /*while num can't fit in width bits, increment width*/
                do {
                        fits = Bitpack_fitss( num, width);
                        if ( fits) {
                                printf("%d fits in %d bits\n", num, width);
                        }else{
                                printf("%d does not fit in %d bits\n", 
                                       num,
                                       width);
                        }
                        width ++;
                } while ( !fits);
                printf("\n");
        }
}

/*
 * getuTest
 *      purp: tests Bitpack_getu by setting a word and trying to 
 *              extract certain bytes, throws a CRE at its end
 *              because of attempted out of bounds extraction
 *      args: nothing
 *      rets: nothing, prints to stdout
 */
void getuTest()
{
        printf( "Bitpack_getu\n\n");

        uint64_t val = 0x1234567890abcdef;
        uint64_t ret = 0;

        /*Iterate through val in small endian order, extracting bytes*/
        for (unsigned int lsb = 0; lsb <= sizeof(val) * 8; lsb += 4) {
                ret = Bitpack_getu( val, 4, lsb);
                fprintf( stdout, 
                         "value: %lX, width: %u, lsb: %u = %lX\n", 
                         val, 4, lsb, ret);
        }

}

/*
 * getsTest
 *      purp: tests Bitpack_gets by setting a word and trying to 
 *              extract certain bytes, throws a CRE at its end
 *              because of attempted out of bounds extraction
 *      args: nothing
 *      rets: nothing, prints to stdout
 */
void getsTest()
{
        printf( "Bitpack_gets\n\n");

        int64_t val = 0x1234567890abcdef;
        int64_t ret = 0;

        /*Iterate through val in small endian order, extracting bytes*/
        for (unsigned int lsb = 0; lsb <= sizeof(val) * 8; lsb += 4) {
                ret = Bitpack_gets( val, 4, lsb);
                fprintf( stdout, 
                         "value: %lX, width: %u, lsb: %u = %li\n", 
                         val, 4, lsb, ret);
        }
}

/*
 * newuTest
 *      purp: tests the Bitpack_newu function by creating a base word
 *              and iterating through it in small endian order, 
 *              inserting a pair of bytes into it and checking
 *              if it was inserted properly and that the rest
 *              of the word is untouched. Throws a CRE at the end because of
 *              attempted out of bounds extraction
 *      args: nothing
 *      rets: nothing, prints to stdout
 */
void newuTest()
{
        printf( "Bitpack_newu\n\n");

        uint64_t word1 = 0x123456789abcdef0;
        uint64_t word2;
        uint64_t value = 0xdd;
        uint64_t width = 8;
        bool untouch, insert;
        
        /* Iterate through the word, starting at the small end*/
        for (unsigned lsb = 0; lsb <= 8 * sizeof( word1); lsb += 4) {
                /*create the new word, by inserting value at lsb*/
                word2   = Bitpack_newu( word1, width, lsb, value);
        
                /* check if it was inserted properly*/ 
                insert  = (value == Bitpack_getu( word2, width, lsb));
        
                /*check if the rest of the bytes weren't messed up*/
                untouch = true;
                if ( lsb != 0) {
                        untouch &= (Bitpack_getu( word2, lsb, 0) ==
                                    Bitpack_getu( word1, lsb, 0));
                }
                if ( (width - (lsb + width)) > 0) {
                        untouch &= (Bitpack_getu( word2, 
                                                  64-(lsb+width),
                                                  lsb+width) == 
                                    Bitpack_getu( word1,
                                                  64-(lsb+width),
                                                  lsb+width));
                }
        
                /*print the results of this test*/
                printf( "word1: %lX, word2: %lX, lsb: %d ",word1, word2, lsb);
                printf( "untocuhed: %d, inserted: %d\n", untouch, insert);
                if (!insert) {
                        printf("value: %lX, held: %lX\n", 
                               value, Bitpack_getu( word2, width, lsb));
                }
        }
}

/*
 * newsTest
 *      purp: tests the Bitpack_news function by creating a base word
 *              and iterating through it in small endian order, 
 *              inserting a pair of bytes into it and checking
 *              if it was inserted properly and that the rest
 *              of the word is untouched. Throws a CRE at the end because of
 *              attempted out of bounds extraction
 *      args: nothing
 *      rets: nothing, prints to stdout
 */
void newsTest()
{
        printf( "Bitpack_news\n\n");

        int64_t word1 = 0x123456789abcdef0;
        int64_t word2;
        int64_t value = (int8_t)0xdd;
        uint64_t width = 8;
        bool untouch, insert;
        
        /* Iterate through the word, starting at the small end*/
        for (unsigned lsb = 0; lsb <= 8 * sizeof( word1); lsb += 4) {
                /*create the new word, by inserting value at lsb*/
                word2   = Bitpack_news( word1, width, lsb, value);
        
                /* check if it was inserted properly*/ 
                insert  = (value == Bitpack_gets( word2, width, lsb));
        
                /*check if the rest of the bytes weren't messed up*/
                untouch = true;
                if ( lsb != 0) {
                        untouch &= (Bitpack_gets( word2, lsb, 0) ==
                                    Bitpack_gets( word1, lsb, 0));
                }
                if ( (width - (lsb + width)) > 0) {
                        untouch &= (Bitpack_gets( word2, 
                                                  64-(lsb+width),
                                                  lsb+width) == 
                                    Bitpack_gets( word1,
                                                  64-(lsb+width),
                                                  lsb+width));
                }
        
                /*print the results of this test*/
                printf( "word1: %lX, word2: %lX, lsb: %d ",word1, word2, lsb);
                printf( "untocuhed: %d, inserted: %d\n", untouch, insert);
                if (!insert) {
                        printf("value: %lX, held: %lX\n", 
                               value, Bitpack_gets( word2, width, lsb));
                }
        }
}
