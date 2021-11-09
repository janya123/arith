/*
 * pack.c 
 * Purpose: Convert between bitpacked words and values in quantized dct form
 * Written by : Kenny Lin (klin04) and Janya Gambhir (jgambh01)
 *         on : 3/22/2021
 */
#ifndef PACK40_INCLUDED
#define PACK40_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "compress40.h"
#include "bitpack.h"
#include "convert40.h"

#define US_TYPE uint64_t

/*
 * pack
 * Packs luminence values of the pixel and average Pb and Pr indexes into a 32
 * bit word
 */
US_TYPE pack(quant_dct qdct);

/*
 * unpack
 * Unpacks words into luminence values of the pixel and Pb and Pr 
 * indices
 */
quant_dct unpack(US_TYPE word);

#endif