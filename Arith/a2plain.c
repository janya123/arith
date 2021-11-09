/*
 * COMP40 Homework 3: locality
 * a2plain.c
 * March 88, 2021
 * Authors: Kyla Levin (klalic01) and Janya Gambhir (jgambh01)
 *
 * Purpose: Implements an A2Methods_T method suite that uses the ordinary 
 *          UArray2 as its two-dimensional array representation
 */


#include <string.h>

#include "assert.h"
#include "a2plain.h"
#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

/* new
 * Purpose: Returns a UArray2 with specified width, height, element size
 * Inputs:  3 non-negative integers representing width (number of columns), 
 *              height (number of rows), element size in the array
 * Outputs: For valid inputs, void pointer to a dynamically allocated UArray2.
 */
static A2Methods_UArray2 new(int width, int height, int size)
{
  return UArray2_new(width, height, size);
}

/* new_with_blocksize
 * Purpose: Returns a UArray2 with specified width, height, element size, 
 *              and blocksize
 * Inputs:  3 non-negative integers representing width (number of columns), 
 *              height (number of rows), element size in the array, blocksize
 * Outputs: For valid inputs, void pointer to a dynamically allocated UArray2.
 */
static A2Methods_UArray2 new_with_blocksize(int width, int height, int size, 
                                           int blocksize)
{
  (void) blocksize;
  return UArray2_new(width, height, size);
}

/* a2free
 * Purpose: Returns a UArray2 with specified width, height, element size, 
 *              and blocksize
 * Inputs: Array2 which needs to be freed
 * Outputs: For valid inputs, void pointer to a dynamically allocated UArray2b.
 */
static void a2free(A2Methods_UArray2 *array2p)
{
    assert (*array2p != NULL);
    UArray2_free((UArray2_T *) array2p);
}

/* width
 * Purpose: Returns the width of the specified array2
 * Inputs:  Array2 from which we will return the width
 * Outputs: For valid inputs, int representing width of array
 */
static int width(A2Methods_UArray2 array2)
{
    assert(array2 != NULL);
    return UArray2_width(array2);
}

/* height
 * Purpose: Returns the height of the specified array2
 * Inputs:  UArray2 from which we will return the height
 * Outputs: For valid inputs, int representing height of array
 */
static int height(A2Methods_UArray2 array2)
{
    assert(array2 != NULL);
    return UArray2_height(array2);
}

/* size
 * Purpose: Returns the size of the specified UArray2
 * Inputs:  UArray2 from which we will return the size
 * Outputs: For valid inputs, int representing size of element in the UArray2
 */
static int size(A2Methods_UArray2 array2)
{
    assert(array2 != NULL);
    return UArray2_size(array2);
}

/* blocksize
 * Purpose: Returns the blocksize of the specified A2Methods array
 * Inputs:  UArray2 from which we will return the blocksize
 * Outputs: For valid inputs, int representing blocksize in the array
 *          For non-blocked arrays, returns 0
 */
static int blocksize(A2Methods_UArray2 array2)
{
    assert(array2 != NULL);
    return 0;
}

/* at
 * Purpose: Returns the element at the specified index of an A2Methods array
 * Inputs:  T UArray2, two integers representing col and row. 
 * Outputs: A void pointer to the element at the specified col and row
 */
static A2Methods_Object *at(A2Methods_UArray2 array2, int i, int j)
{
    assert(array2 != NULL);
    return UArray2_at(array2, i, j);
}

static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
    assert(uarray2 != NULL);
    UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
    assert(uarray2 != NULL);
    UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

struct small_closure {
  A2Methods_smallapplyfun *apply;
  void                    *cl;
};

static void apply_small(int i, int j, UArray2_T array2,
                        void *elem, void *vcl)
{
  assert(array2 != NULL);
  struct small_closure *cl = vcl;
  (void)i;
  (void)j;
  (void)array2;
  cl->apply(elem, cl->cl);
}

static void small_map_row_major(A2Methods_UArray2 a2,
                                A2Methods_smallapplyfun apply,
                                void *cl)
{
    assert (apply != NULL);
    struct small_closure mycl = { apply, cl };
    UArray2_map_row_major(a2, apply_small, &mycl);
}

static void small_map_col_major(A2Methods_UArray2 a2,
                                A2Methods_smallapplyfun apply,
                                void *cl)
{
    assert(apply != NULL);
    struct small_closure mycl = { apply, cl };
    UArray2_map_col_major(a2, apply_small, &mycl);
}


static struct A2Methods_T uarray2_methods_plain_struct = {
    new,
    new_with_blocksize,
    a2free,
    width,
    height,
    size,
    blocksize,             // blocksize
    at,
    map_row_major,
    map_col_major,
    NULL,                  // map_block_major
    map_row_major,         // map_default
    small_map_row_major,
    small_map_col_major,
    NULL,                  // small_map_block_major
    small_map_row_major,   // small_map_default
};

// finally the payoff: here is the exported pointer to the struct
A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
