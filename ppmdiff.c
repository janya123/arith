#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <a2methods.h>
#include <a2plain.h>
#include <pnm.h>
#include <math.h>


void diff_images(FILE *f1, FILE *f2);
void check_dimensions(int larger_dimension, int smaller_dimension);
void calculate_diff(int small_width, int small_height, Pnm_ppm image1, 
                    Pnm_ppm image2);
double rgb_diff(Pnm_ppm image1, Pnm_ppm image2, int col, int row);
                    

int main(int argc, char *argv[])
{
    (void)argc;
    FILE *f1 = NULL;
    FILE *f2 = NULL;
    
    for (int i = 1; i < argc; i++) {
        if (*argv[i] == '-') {
            printf("%s\n", argv[i]);
            if (f1 != NULL && f2 == NULL) {
                f2 = stdin;
            } else {
                f1 = stdin;
            }
        } else {
            if (f1 != NULL && f2 == NULL) {
                f2 = fopen(argv[i], "r");
                assert (f2 != NULL);
            } else {
                f1 = fopen(argv[i], "r");
                assert (f1 != NULL);
            }
        }
    }
    if (f1 == NULL || f2 == NULL) {
        fprintf(stderr, "Usage\n");
        exit(EXIT_FAILURE);
    }
    
    diff_images(f1, f2);
    
    fclose(f1);
    fclose(f2);
    
    exit(EXIT_SUCCESS);
}

void diff_images(FILE *f1, FILE *f2)
{
    A2Methods_T methods = uarray2_methods_plain; 

    Pnm_ppm image1 = Pnm_ppmread(f1, methods);
    Pnm_ppm image2 = Pnm_ppmread(f2, methods);
    
    int small_width = 0;
    int small_height = 0;
    int large_width = 0;
    int large_height = 0;
    
    if (image1->width >= image2->width) {
        small_width = image2->width;
        large_width = image1->width;
    } else {
        small_width = image1->width;
        large_width = image2->width;
    }
    
    if (image1->height >= image2->height) {
        small_height = image2->height;
        large_height = image1->height;
    } else {
        small_height = image1->height;
        large_height = image2->height;
    }
    
    check_dimensions(large_width, small_width);
    check_dimensions(large_height, small_height);
    
    calculate_diff(small_width, small_height, image1, image2);
    Pnm_ppmfree(&image1);
    Pnm_ppmfree(&image2);
}

void check_dimensions(int larger_dimension, int smaller_dimension)
{
    if (larger_dimension - smaller_dimension > 1) {
        fprintf(stderr, "Difference is too large \n");
        printf("1.0 \n");
        exit(EXIT_FAILURE);
    }
}
    
void calculate_diff(int small_width, int small_height, Pnm_ppm image1, 
                    Pnm_ppm image2)
{   
    double rgb_difference = 0;
    for (int i = 0; i < small_width; i++) {
        for (int j = 0; j < small_height; j++) {
            rgb_difference += rgb_diff(image1, image2, i, j);
        }
    }
    double e_val = sqrt(rgb_difference / (3 * small_width * small_height));
    printf("%.4f\n", e_val);
}

double rgb_diff(Pnm_ppm image1, Pnm_ppm image2, int i, int j)
{
    Pnm_rgb pixel1 = image1->methods->at(image1->pixels, i, j);
    Pnm_rgb pixel2 = image2->methods->at(image2->pixels, i, j);
    
    double image1_denom = (double) image1->denominator;
    double image2_denom = (double) image2->denominator;
    
    double r = ((double) pixel1->red / image1_denom) 
               - ((double)pixel2->red / image2_denom);
    r *= r;
    
    double g = ((double) pixel1->green / image1_denom) 
               - ((double)pixel2->green / image2_denom);
    g *= g;
    
    double b = ((double) pixel1->blue / image1_denom) 
               - ((double)pixel2->blue / image2_denom);
    b *= b;
    
    return r + g + b;
}