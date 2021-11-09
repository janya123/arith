
#include "uarray2.h"
#include "uarray.h"
#include "mem.h"
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"

struct UArray2_T {
    int width;
    int height;
    int size;
    UArray_T elems;
};

#define T UArray2_T

extern T UArray2_new (int width, int height, int size)
{
    assert(width >= 0 && height >= 0);
    T uarray2 = (T) calloc(1, sizeof(struct UArray2_T));
    assert(uarray2 != NULL);
    uarray2->width = width;
    uarray2->height = height;
    uarray2->size = size;
    uarray2->elems = UArray_new(width * height, size);
    return uarray2;
}


extern void  UArray2_free  (T *uarray2)
{
    assert(*uarray2 != NULL);
    T myuarray2 = *(T *) uarray2;
    UArray_free(&(myuarray2->elems));
    free (myuarray2);
}


extern int   UArray2_width (T  uarray2)
{
    assert(uarray2 != NULL);
    return uarray2->width;
}


extern int   UArray2_height (T  uarray2)
{
    assert(uarray2 != NULL);
    return uarray2->height;
}

extern int   UArray2_size  (T  uarray2)
{
    assert(uarray2 != NULL);
    return uarray2->size;
}


extern void *UArray2_at    (T  uarray2, int col, int row)
{

    assert(uarray2 != NULL);
    assert(col >= 0 && col < uarray2->width);
    assert(row >= 0 && row < uarray2->height);
    int index = row * uarray2->width + col;
    UArray_T array = uarray2->elems;
    return UArray_at(array, index);
}


extern void UArray2_map_row_major (T uarray2, void apply
(int col, int row, T a, void *p1, void *p2), void *cl)
{
    assert(uarray2 != NULL);
    int array_length = uarray2->width * uarray2->height;
    for (int i = 0; i < array_length; i++) {
        int row = i / uarray2->width;
        int col = i % uarray2->width;
        apply(col, row, uarray2, UArray2_at(uarray2, col, row), cl);
    }
}


extern void UArray2_map_col_major
(T uarray2, void apply(int col, int row, T a, void *p1, void *p2), void *cl)
{
    assert(uarray2 != NULL);
    int array_length = uarray2->width * uarray2->height;
    for (int i = 0; i < array_length; i++) {
        int row = i % uarray2->height;
        int col = i / uarray2->height;
        apply(col, row, uarray2, UArray2_at(uarray2, col, row), cl);
    }
}
