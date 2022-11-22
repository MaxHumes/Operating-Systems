#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "umalloc.h"

//used for pointers in saturation test
static char* sat_arr[MEM_SIZE]; 
size_t max = 1; 
int sat_space = 0;

void consistency(){
printf("-----CONSISTENCY-----\n");
    char* first = (char*)malloc(5); 
    first[0] = 'f';
    first[1] = 'i';
    first[2] = 'v';
    first[3] = 'e';
    first[4] = '\0';
    free(first); 
    char* second = (char*)malloc(5);
    printf("first: %p, second %p\n", first, second); 
    if(first == second){
        printf("consistency test: success\n"); 
    }
    else{
        printf("consistency test: failed\n");
    }
}

void maximization(){
printf("-----MAXIMIZATION-----\n");
    size_t size = 1; 
    char* allocate = (char*) malloc(size); 
    while(allocate != NULL){
        free(allocate); 
        size = size * 2; 
        allocate = (char*) malloc(size); 
    }
    while(allocate == NULL){
        size = size / 2; 
        allocate = (char*) malloc(size); 
    }
    free(allocate);
    printf("maximization test: max is %lu\n", size); 
    max = size; 
}

void basic_coalescence(){
printf("-----BASIC COALESCENCE-----\n");
    char* half_alloc = (char*) malloc(max / 2); 
    char* quarter_alloc = (char*) malloc(max / 4); 
    free(half_alloc); 
    free(quarter_alloc); 
    char* max_alloc = (char*) malloc(max); 
    if(max_alloc != NULL){
        printf("basic coalescence test: success\n");
        free(max_alloc); 
    }
    else{
        printf("basic coalescence test: failed\n");
    }
}

void saturation(){
printf("-----SATURATION-----\n");
    for(int i = 0; i < 9216; i++){
        sat_arr[i] = (char*)malloc(1024); 
    }
    int j = 9216;
    while(sat_arr[j - 1] != NULL){
        sat_arr[j] = (char*)malloc(1); 
        j++;
    }
    sat_space = j - 2;
    printf("saturation test: space saturated\n");
}

void time_overhead(){
printf("-----TIME OVERHEAD-----\n");
    free(sat_arr[sat_space]); 
    clock_t start_t, end_t;
    start_t = clock();
    sat_arr[sat_space] = (char*)malloc(1); 
    end_t = clock(); 

    double total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;

    printf("time overhead test: elapsed time: %f\n", total_t); 
}

void intermediate_coalescence(){
printf("-----INTERMEDIATE COALESCENCE-----\n");
    for(int i = 0; i < sat_space; i++){
        free(sat_arr[i]); 
    }
    char* max_alloc = malloc(max); 
    if(max_alloc != NULL){
        printf("intermediate coalescence test: success\n"); 
        free(max_alloc); 
    }
    else{
        printf("intermediate coalescence test: failed\n"); 
    }
}

int main(){
    consistency();
    maximization();
    basic_coalescence();
    saturation();
    time_overhead();
    intermediate_coalescence();

    return 0;
}

// #include <stdlib.h>
// #include <stdio.h>
// #include <time.h>
// #include "umalloc.h"

// int main(int argc, char **argv) {

//     double test1, test2, test3, test4, test5 = 0;

//     for (int iter = 0; iter < 50; iter++) {

//         // test 1: malloc and then immediately free the ptr
//         clock_t start1 = clock();

//         for (int i = 0; i < 120; i++) {
//             char *ptr = malloc(1);
//             free(ptr);
//         }

//         clock_t end1 = clock();
//         test1 += (double)(end1 - start1) / CLOCKS_PER_SEC;
        
//         // test 2: malloc and then free sequentially 
//         clock_t start2 = clock();

//         char *arr[120];
//         for (int i = 0; i < 120; i++) {
//             arr[i] = malloc(1);
//         }

//         for (int i = 0; i < 120; i++) {
//             free(arr[i]);
//         }

//         clock_t end2 = clock();
//         test2 += (double)(end2 - start2) / CLOCKS_PER_SEC;

//         // test 3: 
//         clock_t start3 = clock();

//         int mallocs = 0;
//         int inArray = 0;
//         char *arr2[120];

//         while (mallocs != 120) {
//             int randNum = rand() % 2;
//             if (randNum == 0) {
//                 arr2[inArray] = malloc(1);
//                 mallocs++;
//                 inArray++;
//             } else if (randNum == 1 && inArray != 0) {
//                 free(arr2[inArray - 1]);
//                 inArray--;
//             }
//         }

//         while (inArray != 0) {
//             free(arr2[inArray - 1]);
//             inArray--;
//         }

//         clock_t end3 = clock();
//         test3 += (double)(end3 - start3) / CLOCKS_PER_SEC;

//         // test 4: testing the ability to split chunks
//         clock_t start4 = clock();

//         int fraction = 2;
//         do {
//             char *ptr = malloc(4096 / fraction);
//             free(ptr);
//             fraction *= 2;
//         } while (fraction <= 4096);

//         clock_t end4 = clock();
//         test4 += (double)(end4 - start4) / CLOCKS_PER_SEC;

//         // test 5: malloc and then freeing backwards
//         clock_t start5 = clock();

//         char *arr3[120];
//         for (int i = 0; i < 120; i++) {
//             arr3[i] = malloc(1);
//         }

//         for (int i = 119; i >= 0; i--) {
//             free(arr3[i]);
//         }

//         clock_t end5 = clock();
//         test5 += (double)(end5 - start5) / CLOCKS_PER_SEC;
//     }

//     printf("Test 1 Average Time: %f\n", test1 / 50);
//     printf("Test 2 Average Time: %f\n", test2 / 50);
//     printf("Test 3 Average Time: %f\n", test3 / 50);
//     printf("Test 4 Average Time: %f\n", test4 / 50);
//     printf("Test 5 Average Time: %f\n", test5 / 50);
    
//     int* test = (int*)malloc(sizeof(int));
//     char* str = (char*)malloc(1000000);
//     free(test);
//     free(str);
//     free(str);

//     return 0;
// }
