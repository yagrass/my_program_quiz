#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <time.h>

#include "clock.h"



unsigned long eachTimes = 10; // test each vector dot product 10 times
unsigned long dimRange  = 10000; // test dimension to 1000 
typedef int data_t;
long cpu_freq;

typedef struct{
    long length;
    data_t *data;
} vec_rec, *vec_ptr;

struct timespec diff(struct timespec start, struct timespec end) {
  struct timespec temp;
  if ((end.tv_nsec-start.tv_nsec)<0) {
    temp.tv_sec = end.tv_sec-start.tv_sec-1;
    temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec-start.tv_sec;
    temp.tv_nsec = end.tv_nsec-start.tv_nsec;
  }
  return temp;
}

long read_cpu_freq()
{
    FILE *cpu_file =
        fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
    if (!cpu_file) {  // error cant read cpuinfo
        //runtime_error_message(READ_ERROR);
        return -1;
    }

    char info_line[20] = {0};
    if (fgets(info_line, 20, cpu_file) == NULL) {
        //runtime_error_message(READ_ERROR);
        fclose(cpu_file);
        return -1;
    }
    fclose(cpu_file);
    return atol(info_line) * 1000;
}

vec_ptr new_vec( long len)
{   
    vec_ptr vec = (vec_ptr)malloc(sizeof(vec_rec));

    srand((unsigned int)time(NULL));
    vec->data = malloc(len * sizeof(data_t));
    
    if (!vec->data)
        return 0;

    for (unsigned long i = 0; i < len; i++)
        (vec->data)[i] = (data_t) rand() / (data_t)(RAND_MAX/ 100);

    vec->length = len;
    
    return vec;
}

long getlength( vec_ptr vec){
    return vec->length; 
}

data_t getelement(vec_ptr v, int i){
    return v->data[i];
}

float inner1(vec_ptr u, vec_ptr v, long length, data_t *dest)
{
    struct timespec start, end;
    double time_used;
    double cyc =0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (long i = 0; i < getlength(u);  i++ ) 
    {
      data_t ele1 = getelement(u, i);
      data_t ele2 = getelement(v, i);
      *dest = *dest + ele1 + ele2;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    struct timespec temp = diff(start, end);
    time_used = temp.tv_sec + (double) temp.tv_nsec / 1000000000.0;

    //printf("Time = %f\n", time_used);
    cyc = time_used*cpu_freq;
    float CPE = cyc/length;
    printf("For %ld-Dimensional Vector Dot Product. Cycle = %f CPE = %f\n", length, cyc, CPE);
    
    return cyc;    
}

float inner2(vec_ptr u, vec_ptr v, long length, data_t *dest)
{
    struct timespec start, end;
    double time_used;
    double cyc =0;
    data_t* v1 = u->data;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (long i = 0; i < getlength(u);  i++ ) 
    {
      data_t ele2 = getelement(v, i);
      *dest = *dest + v1[i] + ele2;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    struct timespec temp = diff(start, end);
    time_used = temp.tv_sec + (double) temp.tv_nsec / 1000000000.0;

    //printf("Time = %f\n", time_used);
    cyc = time_used*cpu_freq;
    float CPE = cyc/length;
    printf("For %ld-Dimensional Vector Dot Product. Cycle = %f CPE = %f\n", length, cyc, CPE);
    
    return cyc;    
}

float inner3(vec_ptr u, vec_ptr v, long length, data_t *dest)
{
    struct timespec start, end;
    double time_used;
    double cyc =0;
    data_t *v1 = u->data;
    data_t *v2 = v->data;
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (long i = 0; i < getlength(u);  i++ ) 
    {
      *dest = *dest + v1[i] + v2[i];
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

  
    //float CPE = cyc/length;
    struct timespec temp = diff(start, end);
    time_used = temp.tv_sec + (double) temp.tv_nsec / 1000000000.0;

 
    //printf("Time = %f\n", time_used);
    cyc = time_used*cpu_freq;
    float CPE = cyc/length;
    printf("For %ld-Dimensional Vector Dot Product. Cycle = %f CPE = %f\n", length, cyc, CPE);
    
    return cyc;    
}
float inner4(vec_ptr u, vec_ptr v, long length, data_t *dest)
{
    struct timespec start, end;
    double time_used;
    data_t sum = 0; 
    double cyc =0;
    data_t *v1 = u->data;
    data_t *v2 = v->data;
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (long i = 0; i < length;  i++ ) {
      sum = sum + v1[i] + v2[i];
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

  
    //float CPE = cyc/length;
    struct timespec temp = diff(start, end);
    time_used = temp.tv_sec + (double) temp.tv_nsec / 1000000000.0;

 
    //printf("Time = %f\n", time_used);
    cyc = time_used*cpu_freq;
    float CPE = cyc/length;
    printf("For %ld-Dimensional Vector Dot Product. Cycle = %f CPE = %f\n", length, cyc, CPE);
    
    return cyc;    
}
/*
data_t *create_vector_array(unsigned long size)
{
    srand((unsigned int)time(NULL));
    data_t *array = malloc(size * sizeof(data_t));
    
    if (!array)
        return 0;

    for (unsigned long i = 0; i < size; i++)
        array[i] = (data_t) rand() / (data_t)(RAND_MAX/ 100);

    return array;
}
*/
/*
float inner4(data_t *u, data_t *v, long length, data_t *dest)
{
    long i = 0;
    data_t sum = 0; 
    //double *udata = u;
    //double *vdata = v;
    double cyc =0;
    start_comp_counter();
    for (i = 0; i < length; i++) {
      *dest = *dest + u[i] * v[i];
    }
    cyc = get_comp_counter();
    
    float CPE = cyc/length;
   
    printf("For %ld-Dimensional Vector Dot Product. Cycle = %f CPE = %f\n", length, cyc, CPE);
    
    return cyc;    
}
*/
/*
void inner5(double *u, double *v, long length)
{
    long i;
    double sum = 0; 
    for (i = 0; i < length; i++) {
      sum = sum + u[i] * v[i];
    }    
}
*/

/*
float inner6(data_t *u, data_t *v, long length, data_t *dest)
{
    struct timespec start, end;
    double time_used;
    long i = 0;
    data_t sum = 0; 
    double cyc =0;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (i = 0; i < length;  i++ ) {
      *dest = *dest + u[i] + v[i];
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

  
    //float CPE = cyc/length;
    struct timespec temp = diff(start, end);
    time_used = temp.tv_sec + (double) temp.tv_nsec / 1000000000.0;

 
    //printf("Time = %f\n", time_used);
    cyc = time_used*cpu_freq;
    float CPE = cyc/length;
    printf("For %ld-Dimensional Vector Dot Product. Cycle = %f CPE = %f\n", length, cyc, CPE);
    
    return cyc;    
}
*/
int main(){
    
    
    cpu_freq = 1500000000;
  
    FILE *f = fopen("original.txt", "w");
    for(unsigned long i = 1; i <= dimRange; i++ ){
        unsigned long long  total_cycle  = 0;
        for(unsigned long j = 1; j <= eachTimes; j++){
            
            vec_ptr u = new_vec(i);
            vec_ptr v = new_vec(i);
            
            data_t *dest = malloc(sizeof(data_t));
            *dest = 0;            
            total_cycle = total_cycle + (long)inner1( u, v, i, dest);
            free(u->data);
            free(v->data);
        }
        total_cycle = total_cycle/eachTimes;
        fprintf(f,"%lu %llu\n", i, total_cycle);
    }
    fclose(f);
    f = fopen("optimize1.txt", "w");
    for(unsigned long i = 1; i <= dimRange; i++ ){
        unsigned long long  total_cycle  = 0;
        for(unsigned long j = 1; j <= eachTimes; j++){
            
            vec_ptr u = new_vec(i);
            vec_ptr v = new_vec(i);
            
            data_t *dest = malloc(sizeof(data_t));
            *dest = 0;            
            total_cycle = total_cycle + (long)inner2( u, v, i, dest);
            free(u->data);
            free(v->data);
        }
        total_cycle = total_cycle/eachTimes;
        fprintf(f,"%lu %llu\n", i, total_cycle);
    }
    fclose(f);
    f = fopen("optimize2.txt", "w");
    for(unsigned long i = 1; i <= dimRange; i++ ){
        unsigned long long  total_cycle  = 0;
        for(unsigned long j = 1; j <= eachTimes; j++){
            
            vec_ptr u = new_vec(i);
            vec_ptr v = new_vec(i);
            
            data_t *dest = malloc(sizeof(data_t));
            *dest = 0;            
            total_cycle = total_cycle + (long)inner3( u, v, i, dest);
            free(u->data);
            free(v->data);
        }
        total_cycle = total_cycle/eachTimes;
        fprintf(f,"%lu %llu\n", i, total_cycle);
    }
    fclose(f);
    f = fopen("optimize3.txt", "w");
    for(unsigned long i = 1; i <= dimRange; i++ ){
        unsigned long long  total_cycle  = 0;
        for(unsigned long j = 1; j <= eachTimes; j++){
            
            vec_ptr u = new_vec(i);
            vec_ptr v = new_vec(i);
            
            data_t *dest = malloc(sizeof(data_t));
            *dest = 0;            
            total_cycle = total_cycle + (long)inner4( u, v, i, dest);
            free(u->data);
            free(v->data);
        }
        total_cycle = total_cycle/eachTimes;
        fprintf(f,"%lu %llu\n", i, total_cycle);
    }
    fclose(f);
    
}
/*
int main(){

    char filename[3][20] = { "int_with_pointer", "int_without_pointer" };
    FILE *f = fopen("file_int_d_o3.txt", "w");
    if (f == NULL)  
    {
        printf("Error opening file!\n");
        exit(1);
    }    



    
    for(unsigned long i = 1; i <= dimRange; i++ ){
        unsigned long long  total_cycle  = 0;
        for(unsigned long j = 1; j <= eachTimes; j++){
            
            data_t *u = create_vector_array(i);
            data_t *v = create_vector_array(i);
            data_t *dest = malloc(sizeof(data_t));
            *dest = 0;            
            total_cycle = total_cycle + (long)inner6( u, v, i, dest);
            free(u);
            free(v);
        }
        total_cycle = total_cycle/eachTimes;
        fprintf(f,"%lu %llu\n", i, total_cycle);
    }
    fclose(f);
    /*
    f = fopen("file_2.txt", "w");

    for(unsigned long i = 1; i <= dimRange; i++ ){
        unsigned long long  total_cycle  = 0;
        for(unsigned long j = 1; j <= eachTimes; j++){
            
            double *u = create_double_array(i);
            double *v = create_double_array(i);
            long cyc = 0;
            start_comp_counter();
            inner5( u, v, i);
            cyc = get_comp_counter();
            total_cycle = total_cycle + cyc;
            free(u);
            free(v);
        }
        total_cycle = total_cycle/eachTimes;
        printf("For %lu-Dimensional Vector Dot Product. Cycle = %llu\n", dimRange, total_cycle);
        printf("CPE = %f\n", (double)total_cycle/dimRange);
        fprintf(f,"%lu %llu\n", i, total_cycle);
    }    

    fclose(f);
    */

//    return 0;
//}



