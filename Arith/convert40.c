/*
 * convert40.c 
 * Purpose: Convert between the different pixel representations used during 
 *          compression and decompression
 * Written by : Kenny Lin (klin04) and Janya Gambhir (jgambh01)
 *         on : 3/22/2021
 */
 
#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include "arith40.h"
#include "convert40.h"
#include "compress40.h"
#include "pnm.h"
#include "math40.h"

#define BCD_COEFF 103
#define A_COEFF 63
#define MAX_BCD 0.3
#define MIN_BCD -0.3
#define DENOMINATOR 255

/*
 * rgb_to_cv
 * Converts the rgb values in a pixel to component video color space values 
 * (Y/Pb/Pr), then returns these values stored in a colorspace struct
 * Input: Pnm_rgb Pixel to be converted, integer representing denominator of
 *        the image
 * Output: For valid inputs, a structure containing the component video color
 *         space values for the pixel specified
 *         For invalid inputs (null pixel), program exits with a CRE
 */
colorspace rgb_to_cv(Pnm_rgb pixel, int denom)
{
    assert(pixel != NULL);
    
    colorspace cv;

    float r = (float)pixel->red / (float) denom;
    float g = (float)pixel->green / (float) denom;
    float b = (float)pixel->blue / (float) denom;
    
    cv.y = 0.299 * r + 0.587 * g + 0.114 * b;
    cv.pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
    cv.pr = 0.5 * r - 0.418688 * g - 0.081312 * b;
    
    return cv;
}

/*
 * set_cv_to_rgb
 * converts a pixel from component video to rgb and populates the members of 
 * image's Pnm_rgb struct 
 * Input: a null cv will result in CRE
 * Output: the pixel at col, row in image should contain the rgb values 
 *         converted from cv
 */
void set_cv_to_rgb(int col, int row, colorspace cv, Pnm_ppm image)
{
    assert(image != NULL);
    
    Pnm_rgb converted_pixel = cv_to_rgb(cv, image->denominator);
    Pnm_rgb pixel = image->methods->at(image->pixels, col, row);
    
    pixel->red = converted_pixel->red;
    pixel->green = converted_pixel->green;
    pixel->blue = converted_pixel->blue;
    
    free(converted_pixel);
}

/*
 * cv_to_rgb
 * Converts the component video color space values to RGB values, then returns
 * these values in a Pnm_rgb struct
 * Input: colorspace struct to be converted, integer representing denominator of
 *        the image
 * Output: For valid inputs, a structure containing the rgb values of the pixel
 */
Pnm_rgb cv_to_rgb(colorspace cv, int denominator) 
{
    float r = 1.0 * cv.y + 0.0 * cv.pb + 1.402 * cv.pr;
    float g = 1.0 * cv.y - 0.344136 * cv.pb - 0.714136 * cv.pr;
    float b = 1.0 * cv.y + 1.772 * cv.pb + 0.0 * cv.pr;
    
    Pnm_rgb pixel = malloc(sizeof(*pixel));
    assert(pixel != NULL);
    
    int red = round_float(r * denominator);
    int green = round_float(g * denominator);
    int blue = round_float(b * denominator);
    
    pixel->red = check_range((float) red, (float) DENOMINATOR, 0.0);
    pixel->green = check_range((float) green, (float) DENOMINATOR, 0.0);
    pixel->blue = check_range((float) blue, (float) DENOMINATOR, 0.0);
    
    return pixel;
}

/*
 * store_colorspace
 * Takes four pixels and stores them into a colorspace_block struct to access
 * all four pixels in one block
 * Input: Pnm_rgb pixels to be stored (four), integer representing denominator
 *        of teh image
 * Output: For valid inputs, a structure containing the four pixels
 *         For invalid inputs (null pixels), CRE and program exits
 */
colorspace_block store_colorspace(Pnm_rgb tl, Pnm_rgb tr, Pnm_rgb ll, 
                                  Pnm_rgb lr, int denom)
{
    assert(tl != NULL);
    assert(tr != NULL);
    assert(ll != NULL);
    assert(lr != NULL);
    
    colorspace_block block;
    
    block.tl = rgb_to_cv(tl, denom);
    block.tr = rgb_to_cv(tr, denom);
    block.ll = rgb_to_cv(ll, denom);
    block.lr = rgb_to_cv(lr, denom);
    
    return block;
}

/*
 * cv_to_dct
 * Converts the component video color space values to a b c d values with 
 * direct cosine transform, storing the converted values in a dctspace struct 
 * which is returned
 * Input:  colorspace structs to be converted, containing four pixels
 * Output: For valid inputs, a structure containing the component video color
 *         space values for the pixel specified
 *         For invalid inputs (null pixel), program exits with a CRE
 */
dctspace cv_to_dct(colorspace_block cv_block) 
{
    dctspace dct;
    
    float Y1 = cv_block.tl.y;
    float Y2 = cv_block.tr.y;
    float Y3 = cv_block.ll.y;
    float Y4 = cv_block.lr.y;
    
    float ave_pb = (cv_block.tl.pb + cv_block.tr.pb + cv_block.ll.pb + 
                   cv_block.lr.pb) / 4.0;
    float ave_pr = (cv_block.tl.pr + cv_block.tr.pr + cv_block.ll.pr + 
                    cv_block.lr.pr) / 4.0;
    
    dct.pb = ave_pb;
    dct.pr = ave_pr;
    
    dct.a = (Y4 + Y3 + Y2 + Y1) / 4.0;
    dct.b = (Y4 + Y3 - Y2 - Y1) / 4.0;
    dct.c = (Y4 - Y3 + Y2 - Y1) / 4.0;
    dct.d = (Y4 - Y3 - Y2 + Y1) / 4.0;
    
    return dct;
}

/*
 * dct_to_cv
 * Converts the four luminance values of the pixel as well as Pb and Pr indices
 * into component video color space values via inverse direct cosine transform,
 * storing the converted values in a colorspace struct which is returned
 * Input:  dctspace struct to be converted
 * Output: For valid inputs, a structure containing the component video color
 *         space values for the pixel specified
 *         For invalid inputs (null pixel), program exits with a CRE
 */
colorspace_block dct_to_cv(dctspace dct) 
{
    colorspace_block cv_block;
    colorspace cv_tl, cv_tr, cv_ll, cv_lr;
    
    cv_tl.pb = cv_tr.pb = cv_ll.pb = cv_lr.pb = dct.pb;
    cv_tl.pr = cv_tr.pr = cv_ll.pr = cv_lr.pr = dct.pr;
    
    cv_tl.y = dct.a - dct.b - dct.c + dct.d;
    cv_tr.y = dct.a - dct.b + dct.c - dct.d;
    cv_ll.y = dct.a + dct.b - dct.c - dct.d;
    cv_lr.y = dct.a + dct.b + dct.c + dct.d;
    
    cv_block.tl = cv_tl;
    cv_block.tr = cv_tr;
    cv_block.ll = cv_ll;
    cv_block.lr = cv_lr;
    
    return cv_block;
}

/*
 * quantize
 * quantize the a, b, c, d, pb, and pr values from the dct
 * Input: pointers to dctspace struct to be quantized
 * Output: For valid inputs, a structure containing the quantized dct struct 
 *         members
 */
quant_dct quantize(dctspace dct)
{
    quant_dct qdct;
    
    qdct.pr = Arith40_index_of_chroma(dct.pr);
    qdct.pb = Arith40_index_of_chroma(dct.pb);
    
    qdct.a = round_float(A_COEFF * dct.a);
    qdct.b = round_float(BCD_COEFF * check_range(dct.b, MAX_BCD, MIN_BCD)); 
    qdct.c = round_float(BCD_COEFF * check_range(dct.c, MAX_BCD, MIN_BCD)); 
    qdct.d = round_float(BCD_COEFF * check_range(dct.d, MAX_BCD, MIN_BCD));
    
    return qdct;
}

/*
 * dequantize
 * dequantize the a, b, c, d, pb, and pr values for the dct
 * Input: pointers to 1dctspace struct to be dequantized
 * Output: For valid inputs, a structure containing the quantized dct struct 
 *         members
 */
dctspace dequantize(quant_dct qdct)
{
    dctspace dct;
    
    dct.pr = Arith40_chroma_of_index(qdct.pr);
    dct.pb = Arith40_chroma_of_index(qdct.pb);
    
    dct.a = (float) qdct.a / A_COEFF;
    dct.b = (float) qdct.b / BCD_COEFF;
    dct.c = (float) qdct.c / BCD_COEFF;
    dct.d = (float) qdct.d / BCD_COEFF;
    
    return dct;
}
