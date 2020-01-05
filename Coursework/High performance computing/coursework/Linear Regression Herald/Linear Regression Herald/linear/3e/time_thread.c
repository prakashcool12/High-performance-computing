#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

/******************************************************************************
 * This program takes an initial estimate of m and c and finds the associated 
 * rms error. It is then as a base to generate and evaluate 8 new estimates, 
 * which are steps in different directions in m-c space. The best estimate is 
 * then used as the base for another iteration of "generate and evaluate". This 
 * continues until none of the new estimates are better than the base. This is
 * a gradient search for a minimum in mc-space.
 * 
 * To compile:
 *   cc -o time_thread time_thread.c -lm -pthread
 * 
 * To run:
 *   ./linear
 * 
 * Dr Kevan Buckley, University of Wolverhampton, 2018
 *****************************************************************************/
int i;
  double bm = 1.3;
  double bc = 10;
  double be;
  double dm[8];
  double dc[8];
  double e[8];
  double step = 0.01;
  double best_error = 999999999;
  int best_error_i;
  int minimum_found = 0;
  double om[] = {0,1,1, 1, 0,-1,-1,-1};
  double oc[] = {1,1,0,-1,-1,-1, 0, 1};

typedef struct point_t {
  double x;
  double y;
} point_t;

int n_data = 1000;
point_t data[];

double residual_error(double x, double y, double m, double c) {
  double e = (m * x) + c - y;
  return e * e;
}

double rms_error(double m, double c) {
  int i;
  double mean;
  double error_sum = 0;
  
  for(i=0; i<n_data; i++) {
    error_sum += residual_error(data[i].x, data[i].y, m, c);  
  }
  
  mean = error_sum / n_data;
  
  return sqrt(mean);
}
int time_difference(struct timespec *start, struct timespec *finish, long long int *difference)
{
  long long int ds =  finish->tv_sec - start->tv_sec; 
  long long int dn =  finish->tv_nsec - start->tv_nsec; 

  if(dn < 0 ) {
    ds--;
    dn += 1000000000; 
  } 
  *difference = ds * 1000000000 + dn;
  return !(*difference > 0);
}

void *linear_regression_thread(void *args){

  int *a = args;
  int i = *a;

  dm[i] = bm +(om[i] * step);
      dc[i] = bc + (oc[i] * step);
  e[i] = rms_error(dm[i], dc[i]);
  if(e[i] < best_error) {
  best_error = e[i];
  best_error_i = i;
  pthread_exit(NULL);
  }
}  

int main() {
   struct timespec start, finish;   
  long long int time_elapsed;
  clock_gettime(CLOCK_MONOTONIC, &start);

  int i;
  pthread_t p_threads[8];
  
  be = rms_error(bm, bc);

  while(!minimum_found) {
    for(i=0;i<8;i++) {
      pthread_create(&p_threads[i], NULL, (void*)linear_regression_thread, &i);
      pthread_join(p_threads[i], NULL); 
    }

     //printf("best m,c is %lf,%lf with error %lf in direction %d\n", 
       //dm[best_error_i], dc[best_error_i], best_error, best_error_i);
    if(best_error < be) {
      be = best_error;
      bm = dm[best_error_i];
      bc = dc[best_error_i];
    } else {
      minimum_found = 1;
    }
  }
  printf("minimum m,c is %lf,%lf with error %lf\n", bm, bc, be);
  clock_gettime(CLOCK_MONOTONIC, &finish);
  time_difference(&start, &finish, &time_elapsed);
  printf("Time elapsed was %lldns or %0.9lfs\n", time_elapsed, 
         (time_elapsed/1.0e9)); 
return 0;
}

point_t data[] = {
  {67.19,104.66},{72.89,121.14},{82.83,124.39},{73.79,116.21},
  {83.98,136.57},{84.59,138.52},{79.27,141.21},{80.50,135.14},
  {72.76,118.70},{69.16,123.21},{82.30,127.32},{72.34,125.25},
  {73.55,115.21},{69.06,118.19},{87.04,115.91},{86.93,132.84},
  {46.19,106.60},{35.39,56.66},{79.49,124.10},{59.38,92.34},
  {36.08,75.58},{52.35,103.11},{78.02,114.59},{85.91,139.59},
  {36.46,69.61},{32.67,72.07},{24.84,45.23},{57.76,95.53},
  {64.32,107.47},{29.90,65.85},{ 1.50,12.22},{28.53,54.79},
  { 9.48,30.40},{35.85,76.52},{47.49,91.64},{57.60,100.37},
  {29.98,64.11},{46.84,74.47},{98.64,161.45},{85.42,123.40},
  { 7.21,46.32},{63.13,113.07},{72.11,108.56},{76.62,141.24},
  {89.33,120.36},{ 1.47,39.67},{52.21,86.30},{44.92,77.81},
  {79.95,113.75},{42.44,59.36},{17.89,68.00},{31.01,50.83},
  {70.51,131.95},{ 6.44,33.61},{ 2.18,48.83},{ 3.35,23.31},
  {49.25,108.10},{ 2.72,40.65},{16.81,40.78},{49.41,76.88},
  {18.99,65.33},{61.94,102.70},{77.98,128.80},{50.39,97.19},
  {50.78,84.26},{90.37,136.16},{37.98,70.20},{71.89,123.72},
  {16.62,60.50},{30.74,53.36},{ 3.30,31.47},{61.48,102.85},
  {65.53,98.08},{92.83,150.97},{48.34,75.30},{84.00,116.13},
  {99.10,152.18},{25.99,77.22},{50.83,85.65},{66.83,107.32},
  {20.67,53.60},{16.25,60.95},{76.90,120.32},{80.23,129.58},
  {20.07,65.06},{75.33,117.53},{ 0.14,48.70},{59.56,86.35},
  {23.36,74.97},{14.15,39.00},{78.69,139.61},{76.39,127.02},
  {61.37,112.82},{25.11,72.12},{28.51,73.68},{54.24,104.99},
  {42.20,79.72},{19.36,45.72},{73.12,127.09},{30.29,87.89},
  {29.46,72.23},{59.04,84.26},{97.21,150.43},{ 7.36,33.79},
  {57.48,115.64},{13.70,53.69},{72.89,107.82},{90.34,158.63},
  {95.68,137.73},{39.57,81.66},{74.73,114.48},{63.89,113.98},
  {24.58,66.10},{36.55,80.70},{44.48,78.22},{74.55,127.55},
  {43.80,84.36},{77.94,140.49},{28.97,85.98},{ 4.82,32.59},
  {79.32,119.47},{38.28,71.13},{61.12,104.94},{18.96,35.31},
  { 7.79,56.10},{80.24,122.09},{85.39,122.54},{70.41,99.74},
  { 6.21,52.45},{29.80,60.44},{27.53,63.14},{54.26,103.61},
  {71.30,129.89},{30.07,76.40},{45.50,73.30},{86.34,143.83},
  {98.83,154.14},{15.06,47.67},{23.84,63.18},{15.18,62.64},
  {46.74,102.88},{67.09,103.26},{40.46,76.71},{25.96,66.34},
  {93.67,147.61},{61.43,126.56},{60.94,105.85},{59.77,112.44},
  {12.91,31.62},{96.10,146.72},{62.75,119.48},{52.58,90.15},
  {94.46,128.80},{81.28,132.48},{48.48,83.99},{41.80,88.11},
  {68.26,107.25},{84.55,146.08},{86.84,122.24},{56.25,95.90},
  {79.45,131.89},{ 5.48,35.88},{39.59,69.91},{72.20,130.38},
  {56.18,99.33},{41.37,73.46},{ 0.90,21.86},{47.99,98.07},
  {57.41,96.04},{31.71,67.94},{ 8.12,30.00},{95.71,150.37},
  {23.33,51.91},{51.03,91.93},{86.38,138.14},{98.98,151.42},
  {72.82,127.68},{88.78,144.80},{44.60,94.70},{42.89,64.91},
  {94.38,144.68},{72.59,121.59},{47.23,71.77},{94.34,152.43},
  {53.46,77.92},{44.96,85.18},{98.37,148.99},{86.53,139.92},
  {29.02,51.30},{34.13,77.57},{53.43,92.45},{69.61,125.56},
  {94.60,155.89},{17.62,32.19},{51.05,82.16},{22.48,54.80},
  {59.74,77.67},{33.40,76.61},{ 7.79,25.43},{62.50,107.56},
  {32.51,51.40},{63.14,99.93},{73.24,118.89},{99.36,155.70},
  {81.55,157.57},{58.25,93.17},{ 5.72,25.90},{67.66,127.32},
  {89.49,122.30},{ 0.71,43.64},{62.26,109.41},{ 3.52,15.18},
  {42.31,90.88},{50.88,74.92},{47.62,88.18},{70.14,109.21},
  {33.58,76.90},{70.74,111.97},{47.75,96.42},{71.89,125.38},
  {96.41,138.62},{82.50,128.78},{60.44,98.90},{46.29,105.41},
  {32.46,64.48},{16.42,55.47},{69.73,101.62},{98.08,143.94},
  { 1.94,40.87},{ 1.74,40.30},{96.26,153.23},{56.82,86.32},
  {37.34,66.44},{42.63,78.72},{48.52,83.15},{52.61,98.10},
  {10.56,43.45},{83.14,121.62},{23.07,53.91},{15.77,34.47},
  {74.13,117.54},{86.85,155.04},{68.96,114.33},{27.48,42.20},
  {84.05,152.29},{25.94,84.00},{60.17,113.17},{ 3.88,26.61},
  {11.06,28.97},{23.09,38.74},{73.18,110.82},{78.66,117.22},
  {50.79,90.06},{42.41,97.56},{77.27,131.89},{30.37,75.15},
  { 5.03,42.74},{92.51,147.53},{ 4.93,48.04},{80.96,126.51},
  {25.72,62.03},{85.16,121.81},{95.80,154.32},{98.97,160.01},
  {52.37,104.34},{90.80,143.96},{86.68,134.27},{52.73,83.85},
  {37.79,69.29},{15.96,46.42},{71.34,141.04},{ 7.47,43.43},
  {17.53,47.91},{33.96,67.92},{84.42,128.50},{75.49,130.59},
  {85.53,139.67},{ 3.45,27.76},{18.51,47.07},{53.74,91.96},
  {35.67,69.25},{ 9.79,31.41},{47.47,94.04},{98.65,149.80},
  {73.67,102.11},{89.80,138.97},{71.39,121.75},{62.22,97.85},
  {63.07,101.27},{42.14,88.86},{23.22,43.77},{78.51,121.66},
  {35.55,72.16},{ 4.70,40.49},{ 5.26,28.55},{75.00,126.33},
  { 9.33,42.57},{31.84,58.82},{59.99,114.72},{ 0.21,29.83},
  {62.74,111.33},{10.70,37.50},{52.40,91.11},{98.54,158.18},
  {95.37,142.99},{11.04,46.20},{86.32,127.02},{32.94,72.07},
  {61.57,115.00},{ 2.57,29.99},{57.88,95.98},{86.73,122.22},
  {53.48,81.02},{75.58,123.33},{ 4.41,30.28},{25.45,58.99},
  {60.18,102.74},{75.55,125.34},{50.24,83.17},{23.85,55.88},
  {27.15,69.02},{32.90,71.12},{15.02,63.79},{ 8.96,29.75},
  {64.71,107.34},{ 9.08,44.66},{54.47,106.34},{35.11,77.69},
  {43.81,80.41},{77.77,127.21},{89.22,125.19},{ 4.53,36.53},
  {83.40,130.39},{18.41,48.13},{32.60,74.55},{76.98,115.24},
  {77.28,111.27},{70.90,110.77},{96.25,156.92},{56.14,101.68},
  {27.30,64.73},{34.96,65.94},{81.10,128.41},{49.52,91.12},
  {59.85,94.47},{38.41,74.08},{17.41,67.38},{91.27,150.83},
  {75.43,113.39},{48.70,89.05},{95.42,136.75},{41.34,98.93},
  {96.94,144.35},{ 8.50,17.14},{44.88,83.79},{71.81,106.24},
  {77.19,131.00},{97.90,125.10},{15.28,54.11},{25.30,75.86},
  {82.88,129.83},{82.79,144.85},{77.06,110.87},{23.86,60.18},
  {64.13,119.33},{79.82,131.84},{32.48,72.67},{86.16,132.24},
  {47.85,90.99},{89.61,142.84},{94.51,138.88},{72.46,93.37},
  {52.01,98.12},{79.40,121.31},{34.14,93.95},{35.98,80.40},
  {67.47,99.47},{91.58,127.26},{37.50,87.23},{49.16,84.66},
  {71.41,99.07},{80.44,128.42},{84.10,136.76},{63.43,104.34},
  {90.73,146.37},{33.50,76.88},{32.84,57.94},{57.88,99.28},
  { 2.81,40.39},{ 6.89,51.21},{46.63,75.65},{56.27,102.12},
  {88.55,142.51},{ 6.04,15.33},{52.16,107.27},{63.22,113.58},
  {10.39,24.92},{96.48,147.58},{60.50,79.13},{51.60,82.21},
  {56.67,76.65},{75.32,129.82},{17.73,54.39},{ 1.36,20.19},
  {88.93,132.23},{49.17,101.94},{39.79,77.17},{54.49,92.60},
  {66.44,89.96},{30.59,63.91},{93.44,146.10},{18.02,48.36},
  {69.85,129.26},{76.63,126.72},{87.29,123.03},{49.28,95.44},
  {37.20,90.91},{53.26,76.61},{36.99,81.16},{33.34,60.14},
  {12.08,49.12},{19.03,58.56},{81.17,124.43},{64.36,109.88},
  {31.04,65.78},{24.95,60.98},{86.29,138.68},{32.91,78.66},
  {44.16,67.48},{65.24,89.22},{93.30,157.48},{44.62,88.40},
  {78.14,119.84},{74.50,117.39},{79.33,124.36},{49.25,98.90},
  {20.74,44.61},{ 2.78,46.50},{23.45,63.89},{52.39,92.78},
  {89.87,126.70},{36.85,73.15},{22.19,63.45},{42.53,82.87},
  {10.41,46.10},{22.27,75.06},{40.29,69.14},{67.87,112.06},
  {89.49,127.36},{53.17,87.01},{65.77,99.79},{39.07,78.16},
  {73.49,111.43},{52.45,113.39},{70.18,116.01},{95.25,142.03},
  {74.89,124.03},{50.60,102.46},{13.31,45.95},{73.13,102.59},
  {92.22,152.40},{10.27,32.34},{91.84,135.75},{45.72,92.86},
  {22.80,65.52},{42.07,85.38},{33.76,62.74},{81.06,124.77},
  { 7.76,39.94},{27.82,87.05},{95.16,130.21},{23.94,64.37},
  {20.22,54.60},{86.61,145.52},{ 7.50,36.61},{37.38,68.68},
  {60.39,116.74},{ 9.44,38.65},{79.14,124.14},{ 5.84,35.86},
  {66.97,113.30},{ 7.02,18.24},{36.18,85.24},{11.58,41.96},
  {40.98,79.78},{88.87,139.35},{33.32,72.01},{ 2.21,47.71},
  {64.80,118.18},{32.59,86.54},{28.40,47.48},{83.42,135.46},
  {79.05,119.24},{90.47,134.34},{33.75,68.59},{18.78,59.23},
  {91.34,118.19},{34.06,70.07},{42.68,108.22},{ 0.70,21.95},
  { 1.34,39.31},{49.96,97.50},{47.77,83.82},{27.88,64.44},
  {85.44,126.90},{32.25,57.69},{16.33,47.97},{ 6.86,40.84},
  {14.06,49.72},{97.45,168.23},{ 6.41,41.17},{99.21,145.59},
  {98.09,146.72},{11.94,46.87},{ 5.20,37.98},{32.81,68.84},
  {66.26,133.04},{38.69,99.75},{91.56,138.07},{ 0.11,34.73},
  {36.63,73.83},{80.89,129.13},{52.73,103.04},{12.62,60.72},
  {61.49,115.30},{95.26,142.25},{30.35,58.34},{41.75,84.36},
  {52.04,78.52},{57.89,105.94},{85.24,143.33},{10.89,55.26},
  {74.36,115.86},{59.44,119.89},{ 7.89,14.68},{50.98,79.27},
  {92.51,157.83},{98.14,149.79},{85.81,112.23},{31.25,67.25},
  {51.72,93.44},{66.07,92.48},{92.46,144.49},{53.98,92.83},
  {78.79,122.58},{79.55,127.89},{32.24,71.90},{ 4.96,28.50},
  {17.56,49.48},{76.19,110.12},{77.82,156.21},{12.58,59.01},
  {16.52,51.79},{75.27,125.72},{26.82,58.47},{22.58,55.91},
  {20.10,46.19},{24.17,44.40},{20.45,53.12},{45.77,85.90},
  { 3.13,27.32},{ 0.98,24.53},{31.19,63.03},{57.18,102.49},
  {17.34,70.02},{84.72,118.52},{55.77,87.14},{ 4.49,46.25},
  {54.65,104.59},{48.53,100.72},{ 7.23,29.72},{ 0.76,30.70},
  {28.74,69.30},{47.84,92.40},{50.68,83.84},{85.87,140.12},
  { 3.65,52.63},{79.29,125.28},{41.90,96.23},{14.98,52.98},
  {40.98,73.19},{97.58,142.61},{ 5.97,40.24},{34.11,77.01},
  {41.60,53.70},{22.46,88.60},{28.11,53.52},{ 6.86,41.38},
  { 1.06,34.11},{ 5.96,40.60},{12.32,46.48},{21.67,57.62},
  {18.11,71.01},{96.72,158.08},{23.04,70.73},{61.14,125.35},
  {24.46,67.41},{50.77,96.50},{43.14,80.23},{49.94,91.99},
  {92.83,157.97},{55.72,94.86},{ 4.69,37.22},{60.17,105.06},
  {74.26,107.58},{38.59,74.53},{51.55,90.42},{ 4.99,37.29},
  {27.86,51.73},{ 5.73,41.43},{72.19,119.56},{59.70,95.08},
  {32.05,73.36},{91.38,134.64},{18.36,56.88},{17.73,53.85},
  { 6.68,47.11},{10.90,42.69},{72.75,109.33},{55.96,107.22},
  {81.09,124.83},{28.16,75.89},{17.68,36.70},{26.22,71.92},
  {63.31,124.78},{11.64,44.16},{67.79,118.63},{16.77,47.18},
  {83.83,121.77},{23.75,48.90},{93.73,134.92},{78.18,107.83},
  {87.37,155.09},{69.88,94.73},{88.97,141.17},{67.50,136.45},
  {48.68,74.99},{57.60,105.78},{ 7.10,29.29},{29.33,69.35},
  {48.26,79.77},{10.75,38.66},{27.71,68.36},{ 3.53,28.50},
  {11.15,62.35},{65.26,107.91},{27.04,59.47},{18.76,44.73},
  { 7.58,44.39},{58.81,94.25},{89.48,142.08},{76.52,113.90},
  {22.32,49.64},{92.00,134.72},{68.35,121.26},{69.25,123.50},
  {71.36,109.39},{96.39,141.96},{10.34,62.84},{10.87,50.97},
  {93.53,161.02},{ 0.15,22.94},{56.05,111.30},{ 4.40,33.63},
  {69.68,119.18},{81.79,125.23},{18.66,53.94},{87.37,125.35},
  {49.34,85.40},{63.48,126.19},{88.52,141.80},{33.86,66.76},
  {53.09,94.21},{67.34,125.15},{91.06,144.82},{76.94,112.90},
  {43.22,86.92},{87.52,140.61},{25.19,66.70},{41.47,77.90},
  {58.66,113.91},{63.65,88.15},{24.01,43.30},{79.42,129.14},
  {40.71,71.80},{22.02,70.29},{ 2.53,25.69},{23.06,77.91},
  {68.50,104.35},{60.89,100.65},{29.54,66.99},{76.52,133.25},
  {93.33,146.26},{61.61,113.95},{30.23,76.92},{ 4.09,34.02},
  {85.92,130.23},{46.97,97.23},{50.21,109.18},{56.83,109.65},
  {93.40,156.00},{25.41,48.65},{52.63,120.10},{28.80,79.10},
  {85.63,134.50},{43.44,97.22},{12.52,51.67},{90.73,137.29},
  {33.93,80.23},{92.63,148.70},{31.29,79.82},{17.49,52.68},
  {74.45,124.09},{10.63,41.36},{77.55,134.02},{54.95,106.39},
  {67.71,94.29},{95.51,134.67},{23.13,64.15},{57.82,106.32},
  {13.47,30.88},{84.85,134.50},{53.12,94.03},{25.13,57.36},
  {24.48,57.46},{20.54,57.55},{46.16,71.71},{65.75,108.86},
  {99.32,139.67},{18.49,57.05},{42.19,72.68},{82.85,136.59},
  {60.91,105.59},{28.37,61.89},{83.54,138.28},{97.16,132.23},
  {65.11,111.43},{ 3.75,37.01},{35.19,72.96},{50.99,88.62},
  {12.90,52.30},{23.60,57.04},{67.79,122.22},{73.69,118.74},
  {98.32,150.06},{88.78,133.26},{ 6.96,40.01},{ 5.91,48.67},
  {31.10,56.71},{62.18,115.56},{56.34,107.16},{ 4.00,27.96},
  {69.29,108.13},{89.24,172.06},{ 9.19,31.53},{65.73,87.18},
  {74.90,131.05},{20.83,52.08},{90.67,144.97},{66.05,101.40},
  {49.23,105.57},{49.52,77.65},{24.39,66.43},{35.27,64.55},
  {69.05,123.32},{28.98,52.05},{79.76,123.98},{64.18,96.90},
  {83.70,132.92},{52.62,97.15},{79.28,130.99},{11.05,34.80},
  {41.44,83.13},{55.11,94.25},{34.41,55.74},{84.08,147.90},
  {37.61,87.21},{63.59,108.36},{33.44,62.99},{ 9.64,56.77},
  {50.31,76.83},{80.01,111.94},{54.74,106.02},{94.37,145.95},
  {63.57,106.39},{54.18,92.68},{91.95,137.30},{65.99,110.68},
  {13.11,47.07},{55.87,105.33},{58.38,101.65},{21.00,53.00},
  { 6.65,55.06},{58.80,89.44},{85.73,115.89},{ 1.05,27.23},
  { 7.06,36.71},{38.15,70.61},{ 1.18,54.00},{28.42,66.51},
  { 5.20,24.04},{93.88,150.64},{36.10,77.11},{ 5.93,37.34},
  {22.51,58.13},{29.01,58.98},{27.50,61.77},{ 1.62,27.68},
  {68.10,110.20},{35.33,78.72},{19.79,61.40},{98.01,145.76},
  {97.46,147.72},{11.41,58.44},{22.22,47.08},{19.41,60.75},
  {71.28,111.50},{75.34,139.30},{46.03,94.14},{92.85,150.21},
  { 9.44,49.37},{97.25,135.59},{37.14,78.58},{97.23,162.61},
  {64.82,109.31},{86.14,142.90},{45.76,80.96},{78.22,130.69},
  { 5.15,38.10},{ 7.17,48.12},{ 2.71,39.62},{58.30,93.81},
  {28.24,53.69},{56.86,101.25},{14.07,52.99},{64.67,116.05},
  {68.67,114.08},{34.04,77.22},{95.31,139.09},{87.59,129.76},
  {91.81,143.13},{13.94,49.39},{69.46,100.04},{32.55,57.85},
  {42.40,94.31},{70.22,132.55},{51.82,89.92},{ 0.62,22.66},
  {50.10,98.83},{50.78,104.68},{70.05,128.91},{31.28,63.96},
  {79.75,107.86},{35.98,72.83},{40.65,89.46},{41.04,88.60},
  {16.07,47.34},{21.93,73.06},{44.37,82.82},{15.93,38.66},
  {36.11,88.49},{75.89,132.87},{71.60,115.05},{ 4.23,32.77},
  {44.14,79.80},{25.86,75.12},{81.37,114.44},{28.86,52.53},
  {50.57,106.77},{89.83,138.74},{14.68,63.38},{ 4.73,43.82},
  { 5.83,41.32},{42.28,64.09},{13.79,52.50},{67.24,109.59},
  {80.66,122.44},{39.75,63.90},{95.20,147.40},{96.00,153.80},
  { 4.29,29.82},{75.11,126.80},{38.43,90.49},{ 1.05,38.93},
  {56.89,72.17},{88.44,113.46},{11.21,54.07},{94.82,125.76},
  { 9.13,52.00},{69.82,118.72},{83.18,120.48},{31.24,72.11},
  {86.49,132.30},{71.39,131.30},{81.54,140.45},{82.42,142.54},
  {61.46,125.00},{48.96,79.72},{28.30,56.04},{86.44,151.20},
  {10.35,42.07},{57.45,78.89},{18.86,58.16},{84.58,124.35},
  {48.56,80.30},{58.96,104.30},{81.18,125.45},{80.97,123.33},
  {48.10,77.47},{63.83,97.79},{20.71,59.76},{62.79,100.62},
  {74.90,141.69},{47.94,89.19},{ 0.55,34.24},{64.38,115.95},
  {24.75,65.03},{29.96,70.23},{29.32,57.53},{ 5.05,44.38},
  {83.39,124.61},{37.46,68.36},{ 7.84,57.33},{81.27,137.88},
  { 9.55,42.83},{22.21,53.94},{83.85,130.03},{90.36,133.75},
  {27.76,74.48},{95.77,138.26},{59.63,101.64},{50.28,93.36},
  {35.19,90.33},{92.43,149.66},{50.61,82.29},{43.13,81.14},
  {44.90,79.47},{35.24,57.01},{29.01,58.28},{58.99,86.60},
  {65.07,100.35},{99.72,130.62},{ 1.01,32.53},{53.96,65.03},
  {89.37,115.34},{51.08,68.26},{69.49,128.37},{92.51,136.00},
  {71.70,117.76},{ 3.75,40.75},{61.22,114.23},{ 0.51,61.15},
  {45.30,91.12},{65.98,118.31},{67.90,119.01},{10.27,51.20},
  {10.28,54.67},{14.55,55.01},{75.74,129.36},{24.85,53.84},
  {90.26,139.78},{68.49,119.24},{65.82,108.78},{83.87,135.04},
  {81.60,135.89},{31.93,70.13},{47.23,82.80},{91.79,131.07},
  { 0.47,42.48},{39.96,75.06},{29.44,73.99},{89.65,144.55},
  {62.13,101.07},{38.91,95.83},{94.61,142.23},{26.83,56.75},
  {23.93,58.82},{15.11,46.28},{28.00,85.81},{89.26,151.20},
  { 1.63,29.70},{93.42,161.81},{74.17,115.20},{89.94,136.40},
  {72.20,110.35},{29.02,55.46},{54.82,93.06},{33.66,82.49},
  {49.10,67.92},{19.98,47.55},{ 1.83,32.98},{36.35,66.55},
  {90.28,137.14},{ 1.33,33.71},{95.14,124.02},{93.26,151.03},
  {67.85,114.26},{37.91,52.48},{ 9.53,56.79},{25.85,54.92}
};
