/*
 * compress40.c 
 * Purpose: Convert between full-color PPM images and compressed binary 
 *          image files
 * Written by : Kenny Lin (klin04) and Janya Gambhir (jgambh01)
 *         on : 3/22/2021
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "a2blocked.h"
#include "a2plain.h"
#include "arith40.h"
#include "assert.h"
#include "pnm.h"
#include "bitpack.h"
#include "convert40.h"
#include "math40.h"
#include "pack40.h"

#define BYTE_SIZE 8
#define WORD_SIZE 32

#define US_TYPE uint64_t

#define DENOMINATOR 255
#define BSIZE 2

typedef A2Methods_UArray2 A2;

/* closure for apply_compression function */
typedef struct compression_cl {
    Pnm_ppm image;
    A2 word_arr;
}compression_cl;

/* COMPRESSION FUNCTIONS */
void compress40(FILE *fp);
Pnm_ppm read_ppm(FILE *fp, A2Methods_T methods);
void apply_compression(int col, int row, A2 array, void *elem, void *cl);
void print_compressed(A2 words, A2Methods_T methods, int width, int height);
void apply_print(int col, int row, A2 array, void *elem, void *cl);

/* DECOMPRESSION FUNCTIONS */
void decompress40(FILE *fp);
void run_decompression(FILE *fp, Pnm_ppm image);
A2 read_words(FILE *fp, int words_width, int words_height);
void apply_decompression(int col, int row, A2 array, void *elem, void *cl);


/////////////////////////////
/*  COMPRESSION FUNCTIONS  */
/////////////////////////////

/*
 * compress40
 * Takes a file to compress and writes the compressed file to 
 * standard output   
 * Input: File stream pointer which contains the decompressed file specified on 
 *        the command line (cannot be null)
 * Output: For valid inputs, void (compressed image written to stdout)
 *         For invalid inputs (null file), CRE and program exits
 */
void compress40 (FILE *fp) 
{
    assert(fp != NULL);
    
    A2Methods_T methods_blocked = uarray2_methods_blocked;
    A2Methods_T methods_plain = uarray2_methods_plain;
    
    Pnm_ppm image = read_ppm(fp, methods_blocked);
    assert(image != NULL);
    
    compression_cl *cl = malloc(sizeof(*cl));
    assert(cl != NULL);
    
    cl->image = image;
    cl->word_arr = methods_plain->new(image->width / BSIZE, 
                                      image->height / BSIZE,
                                      sizeof(US_TYPE));
    
    methods_blocked->map_block_major(image->pixels, apply_compression, cl);
    
    print_compressed(cl->word_arr, methods_plain, image->width, image->height);
    
    methods_plain->free(&(cl->word_arr));
    
    free(cl);
    Pnm_ppmfree(&image);
}

/*
 * read_ppm
 * reads in and image from input and stores it in a Pnm_ppm struct, trimming
 * the width and height to even values if necessary
 * Input: an image with width or height less than 2 will result in CRE 
 * Output: Pnm_ppm struct with the pixmap trimmed if necessary to an even width
 *         and height
 *         Null file passed in will result in a CRE
 */
Pnm_ppm read_ppm(FILE *fp, A2Methods_T methods)
{
    assert(fp != NULL);
    
    Pnm_ppm image = Pnm_ppmread(fp, methods);
    
    assert(image->width >= 2 && image->height >= 2);
    
    if (image->width % 2 != 0) {
        image->width -= 1;
    }
    if (image->height % 2 != 0) {
        image->height -= 1;
    }

    return image;
}

/*
 * apply_compression
 * Iterates through the pixel array of an image and performs the actions 
 * necessary to compress the pixels
 * Input: integers representing column and row of the pixel array, 
 *        A2Methods_UArray2 through which to iterate, void pointer to 
 *        element in the array, void pointer to the Pnm_ppm struct of the image
 * Output: For valid inputs, void
 *         For invalid inputs, (null Pnm_ppm image passed in), CRE and program
 *         exits
 */
void apply_compression(int col, int row, A2 array, void *elem, void *cl) 
{
    (void)array;
    (void)elem;
    
    compression_cl closure = (*(compression_cl *) cl);
    assert(cl != NULL);
    
    Pnm_ppm image = (Pnm_ppm) closure.image;
    assert(image != NULL);
    
    A2Methods_T methods_plain = uarray2_methods_plain;
    A2 words = (A2)closure.word_arr;
    
    int denominator = image->denominator;
    
    /* only applies when in bottom right corner of a block */
    if (col % 2 == 1 && row % 2 == 1) {
        Pnm_rgb pixel_tl = image->methods->at(image->pixels, col - 1, row - 1);
        Pnm_rgb pixel_tr = image->methods->at(image->pixels, col, row - 1);
        Pnm_rgb pixel_ll = image->methods->at(image->pixels, col - 1, row);
        Pnm_rgb pixel_lr = image->methods->at(image->pixels, col, row);
        
        /* store pixels in block and convert from rgb to colorspace values */
        colorspace_block cv_block = store_colorspace(pixel_tl, pixel_tr, 
                                                     pixel_ll, pixel_lr, 
                                                     denominator);
        dctspace dct = cv_to_dct(cv_block);
        quant_dct qdct = quantize(dct);
        US_TYPE word = pack(qdct);
        
        /* store packed codeword into array */
        *((US_TYPE *)methods_plain->at(words, col / 2, row / 2)) = word;
    }
}

/*
 * print_compressed
 * Prints out header and words of compressed file to standard output
 * Input: A2Methods_UArray2 of words through which to iterate, A2 Methods 
 *        object used to traverse the array, integers representing width 
 *        and height of the image
 * Output: For valid inputs, void (print to stdout)
 *         For invalid inputs, (null word array passed in), CRE and program
 *         exits
 */
void print_compressed(A2 words, A2Methods_T methods, int width, int height)
{
    assert(words != NULL);
    
    printf("COMP40 Compressed image format 2\n%u %u\n", width, height);
    methods->map_row_major(words, apply_print, NULL);
}

/*
 * apply_print
 * Iterates through the UArray2 of words to print out each word after
 * compression has been performed
 * Input: integers representing column and row of the word array, 
 *        A2Methods_UArray2 through which to iterate, void pointer to 
 *        element in the array, void pointer cl (null parameters)
 * Output: For valid inputs, void, printing out the words in the array
 *         For invalid inputs, (null array elem), CRE and program
 *         exits
 */
void apply_print(int col, int row, A2 array, void *elem, void *cl)
{
    assert(elem != NULL);
    
    (void)col;
    (void)row;
    (void)array;
    (void)cl;
    
    US_TYPE word = (*(US_TYPE*) elem);
    
    for (int i = 0; i < WORD_SIZE / BYTE_SIZE; i++) {
        char byte = Bitpack_getu(word, BYTE_SIZE, 
                                WORD_SIZE - (BYTE_SIZE * (i + 1)));
        putchar(byte);
    }
}

/////////////////////////////
/* DECOMPRESSION FUNCTIONS */
/////////////////////////////

/*
 * decompress40
 * Takes a file to decompress and writes the decompressed file to 
 * standard output   
 * Input: File stream pointer which contains the compressed file specified on 
 *        the command line (cannot be null)
 * Output: For valid inputs, void (compressed image written to stdout)
 *         For invalid inputs (null file), CRE and program exits
 */
void decompress40 (FILE *fp)
{
    assert(fp != NULL);
    
    A2Methods_T methods = uarray2_methods_blocked;

    unsigned height, width;
    int read = fscanf(fp, "COMP40 Compressed image format 2\n%u %u", 
                      &width, &height);
    assert(read == 2);
    int c = getc(fp);
    assert(c == '\n');
    
    struct Pnm_ppm pixmap = { 
        .width = width, 
        .height = height, 
        .denominator = DENOMINATOR, 
        .methods = methods,
        .pixels = methods->new_with_blocksize
                                    (width, height, sizeof(struct Pnm_rgb), 2),
    };
    
    run_decompression(fp, &pixmap);
    Pnm_ppmwrite(stdout, &pixmap);
    
    methods->free(&pixmap.pixels);
}

/*
 * run_decompression
 * Performs steps necessary to decompress a file then stores decompressed
 * data in Pnm_ppm image   
 * Input: File stream pointer which contains the decompressed file specified
 *        on the command line (cannot be null), image to print after 
 *        decompression
 * Output: For valid inputs, void
 *         For invalid inputs (null file or image), CRE and program exits
 */
void run_decompression(FILE *fp, Pnm_ppm image)
{
    assert(fp != NULL);
    assert(image != NULL);
    
    int words_width = image->width / 2;
    int words_height = image->height / 2;
    
    A2Methods_T methods_plain = uarray2_methods_plain;
    A2 words = read_words(fp, words_width, words_height);
    
    methods_plain->map_row_major(words, apply_decompression, image);
    
    methods_plain->free(&words);
}


/*
 * decompress40
 * Takes a file and reads the words, storing the words into a 2d array   
 * Input: File stream pointer which contains the words, cre if NULL
 * Output: A 2d array of words for valid input
 *         For invalid inputs, null file pointer, CRE and program exits
 */
A2 read_words(FILE *fp, int words_width, int words_height)
{
    assert(fp != NULL);
    
    A2Methods_T methods_plain = uarray2_methods_plain;
    A2 words = methods_plain->new(words_width, words_height, sizeof(US_TYPE));
    
    int num_words = words_width * words_height;
    
    for (int i = 0; i < num_words; i++) {
        US_TYPE pixel = 0;
        
        for (int j = 0; j < WORD_SIZE / BYTE_SIZE; j++) {
            int byte = fgetc(fp);
            assert(byte != EOF); /* check if supplied file is too short */
            pixel = Bitpack_newu(pixel, 
                                 BYTE_SIZE, 
                                 WORD_SIZE - (BYTE_SIZE * (j + 1)), 
                                 byte); 
        }
        
        int pix_col = i % (words_width);
        int pix_row = i / (words_width);
        
        *((US_TYPE *)methods_plain->at(words, pix_col, pix_row)) = pixel;
    }
    return words;    
}

/*
 * apply_decompression
 * Iterates through the array of words and performs the actions 
 * necessary to decompress the pixels
 * Input: integers representing column and row of the word array, 
 *        A2Methods_UArray2 through which to iterate, void pointer to 
 *        element in the array, void pointer to the Pnm_ppm struct of the image
 * Output: For valid inputs, void
 *         For invalid inputs, (null Pnm_ppm image passed in), CRE and program
 *         exits
 */
void apply_decompression(int col, int row, A2 array, void *elem, void *cl)
{
    (void)array;
    
    Pnm_ppm image = (Pnm_ppm) cl;
    assert(image != NULL);
    
    US_TYPE word = *(US_TYPE *) elem;
    
    /* Converts words to luminence/Pb/Pr and to colorspace values */
    quant_dct qdct = unpack(word);
    dctspace dct = dequantize(qdct);
    colorspace_block cv_block = dct_to_cv(dct);
    
    col = col * 2 + 1;
    row = row * 2 + 1;
    
    /* Converts colorspace to rgb in pixel block */
    set_cv_to_rgb(col - 1, row - 1, cv_block.tl, image);
    set_cv_to_rgb(col, row - 1, cv_block.tr, image);
    set_cv_to_rgb(col - 1, row, cv_block.ll, image);
    set_cv_to_rgb(col, row, cv_block.lr, image);
}