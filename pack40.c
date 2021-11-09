/*
 * pack40.c 
 * Purpose: Interface to convert between bitpacked words and values in 
 *          quantized dct form
 * Written by : Kenny Lin (klin04) and Janya Gambhir (jgambh01)
 *         on : 3/22/2021
 */

#include "pack40.h"
#include "compress40.h"

#define LSB_A 26
#define LSB_B 20
#define LSB_C 14
#define LSB_D 8
#define LSB_PB 4
#define LSB_PR 0

#define WIDTHOF_A 6
#define WIDTHOF_BCD 6
#define WIDTHOF_PBPR 4

/*
 * pack
 * Packs luminence values of the pixel and average Pb and Pr indexes into a 32
 * bit word
 * Input: A struct containing a, b, c, d, pb average, and pr average values 
 *        to be packed into a 32-bit word
 * Output: For valid inputs, returns a word
 */
US_TYPE pack(quant_dct qdct)
{
    US_TYPE word = 0;
    
    word = Bitpack_newu(word, WIDTHOF_A, LSB_A, qdct.a);
    word = Bitpack_news(word, WIDTHOF_BCD, LSB_B, qdct.b);
    word = Bitpack_news(word, WIDTHOF_BCD, LSB_C, qdct.c);
    word = Bitpack_news(word, WIDTHOF_BCD, LSB_D, qdct.d);
    word = Bitpack_newu(word, WIDTHOF_PBPR, LSB_PB, qdct.pb);
    word = Bitpack_newu(word, WIDTHOF_PBPR, LSB_PR, qdct.pr);
    
    return word;
}

/*
 * unpack
 * Unpacks words into luminence values of the pixel and Pb and Pr 
 * indices
 * Input: A word to unpack
 * Output: For valid inputs, returns a struct containing luminence values and 
 *         Pb and Pr indices of the word
 */
quant_dct unpack(US_TYPE word)
{
    quant_dct qdct;
    
    qdct.a = Bitpack_getu(word, WIDTHOF_A, LSB_A);
    qdct.b = Bitpack_gets(word, WIDTHOF_BCD, LSB_B);
    qdct.c = Bitpack_gets(word, WIDTHOF_BCD, LSB_C);
    qdct.d = Bitpack_gets(word, WIDTHOF_BCD, LSB_D);
    qdct.pb = Bitpack_getu(word, WIDTHOF_PBPR, LSB_PB);
    qdct.pr = Bitpack_getu(word, WIDTHOF_PBPR, LSB_PR);
    
    return qdct;
}