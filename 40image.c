/*
 * 40image.c
 * by Samuel Weaver (sweave05) and Ross Kamen (rkamen02)
 * October 21, 2016
 * HW 4
 *
 * An image compressor and decompressor
 */


#include "assert.h"
#include "compress40.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void (*compress_or_decompress)(FILE *input) = compress40;

/*
 * main
 *      purp: compresses or decompresses a given image a writes the compressed
 *              or decompressed file to stdout. The filename must be the last
 *              command line argument. -c means compress and -d means 
 *              decompression. If not filename is provided, reads from
 *              stdin. CRE if given information does not conform to standards.
 *              CRE if given file has either dimension < 2.
 *      args: a tag for compression or decompression and file name for reading
 *              in information if not to do so from stdin
 *      rets: 0 if properly executed, 1 if else
 */
int main(int argc, char *argv[])
{
        int i;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n",
                                argv[0], argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }
        assert(argc - i <= 1);    /* at most one file on command line */
        if (i < argc) {
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }

        return( 0);
}
