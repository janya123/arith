#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "bitpack.h"

void test_fitsu();
void test_fitss();
void test_getu();
void test_gets();
void test_newu();
void test_news();

int main() 
{
    // printf("Testing fitsu ...\n");
    // test_fitsu();
    printf("Testing news ...\n");
    test_news();
    exit(EXIT_SUCCESS);
}


void test_fitsu()
{
    int input = 0;
    int width = 0;
    while (width != -1) {
        scanf("%d", &input);
        printf("INPUT NUMBER: %d\n", input);
        scanf("%d", &width);
        printf("WIDTH: %d\n", width);
        bool fits = Bitpack_fitsu(input, width);
        if (fits) {
            printf("FITS\n");
        } else {
            printf("DOES NOT FIT\n");
        }
    }
}

void test_fitss()
{
    int input = 0;
    int width = 0;
    while (width != -1) {
        scanf("%d", &input);
        printf("INPUT NUMBER: %d\n", input);
        scanf("%d", &width);
        printf("WIDTH: %d\n", width);
        bool fits = Bitpack_fitss(input, width);
        if (fits) {
            printf("FITS\n");
        } else {
            printf("DOES NOT FIT\n");
        }
    }
}

void test_getu()
{
    uint64_t input;
    int width = 0;
    int lsb = 0;
    
    while (width != -1) {
        scanf("%lu", &input);
        printf("INPUT: %lu\n", input);
        scanf("%d", &width);
        printf("WIDTH: %d\n", width);
        scanf("%d", &lsb);
        printf("LSB: %d\n", lsb);
        uint64_t result = Bitpack_getu(input, width, lsb);
        printf("%lu", result);
    }
}

void test_gets()
{
    int64_t input;
    int width = 0;
    int lsb = 0;
    
    while (width != -1) {
        scanf("%ld", &input);
        printf("INPUT: %lu\n", input);
        scanf("%d", &width);
        printf("WIDTH: %d\n", width);
        scanf("%d", &lsb);
        printf("LSB: %d\n", lsb);
        int64_t result = Bitpack_gets(input, width, lsb);
        printf("%ld", result);
    }
}

void test_newu()
{
    uint64_t input;
    int width = 0;
    int lsb = 0;
    uint64_t value;
    
    while (width != -1) {
        scanf("%lu", &input);
        printf("INPUT: %lu\n", input);
        scanf("%d", &width);
        printf("WIDTH: %d\n", width);
        scanf("%d", &lsb);
        printf("LSB: %d\n", lsb);
        scanf("%lu", &value);
        printf("VALUE: %lu\n", value);
        
        if (Bitpack_getu(Bitpack_newu(input, width, lsb, value), width, lsb) 
            == value) {
                printf("works\n");
            }
        uint64_t result = Bitpack_newu(input, width, lsb, value);
        printf("RETURNED WORD: %lu\n", result);
    }
    
}

void test_news()
{
    uint64_t input;
    int width = 0;
    int lsb = 0;
    int lsb2 = 0;
    int width2 = 0;
    int64_t value;
    
    while (width != -1) {
        scanf("%lu", &input);
        printf("INPUT: %lu\n", input);
        scanf("%d", &width);
        printf("WIDTH: %d\n", width);
        scanf("%d", &lsb);
        printf("LSB: %d\n", lsb);
        scanf("%ld", &value);
        printf("VALUE: %ld\n", value);
        scanf("%d", &width2);
        printf("WIDTH2: %d\n", width2);
        scanf("%d", &lsb2);
        printf("LSB2: %d\n", lsb2);
        
        if (Bitpack_gets(Bitpack_news(input, width, lsb, value), width, lsb) 
            == value) {
                printf("works\n");
            }
            
        // lsb2 >= w + lsb
        if (Bitpack_gets(Bitpack_news(input, width, lsb, value), width2, lsb2)
            == Bitpack_gets(input, width2, lsb2)) {
                printf("second rule works\n");
            }
        uint64_t result = Bitpack_news(input, width, lsb, value);
        printf("RETURNED WORD: %lu\n", result);
    }
    
}
