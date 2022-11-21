
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "umalloc.h"

int main(int argc, char **argv) {

    double test1, test2, test3, test4, test5 = 0;

    for (int iter = 0; iter < 50; iter++) {

        // test 1: malloc and then immediately free the ptr
        clock_t start1 = clock();

        for (int i = 0; i < 120; i++) {
            char *ptr = malloc(1);
            free(ptr);
        }

        clock_t end1 = clock();
        test1 += (double)(end1 - start1) / CLOCKS_PER_SEC;
        
        // test 2: malloc and then free sequentially 
        clock_t start2 = clock();

        char *arr[120];
        for (int i = 0; i < 120; i++) {
            arr[i] = malloc(1);
        }

        for (int i = 0; i < 120; i++) {
            free(arr[i]);
        }

        clock_t end2 = clock();
        test2 += (double)(end2 - start2) / CLOCKS_PER_SEC;

        // test 3: 
        clock_t start3 = clock();

        int mallocs = 0;
        int inArray = 0;
        char *arr2[120];

        while (mallocs != 120) {
            int randNum = rand() % 2;
            if (randNum == 0) {
                arr2[inArray] = malloc(1);
                mallocs++;
                inArray++;
            } else if (randNum == 1 && inArray != 0) {
                free(arr2[inArray - 1]);
                inArray--;
            }
        }

        while (inArray != 0) {
            free(arr2[inArray - 1]);
            inArray--;
        }

        clock_t end3 = clock();
        test3 += (double)(end3 - start3) / CLOCKS_PER_SEC;

        // test 4: testing the ability to split chunks
        clock_t start4 = clock();

        int fraction = 2;
        do {
            char *ptr = malloc(4096 / fraction);
            free(ptr);
            fraction *= 2;
        } while (fraction <= 4096);

        clock_t end4 = clock();
        test4 += (double)(end4 - start4) / CLOCKS_PER_SEC;

        // test 5: malloc and then freeing backwards
        clock_t start5 = clock();

        char *arr3[120];
        for (int i = 0; i < 120; i++) {
            arr3[i] = malloc(1);
        }

        for (int i = 119; i >= 0; i--) {
            free(arr3[i]);
        }

        clock_t end5 = clock();
        test5 += (double)(end5 - start5) / CLOCKS_PER_SEC;
    }

    printf("Test 1 Average Time: %f\n", test1 / 50);
    printf("Test 2 Average Time: %f\n", test2 / 50);
    printf("Test 3 Average Time: %f\n", test3 / 50);
    printf("Test 4 Average Time: %f\n", test4 / 50);
    printf("Test 5 Average Time: %f\n", test5 / 50);
    
    int* test = (int*)malloc(sizeof(int));
    char* str = (char*)malloc(1000000);
    free(test);
    free(str);
    free(str);

    return 0;
}
