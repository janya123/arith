/*
 * convert40.h 
 * Purpose: Interface to convery between the different pixel representations 
 *          used during compression and decompression
 * Written by : Kenny Lin (klin04) and Janya Gambhir (jgambh01)
 *         on : 3/22/2021
 */

#include "pnm.h"

#ifndef CONVERT40_INCLUDED
#define CONVERT40_INCLUDED

/* represents a pixel in component video */
typedef struct colorspace {
    float y;
    float pb;
    float pr;
}colorspace;

/* holds 4 pixels in component video */
typedef struct colorspace_block {
    colorspace tl;
    colorspace tr;
    colorspace ll;
    colorspace lr;
}colorspace_block;

/* represents 4 pixels after the discrete cosine transformation */
typedef struct dctspace {
    float a;
    float b;
    float c;
    float d;
    float pb;
    float pr;
}dctspace;

/* represents four pixels with quantized dct values */
typedef struct quant_dct {
    int a; 
    int b;
    int c;
    int d;
    int pb;
    int pr;
}quant_dct;

/* CONVERTING BETWEEN RGB AND COMPONENT VIDEO */
/*
 * rgb_to_cv
 * Converts the rgb values in a pixel to component video color space values 
 * (Y/Pb/Pr), then returns these values stored in a colorspace struct
 */
colorspace rgb_to_cv(Pnm_rgb pixel, int denom);

/*
 * cv_to_rgb
 * Converts the component video color space values to RGB values, then returns
 * these values in a Pnm_rgb struct
*/
Pnm_rgb cv_to_rgb(colorspace cv, int denominator); 

/*
 * set_cv_to_rgb
 * converts a pixel from component video to rgb and populates the members of 
 * image's Pnm_rgb struct 
 */
void set_cv_to_rgb(int col, int row, colorspace cv, Pnm_ppm image); 

/*
 * store_colorspace
 * Takes four pixels and stores them into a colorspace_block struct to access
 * all four pixels in one block
 */
colorspace_block store_colorspace(Pnm_rgb tl, Pnm_rgb tr, Pnm_rgb ll, 
                                  Pnm_rgb lr, int denom);

/* CONVERTING BETWEEN COMPONENT VIDEO AND DCT*/ 
 
/*
 * cv_to_dct
 * Converts the component video color space values to a b c d values with 
 * direct cosine transform, storing the converted values in a dctspace struct 
 * which is returned
 */                               
dctspace cv_to_dct(colorspace_block cvblock);

/*
 * dct_to_cv
 * Converts the four luminance values of the pixel as well as Pb and Pr indices
 * into component video color space values via inverse direct cosine transform,
 * storing the converted values in a colorspace struct which is returned
 */
colorspace_block dct_to_cv(dctspace dct);

/* QUANTIZING DCT */
/*
 * quantize
 * quantize the a, b, c, d, pb, and pr values from the dct
 */
quant_dct quantize(dctspace dct);

/*
 * dequantize
 * dequantize the a, b, c, d, pb, and pr values for the dct
 */
dctspace dequantize(quant_dct qdct);

#endif
