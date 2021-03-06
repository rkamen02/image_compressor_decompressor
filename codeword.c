/*
 * codeword.c
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 * HW 4
 *
 * Function implementations for codeword
 */

#include <assert.h>
#include <bitpack.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "codeword.h"
#include "helpers.h"
#include "transcomp.h"
#include <arith40.h>

typedef uint32_t Codeword;

const unsigned WIDTH_A  = 9;
const unsigned WIDTH_B  = 5;
const unsigned WIDTH_C  = 5;
const unsigned WIDTH_D  = 5;
const unsigned WIDTH_PB = 4;
const unsigned WIDTH_PR = 4;

const float MAX_MAG = 0.3;      
        
const unsigned LSB_A  = 23;
const unsigned LSB_B  = 18;
const unsigned LSB_C  = 13;
const unsigned LSB_D  = 8;
const unsigned LSB_PB = 4;
const unsigned LSB_PR = 0;

/* struct representing a quantized version of a cosine-transformed 2x2
 * block of pixels */
typedef struct Quant {
        unsigned pb, pr, a;
        int b, c, d;
} Quant;


/* PRIVATE FUNCTIONS: TRANSCOMP-QUANT CONVERSION */
UArray2_T TransCompsToQuants( UArray2_T TransComps);
UArray2_T QuantsToTransComps( UArray2_T Quants);
void TransToQuantMap( int x,
                      int y,
                      UArray2_T TransComps,
                      void *transcomp_ptr,
                      void *Quants_ptr);
void QuantToTransMap( int x,
                      int y,
                      UArray2_T Quants,
                      void *quant_ptr,
                      void *TransComps_ptr);
TransComp *QuantToTransIndi( Quant quant);
Quant *TransToQuantIndi( TransComp transcomp);
float unquantizeInt( int value, float max_mag, unsigned bit_width);
int quantizeFloat( float value, float max_mag, unsigned bit_width);

/* PRIVATE FUNCTIONS: QUANT-CODEWORD CONVERSION */
UArray2_T QuantsToCodewords(  UArray2_T Quants);
UArray2_T CodewordsToQuants(  UArray2_T Codewords);
void CodewordToQuantMap( int x,
                         int y,
                         UArray2_T Codewords,
                         void *codeword_ptr,
                         void *Quants_ptr);
void QuantToCodewordMap( int x,
                         int y,
                         UArray2_T Quants,
                         void *quant_ptr,
                         void *Codewords_ptr);
Quant *CodewordToQuantIndi( Codeword codeword);
Codeword QuantToCodewordIndi( Quant quant);

/* PRIVATE FUNCTIONS: CODEWORD READING AND WRITING */
void WriteCodewordMap( int x,
                       int y,
                       UArray2_T Codewords,
                       void *codeword_ptr,
                       void *file_ptr);
void ReadCodewordMap( int x,
                      int y,
                      UArray2_T Codewords,
                      void *codeword_ptr,
                      void *file_ptr);
Codeword ReadCodewordIndi( FILE *in);
void WriteCodewordIndi( Codeword codeword, FILE *out);

/* PRIVATE FUNCTIONS: MISC */
void assertCodewordFormat();




/* PUBLIC FUNCTIONS */

/*
 * TransCompsToCodewords
 *      purp: Given a UArray2_T of TransComp structs, first converts 
 *            TransComps into a UArray2_T of Quants, then converts Quants 
 *            into a UArray2_T of Codewords.
 *            CRE if TransComps is NULL.
 *      args: UArray2_T of TransComps
 *      rets: UArray2_T of Codewords
 */
UArray2_T TransCompsToCodewords(  UArray2_T TransComps)
{
        assert( TransComps);
        assertCodewordFormat();

        UArray2_T Quants    = TransCompsToQuants( TransComps);
        UArray2_T Codewords = QuantsToCodewords( Quants);

        UArray2_free( &Quants);

        return Codewords;
}

/*
 * CodewordsToTransComps
 *      purp: Given a UArray2_T of TransComp structs, first converts 
 *            Codewords into a UArray2_T of Quants, then converts Quants 
 *            into a UArray2_T of TransComps.
 *            CRE if Codewords is NULL.
 *      args: UArray2_T of Codewords
 *      rets: UArray2_T of TransComps
 */
UArray2_T CodewordsToTransComps(  UArray2_T Codewords)
{
        assert( Codewords);
        assertCodewordFormat();

        UArray2_T Quants     = CodewordsToQuants( Codewords);
        UArray2_T TransComps = QuantsToTransComps( Quants);

        UArray2_free( &Quants); 
        
        return TransComps;
}

/*
 * writeCodewords
 *      purp: Given a UArray2_T of Codewords and a file destination, writes
 *            the Codewords to the file. 
 *            CRE if any Codeword is improperly formatted.
 *      args: FILE *, UArray2_T of Codewords
 *      rets: none
 */
void writeCodewords( FILE *out, UArray2_T Codewords)
{
        assertCodewordFormat();

        unsigned width  = UArray2_width(  Codewords);
        unsigned height = UArray2_height( Codewords);

        printf("COMP40 Compressed image format 2\n%u %u\n", 
               2 * width,
               2 * height);

        UArray2_map_row_major( Codewords, WriteCodewordMap, out);
}

/*
 * readCodewords
 *      purp: Given a file source containing a list of codewords for an 
 *            image, creates a UArray2_T of Codewords.
 *            CRE if image header is improperly formatted. 
 *            CRE if any Codeword is improperly formatted.
 *            CRE if width or height of the image is less than 1.
 *      args: FILE *
 *      rets: UArray2_T of Codewords
 */
UArray2_T readCodewords( FILE *in)
{
        assertCodewordFormat();
        
        unsigned height, width;

        /* read in header and check formatting */
        int read = fscanf( in, 
                           "COMP40 Compressed image format 2\n%u %u",
                           &width,
                           &height);
        assert( read == 2);

        read = getc( in);
        assert( read == '\n');
        assert( (width > 1) && (height > 1));
        
        UArray2_T Codewords = UArray2_new( width / 2, 
                                           height / 2,
                                           sizeof( Codeword));
        
        UArray2_map_row_major( Codewords, ReadCodewordMap, in);
        
        return Codewords;
}



/* PRIVATE FUNCTIONS: TRANSCOMP-QUANT CONVERSION */

/*
 * TransCompsToQuants
 *      purp: Given a UArray2_T of TransComp structs, converts it into a
 *            2D array of Quant structs.
 *      args: UArray2_T of TransComps
 *      rets: UArray2_T of of Quants
 */
UArray2_T TransCompsToQuants( UArray2_T TransComps)
{
        int width  = UArray2_width(  TransComps);
        int height = UArray2_height( TransComps);

        UArray2_T Quants = UArray2_new( width, 
                                        height,
                                        sizeof( struct Quant));

        UArray2_map_row_major( TransComps, TransToQuantMap, &Quants);

        return Quants;
}

/*
 * QuantsToTransComps
 *      purp: Given a UArray2_T of Quant structs, converts it into a
 *            2D array of TransComp structs.
 *      args: UArray2_T of Quants
 *      rets: UArray2_T of TransComps
 */
UArray2_T QuantsToTransComps( UArray2_T Quants)
{

        int width  = UArray2_width(  Quants);
        int height = UArray2_height( Quants);

        UArray2_T TransComps = UArray2_new( width, 
                                            height, 
                                            sizeof( struct TransComp));

        UArray2_map_row_major( Quants, QuantToTransMap, &TransComps);

        return TransComps;
}

/*
 * TransToQuantMap
 *      purp: An apply function to be called on a UArray2_T of TransComps, 
 *            given a closure pointer to a UArray2_T of Quants. Converts a 
 *            TransComp struct into a Quant struct and inserts the Quant 
 *            struct into the UArray2_T of Quants.
 *      args: int x, int y, UArray2_T TransComps, void *transcomp_ptr,
 *            void *Quants_ptr
 *      rets: none
 */
void TransToQuantMap( int x,
                      int y,
                      UArray2_T TransComps,
                      void *transcomp_ptr,
                      void *Quants_ptr)
{
        (void)TransComps;

        /* convert TransComp to Quant */
        struct TransComp transcomp = *((TransComp *)transcomp_ptr);
        struct Quant *quant = TransToQuantIndi( transcomp);

        /* put new Quant into array */
        UArray2_T Quants = *((UArray2_T *)Quants_ptr);
        struct Quant *quant_at = UArray2_at( Quants, x, y);
        *quant_at = *quant;

        free( quant);
}

/*
 * QuantToTransMap
 *      purp: An apply function to be called on a UArray2_T of Quants,
 *            given a closure pointer to a UArray2_T of TransComps.
 *            Converts a Quant struct into a TransComp struct and inserts
 *            the TransComp struct into the UArray2_T of TransComps.
 *      args: int x, int y, UArray2_T Quants, void *quant_ptr,
 *            void *TransComps_ptr
 *      rets: none
 */
void QuantToTransMap( int x,
                      int y,
                      UArray2_T Quants,
                      void *quant_ptr,
                      void *TransComps_ptr)
{
        (void)Quants;

        /* convert Quant to TransComp */
        struct Quant quant = *((Quant *)quant_ptr);
        struct TransComp *transcomp = QuantToTransIndi( quant);

        /* insert new TransComp into array */
        UArray2_T TransComps = *((UArray2_T *)TransComps_ptr);
        struct TransComp *transcomp_at = UArray2_at( TransComps, x, y);
        *transcomp_at = *transcomp;

        free( transcomp);
}

/*
 * QuantToTransIndi
 *      purp: Given a Quant struct, converts each member into its 
 *            unquantized form, inserts each into a constructed TransComp, 
 *            and returns a pointer to the TransComp.
 *      args: Quant
 *      rets: TransComp *
 */
TransComp *QuantToTransIndi( Quant quant)
{
        /* malloc space */
        TransComp *transcomp = malloc( sizeof( struct TransComp));
        assert( transcomp);

        float scale = pow( 2, WIDTH_A) - 1;

        /* populate members */
        transcomp -> pb = Arith40_chroma_of_index( quant.pb);
        transcomp -> pr = Arith40_chroma_of_index( quant.pr);

        transcomp -> a = (float)quant.a / scale;
        transcomp -> a = moveInBoundsFloat( transcomp -> a, 0, 1);

        transcomp -> b = unquantizeInt( quant.b, MAX_MAG, WIDTH_B);
        transcomp -> c = unquantizeInt( quant.c, MAX_MAG, WIDTH_C);
        transcomp -> d = unquantizeInt( quant.d, MAX_MAG, WIDTH_D);

        return transcomp;
}

/*
 * TransToQuantIndi
 *      purp: Given a TransComp struct, quantizes each member, inserts each
 *            into a constructed Quant, and returns a pointer to the Quant.
 *      args: TransComp
 *      rets: Quant *
 */
Quant *TransToQuantIndi( TransComp transcomp)
{
        /* malloc space */
        Quant *quant = malloc( sizeof( struct Quant));
        assert( quant);
        
        unsigned scale = pow( 2, WIDTH_A) - 1;

        /* populate members */
        quant -> pb = Arith40_index_of_chroma( transcomp.pb);
        quant -> pr = Arith40_index_of_chroma( transcomp.pr);   
        
        quant -> a = scale * (transcomp.a);
        quant -> a = moveInBoundsInt( quant -> a,  0, scale);

        quant -> b = quantizeFloat( transcomp.b, MAX_MAG, WIDTH_B);
        quant -> c = quantizeFloat( transcomp.c, MAX_MAG, WIDTH_C); 
        quant -> d = quantizeFloat( transcomp.d, MAX_MAG, WIDTH_D);
        
        return quant;
}

/*
 * unquantizeInt
 *      purp: Linearly converts a signed int in the domain of integers that
 *            may be represented with bit_width number of bits into a float 
 *            in the domain [ -max_mag -> max_mag ].
 *      args: int value, float max_mag, unsigned bit_width
 *      rets: float
 */
float unquantizeInt( int value, float max_mag, unsigned bit_width)
{
        float pos_scale = pow( 2, bit_width - 1) - 1;
        float neg_scale = -1 * (pow( 2, bit_width - 1));
        float ret_value;

        if ( value < 0 ) {
                ret_value = -1 * (max_mag * (float)value / neg_scale);
        } else {
                ret_value = max_mag * (float)value / pos_scale;
        }

        ret_value = moveInBoundsFloat( ret_value, -1 * max_mag, max_mag);

        return ret_value;
}

/*
 * quantizeFloat
 *      purp: Linearly converts a float from the domain 
 *            [ -max_mag -> max_mag ] into a signed int in the domain of 
 *            integers that may be represented with bit_width number of bits.
 *      args: float value, float max_mag, unsigned bit_width
 *      rets: int
 */
int quantizeFloat( float value, float max_mag, unsigned bit_width)
{       
        int pos_scale = pow( 2, bit_width - 1) - 1;
        int neg_scale = -1 * (pow( 2, bit_width - 1));
        int ret_value;
        
        if ( value < 0) {
                value = -1 * (((float)neg_scale) * value / max_mag);
                ret_value = ceil( value);
        } else {
                value = ((float)pos_scale) * value / max_mag;
                ret_value = floor( value);
        }
        
        ret_value = moveInBoundsInt( ret_value, neg_scale, pos_scale);
        
        return ret_value;       
}


/* PRIVATE FUNCTIONS: QUANT-CODEWORD CONVERSION */

/*
 * QuantsToCodewords
 *      purp: Converts a UArray2_T of Quant structs to a UArray2_T of 
 *            Codewords.
 *      args: UArray2_T of Quants
 *      rets: UArray2_T of Codewords
 */
UArray2_T QuantsToCodewords( UArray2_T Quants)
{
        int width  = UArray2_width(  Quants);
        int height = UArray2_height( Quants);

        UArray2_T Codewords = UArray2_new( width,
                                           height,
                                           sizeof( Codeword));

        UArray2_map_row_major( Quants, QuantToCodewordMap, &Codewords);

        return Codewords;
}

/*
 * CodewordsToQuants
 *      purp: Converts a UArray2_T of Codewords into a UArray2_T of Quant
 *            structs.
 *      args: UArray2_T of Codewords.
 *      rets: UArray2_T of Quants.
 */
UArray2_T CodewordsToQuants( UArray2_T Codewords)
{
        int width  = UArray2_width(  Codewords);
        int height = UArray2_height( Codewords);

        UArray2_T Quants = UArray2_new( width,
                                        height,
                                        sizeof( struct Quant));

        UArray2_map_row_major( Codewords, CodewordToQuantMap, &Quants);

        return Quants;
}

/*
 * CodewordToQuantMap
 *      purp: An apply function to be called on a UArray2_T of Codewords, 
 *            given a closure pointer to a UArray2_T of Quants. Converts a 
 *            Quant struct into a Codeword and inserts the Codeword into a 
 *            UArray2_T.
 *      args: int x, int y, UArray2_T Codewords, void *codeword_ptr,
 *            void *Quants_ptr
 *      rets: void
 */
void CodewordToQuantMap( int x,
                         int y,
                         UArray2_T Codewords,
                         void *codeword_ptr,
                         void *Quants_ptr)
{
        (void)Codewords;

        /* convert Codeword to Quant */
        Codeword codeword = *((Codeword *)codeword_ptr);
        Quant *quant = CodewordToQuantIndi( codeword);

        /* insert Quant into array */
        UArray2_T Quants = *((UArray2_T *)Quants_ptr);
        Quant *quant_at = UArray2_at( Quants, x, y);
        *quant_at = *quant;

        free( quant);
}

/*
 * QuantToCodewordMap
 *      purp: An apply function to be called on a UArray2_T of Quants,
 *            given a closure pointer to a UArray2_T of Codewords. Converts
 *            a Codeword into a Quant struct and inserts the Quant into a 
 *            UArray2_T.
 *      args: int x, int y, UArray2_T Quants, void *quant_ptr,
 *            void *Codewords_ptr
 *      rets: none
 */
void QuantToCodewordMap( int x,
                         int y,
                         UArray2_T Quants,
                         void *quant_ptr,
                         void *Codewords_ptr)
{
        (void)Quants;

        /* convert Quant to Codeword */
        Quant quant = *((Quant *)quant_ptr);
        Codeword code = QuantToCodewordIndi( quant);

        /* insert Codeword into array */
        UArray2_T Codewords = *((UArray2_T *)Codewords_ptr);
        Codeword *codeword_at = UArray2_at( Codewords, x, y);
        *codeword_at = code;
}

/*
 * CodewordToQuantIndi
 *      purp: Given a Codeword, populates a constructed Quant by extracting 
 *            data from predetermined fields in the Codeword.
 *      args: Codeword
 *      rets: Quant *
 */
Quant *CodewordToQuantIndi( Codeword codeword)
{
        Quant *quant = malloc( sizeof( struct Quant));
        assert( quant != NULL);

        quant -> pb = Bitpack_getu( codeword, WIDTH_PB, LSB_PB);
        quant -> pr = Bitpack_getu( codeword, WIDTH_PR, LSB_PR);
        quant -> a  = Bitpack_getu( codeword, WIDTH_A,  LSB_A); 

        quant -> b = Bitpack_gets( codeword, WIDTH_B, LSB_B);
        quant -> c = Bitpack_gets( codeword, WIDTH_C, LSB_C);
        quant -> d = Bitpack_gets( codeword, WIDTH_D, LSB_D);

        return quant;
}

/*
 * QuantToCodewordIndi
 *      purp: Given a Quant struct, populates a Codeword by inserting the 
 *            members of the Quant at predetermined locations in Codeword.
 *      args: Quant
 *      rets: Codeword
 */
Codeword QuantToCodewordIndi( Quant quant)
{
        Codeword codeword = 0;

        codeword = Bitpack_newu( codeword, WIDTH_PB, LSB_PB, quant.pb);
        codeword = Bitpack_newu( codeword, WIDTH_PR, LSB_PR, quant.pr);
        codeword = Bitpack_newu( codeword, WIDTH_A,  LSB_A,  quant.a);

        codeword = Bitpack_news( codeword, WIDTH_B, LSB_B, quant.b);
        codeword = Bitpack_news( codeword, WIDTH_C, LSB_C, quant.c);
        codeword = Bitpack_news( codeword, WIDTH_D, LSB_D, quant.d);

        return codeword;
}



/* PRIVATE FUNCTIONS: CODEWORD READING AND WRITING */

/*
 * WriteCodewordMap
 *      purp: An apply function to be called on a UArray2_T of Codewords, 
 *            given a closure pointer to a destination write file. Writes 
 *            the contents of a Codeword to the specified destination.
 *      args: int x, int y, UArray2_T Codewords, void *codeword_ptr,
 *            void *file_ptr
 *      rets: none
 */
void WriteCodewordMap( int x,
                       int y,
                       UArray2_T Codewords,
                       void *codeword_ptr,
                       void *file_ptr)
{
        (void)x;
        (void)y;
        (void)Codewords;
        
        Codeword codeword = *((Codeword *)codeword_ptr);
        WriteCodewordIndi( codeword, (FILE *)file_ptr);
}

/*
 * ReadCodewordMap
 *      purp: An apply function to be called on an unpopulated UArray2_T of 
 *            Codewords, given a closure pointer to a source file of 
 *            codewords. Populates the UArray2_T using the supplied 
 *            codewords in the file.
 *      args: int x, int y, UArray2_T Codewords, void *codeword_ptr,
 *            void *file_ptr
 *      rets: none
 */
void ReadCodewordMap( int x,
                      int y,
                      UArray2_T Codewords,
                      void *codeword_ptr,
                      void *file_ptr)
{
        (void)x;
        (void)y;
        (void)Codewords;

        Codeword *codeword_at = (Codeword *)codeword_ptr;
        *codeword_at = ReadCodewordIndi( (FILE *)file_ptr);
}

/*
 * ReadCodewordIndi
 *      purp: Given a pointer to a source file of codewords, reads 
 *            sizeof( Codeword) - bytes from the file to create a new 
 *            Codeword.
 *            CRE if file ends with an incomplete Codeword.
 *      args: FILE *
 *      rets: Codeword
 */
Codeword ReadCodewordIndi( FILE *in)
{
        int byte;
        Codeword codeword;
        for (int lsb = sizeof( Codeword) - 1; lsb >= 0; lsb --) {
                byte = fgetc( in);
                assert( byte != EOF);
                codeword = Bitpack_newu( codeword, 8, lsb * 8, byte);
        }
        return codeword;
}

/*
 * WriteCodewordIndi
 *      purp: Given a Codeword and a pointer to a destination file, writes a
 *            Codeword to the file.
 *      args: Codeword, FILE *
 *      rets: none
 */
void WriteCodewordIndi( Codeword codeword, FILE *out)
{
        int byte;
        for (int lsb = sizeof(Codeword) - 1; lsb >= 0; lsb --) {
                byte = Bitpack_getu( codeword, 8, lsb * 8);
                putc( byte, out);               
        }
}



/* PRIVATE FUNCTIONS: MISC */

/*
 * assertCodewordFormat
 *      purp: Verifies that the set globals defining the field locations 
 *            within Codewords are compatible with the size of a Codeword.
 *      args: none
 *      rets: none
 */
void assertCodewordFormat()
{
        int n = 7;
        int next_bit;
        int widths[ 7] = {WIDTH_A, 
                         WIDTH_B,
                         WIDTH_C,
                         WIDTH_D,
                         WIDTH_PB,
                         WIDTH_PR,
                         0};
        int lsbs[ 7] = { LSB_A,
                        LSB_B,
                        LSB_C,
                        LSB_D,
                        LSB_PB,
                        LSB_PR,
                        (int)(8 * sizeof( Codeword))};

        for ( int i = 0; i < n; i ++) {
                for ( int j = 0; j < n; j ++) {
                        if ( (i == j) || (lsbs[i] > lsbs[j])) {
                                continue;
                        }

                        next_bit = widths[i] + lsbs[i];
                        assert( next_bit <= lsbs[j]);
                }
        }       
}
