/*
 * bitpack.c
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 *
 * Implementation for bitpack.c
 */

#include <bitpack.h>
#include <assert.h>
#include <except.h>

Except_T Bitpack_Overflow = { "Overflow packing bits" };

/*
 * is_positive
 *      purp: returns whether or not the given int64_t is positive
 *      args: an int64_t
 *      rets: whether or not its positive
 */
bool is_positive( int64_t n)
{
        uint64_t sign = (uint64_t)n >> 63;
        
        return sign == 0;
}

/*
 * Bitpack_fitsu
 *      purp: returns whether or not the givne uint64_t can fit
 *              in the given number of bits
 *      args: a uint64_t and number of bits
 *      rets: whether or not the number can fit in the given
 *              number of bits
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        uint64_t max;
        
        if ( width >= 64) {
                return true;
        }

        /* nothing can be held in 0 bits */
        if (width == 0) {
                return false;
        }
        
        /* 
         * the maximum positive value that 
         * can be held in k bits is 2^k - 1 
         */
        max = ((uint64_t)1 << width) - 1;
        
        if ( n > max ) {
                return false;
        }
        
        return true;
}

/*
 * Bitpack_fitss
 *      purp: returns whether or not the givne int64_t can fit
 *              in the given number of bits
 *      args: a int64_t and number of bits
 *      rets: whether or not the number can fit in the given
 *              number of bits
 */
bool Bitpack_fitss( int64_t n, unsigned width)
{
        uint64_t max;

        if ( width >= 64) {
                return true;
        }
        
        /* nothing can be held in 0 bits */
        if ( width == 0) {
                return false;
        }

        /* only -1 and 0 can be stored as a signed 1 bit number*/
        if ( width == 1) {
                if ( (n == 0) || (n == -1)) {
                        return true;
                }
                return false;
        }

        /* 
         * if n is positive, just use Bitpack_fitsu and 
         * disregard the sign bit
         */
        if ( is_positive( n)) {
                return Bitpack_fitsu( n, width - 1);
        }

        /*
         * figure out the maximum magnitude value that can be stored
         * as a negative number in k bits. (its 2^(k - 1)).
         */
        max = (uint64_t)1 << (width - 1) ;

        if ((uint64_t)(-1 * n) > max) {
                return false;
        }
        return true;
}

/*
 * Bitpack_getu
 *      purp: returns the width bits with a least significant bit of lsb
 *              in word. lsb is indexed starting at 0. CRE if 
 *              width bits could not fit in a 64 bits with lsb = lsb.
 *              if width == 0, returns 0
 *      args: the word, number of bits, least significant bit
 *      rets: the width bits with a lsb of lsb in word
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert( width + lsb <= 64);

        if (width == 0) {
                return 0;
        }
        
        /*
         * move the sequence of bits to the left end of word, erasing
         * all bits to the left of the wanted sequence
         */
        word = word << (64 - (width + lsb));

        /*
         * move the sequence of bits to the right end of word,
         * erasing all bits to the left of the wanted sequence and
         * preparing it for returning
         */
        word = word >> (64 - width);
                
        return word;
}

/*
 * Bitpack_gets
 *      purp: returns the width bits with a least significant bit of lsb
 *              in word. lsb is indexed starting at 0. CRE if 
 *              width bits could not fit in a 64 bits with lsb = lsb.
 *              if width == 0, returns 0
 *      args: the word, number of bits, least significant bit
 *      rets: the width bits with a lsb of lsb in word
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert( width + lsb <= 64);
                
        int64_t sign, null;
        
        if (width == 0) {
                return 0;
        }

        /*extract the sign bit*/
        sign = Bitpack_getu( word,         1, width + lsb - 1);

        /*extract the rest of the bits*/
        word = Bitpack_getu( word, width - 1,             lsb); 
        
        /*
         * if the sequence was negative, turn all bits in the extracted
         * word to the left of bit number width, to 1
        */
        if ( sign == 1 ) {

                /*create the sequence of 1s to OR the new word with */
                null = ~0;

                /*
                 * turn all of the bits that overlap with the desired 
                 * sequence to 0s
                */
                null = null << (width - 1);
        
                word = null | word;
        }
        
        return word;
}

/*
 * Bitpack_newu
 *      purp: given a word, sequence of bits, a lsb, and the width of the
 *              sequence, inserts the sequence into the word at lsb and
 *              returns the new word. CRE if the sequence would not fit
 *              in the given word or in the given number of bits
 *      args: the word to be inserted into, the width of the sequence
 *              to be inserted, the lsb, the sequence of bits
 *      rets: the new word with the sequence inserted
 */
uint64_t Bitpack_newu(uint64_t word, 
                      unsigned width, 
                      unsigned lsb, 
                      uint64_t value)
{       
        assert( width + lsb <= 64);
        uint64_t mask, null;

        if (!Bitpack_fitsu( value, width)) {
                RAISE( Bitpack_Overflow);
        }
        
        /* make mask a sequence of 1s with 0s of width width at lsb*/
        mask =  0;
        null = ~0;

        /* you can not do shifts of the same size as the length of a word*/
        if ( lsb + width < 64) {

                /*create the left side of the mask*/
                mask = null << (width + lsb);
        }
        if ( lsb  > 0) {

                /* create the right side of the mask*/
                mask = mask | (null >> (64 - lsb));
        }

        word  = word  &  mask;
        value = value << lsb;

        return word | value;
}

/*
 * Bitpack_news
 *      purp: given a word, sequence of bits, a lsb, and the width of the
 *              sequence, inserts the sequence into the word at lsb and
 *              returns the new word. CRE if the sequence would not fit
 *              in the given word or in the given number of bits
 *      args: the word to be inserted into, the width of the sequence
 *              to be inserted, the lsb, the sequence of bits
 *      rets: the new word with the sequence inserted
 */
uint64_t Bitpack_news(uint64_t word, 
                      unsigned width,
                      unsigned lsb, 
                      int64_t value)
{
        assert( width + lsb <= 64);

        if (!Bitpack_fitss( value, width)) {
                RAISE( Bitpack_Overflow);
        }

        /*
         * extract the value from value so that it can be used in Bitpack_newu
         * this means we will pretend the sign bit is a normal bit
         */
        value = Bitpack_getu( value, width, 0);
        word  = Bitpack_newu( word, width, lsb, value);
        
        return word;
}
