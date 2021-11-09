/*
 * bitpack.c 
 * Purpose: Pack and retrieve values to and from words, manipulating bit fields
 *          with a 64 bit (8 byte) value word
 * Written by : Kenny Lin (klin04) and Janya Gambhir (jgambh01)
 *         on : 3/22/2021
 */

#include "bitpack.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "assert.h"

#define MAX_BITS 64
#define S_TYPE int64_t
#define US_TYPE uint64_t

Except_T Bitpack_Overflow = { "Overflow packing bits" };

/* shifting functions */
US_TYPE left_shift(US_TYPE initial, int shift_amount);
US_TYPE right_shift(US_TYPE initial, int shift_amount);

/* helper functions */
S_TYPE convert_from_twocomp(S_TYPE value, int width);
US_TYPE make_mask(int width, int lsb);

/* width test functions */

/*
 * Bitpack_fitsu
 * checks if an unsigned integer will fit into a word of width bits 
 * Inputs: a width of greater than 64 will result in a CRE 
 * Outputs: whether width bits can hold n
 */
bool Bitpack_fitsu(US_TYPE n, unsigned width)
{
    assert(width <= MAX_BITS);
    
    if (width <= 0) {
        return false;
    } else {
        return n <= left_shift(2, width - 1) - 1;
    }
}

/* field extraction functions */

/*
 * Bitpack_fitss
 * checks if a signed integer will fit into a word of width bits in 
 * two's complement 
 * Inputs: a width of greater than 64 will result in a CRE 
 * Outputs: whether width bits can hold n
 */
bool Bitpack_fitss( S_TYPE n, unsigned width)
{
    assert(width <= MAX_BITS);
    
    if (width <= 0) {
        return false;
    } else if (width < 2) {
        return n == 0 || n == 1;
    } else {
        int max_positive = (left_shift(2, width - 2)) - 1;
        int max_negative = -1 * (left_shift(2, width - 2));
        return n <= max_positive && n >= max_negative;
    }
}


/*
 * Bitpack_getu
 * Extracts a field from a word given the width of field and the location of 
 * the field's least significant bit
 * Inputs: word from which to extract field, unsigned ints representing width 
 *         and lsb
 *         Width must be less than the number of maximum bits and width + lsb
 *         must be less than the number of maximum bits
 * Outputs: For valid inputs, returns an unsigned value from word
 *         For invalid width and lsb values, CRE and program exits
 *          
 */
US_TYPE Bitpack_getu(US_TYPE word, unsigned width, unsigned lsb) 
{
    assert (width <= MAX_BITS);
    assert (width + lsb <= MAX_BITS);
    
    if (width == 0) {
        return 0;
    }
    
    US_TYPE mask = make_mask(width, lsb);
    return right_shift((word & mask), lsb);
}

/*
 * Bitpack_gets
 * Extracts a field from a word given the width of field and the location of 
 * the field's least significant bit
 * Inputs: word from which to extract field, unsigned ints representing width 
 *         and lsb
 *         Width must be less than the number of maximum bits and width + lsb
 *         must be less than the number of maximum bits
 * Outputs: For valid inputs, returns a signed value from word
 *         For invalid width and lsb values, CRE and program exits
 *          
 */
S_TYPE Bitpack_gets(US_TYPE word, unsigned width, unsigned lsb) 
{
    word = Bitpack_getu(word, width, lsb);
    return convert_from_twocomp(word, width);
}

/* field update functions */

/*
 * Bitpack_newu
 * Takes in a word and creates a duplicate with the field of width with 
 * least significant bit at lsb being replaced by a width-bit representation 
 * of an unsigned value
 * Inputs: word to duplicate, unsigned ints representing width 
 *         and lsb, an unsigned value
 *         Width must be less than the number of maximum bits and width + lsb
 *         must be less than the number of maximum bits
 * Outputs: For valid inputs, an unsigned word
 *         For invalid width and lsb values, CRE and program exits
 *          
 */
US_TYPE Bitpack_newu(US_TYPE word, unsigned width, unsigned lsb, US_TYPE value)
{
    assert (width <= MAX_BITS);
    assert (width + lsb <= MAX_BITS);
    
    if (!Bitpack_fitsu(value, width)) { /* check if value fits in width bits */
        RAISE(Bitpack_Overflow);
    }
    
    US_TYPE mask = make_mask(width, lsb);
    mask = ~mask;
    word = word & mask;
    value = left_shift(value, lsb);
    return word | value;
}

/*
 * Bitpack_news
 * Takes in a word and creates a duplicate with the field of width with 
 * least significant bit at lsb being replaced by a width-bit representation 
 * of a signed value
 * Inputs: word to duplicate, unsigned ints representing width 
 *         and lsb, a signed value
 *         Width must be less than the number of maximum bits and width + lsb
 *         must be less than the number of maximum bits
 * Outputs: For valid inputs, an unsigned word
 *         For invalid width and lsb values, CRE and program exits
 *          
 */
US_TYPE Bitpack_news(US_TYPE word, unsigned width, unsigned lsb, S_TYPE value)
{
    assert (width <= MAX_BITS);
    assert (width + lsb <= MAX_BITS);
    
    if (!Bitpack_fitss(value, width)) { /* check if value fits in width bits */
        RAISE(Bitpack_Overflow);
    }

    US_TYPE mask = make_mask(width, lsb);
    mask = ~mask;
    word = word & mask;
    US_TYPE mask2 = make_mask(width, 0); 
    value = value & mask2;
    value = left_shift(value, lsb);
    
    return word | value;
}

/* shifting functions */

/*
 * left_shift
 * Shifts a word left by a specified amount
 * Inputs: word to shift, integer representing amount by which to shift left
 * Outputs: shifted word
 */
US_TYPE left_shift(US_TYPE initial, int shift_amount) 
{
    return initial << shift_amount;
}

/*
 * right_shift
 * Shifts a word left by a specified amount
 * Inputs: word to shift, integer representing amount by which to shift left
 * Outputs: shifted word
 */
US_TYPE right_shift(US_TYPE initial, int shift_amount) 
{
    return initial >> shift_amount;
}

/* helper functions */

/*
 * make_mask
 * Creates a bit mask and performs necessary right and left shifts by the 
 * width and lsb
 * Inputs: integers representing width and lsb by which to shift the mask
 * Outputs: mask
 */
US_TYPE make_mask(int width, int lsb)
{
    US_TYPE mask = ~0;
    mask = right_shift(mask, MAX_BITS - width);
    mask = left_shift(mask, lsb);
    return mask;
}

/*
 * convert_from_twocomp
 * Takes a value and converts from its two's complement representation if it
 * is a negative signed value
 * Inputs: unsigned value on which to perform conversion, unsigned integer 
 *         representing width of field
 * Outputs: For valid inputs, returns the converted signed value
 *          
 */
S_TYPE convert_from_twocomp(S_TYPE value, int width) {
    bool is_negative = (value & left_shift(1, width - 1)) != 0;
    if (is_negative) {
        US_TYPE mask = ~0;
        mask = right_shift(mask, MAX_BITS - width);;
        value = (~value) & mask;
        return -1 * (value + 1);
    } else { /* no conversion needed */
        return value;
    }
}
