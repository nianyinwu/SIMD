#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define height 17280
#define width  30720

struct timespec diff(struct timespec start, struct timespec end){		/*Counting time function */
	struct timespec temp;
	if((end.tv_nsec-start.tv_nsec<0)){
		temp.tv_sec=end.tv_sec-start.tv_sec-1;
		temp.tv_nsec=1000000000+end.tv_nsec-start.tv_nsec;
	}
	else{
		temp.tv_sec=end.tv_sec-start.tv_sec;
		temp.tv_nsec=end.tv_nsec-start.tv_nsec;
	}
	return  temp;
}

int main(void){
	struct timespec start, end;
	void *ptr;
	int *pBitmap, row, col;
	int pixel, red, green, blue, alpha, bw;

	pBitmap = (int*) malloc(height * width * sizeof(int));

	for (row=0; row<height; row++) {
		for (col=0; col<width; col++) {
			pBitmap[col+row*width] = rand();
		}
	}

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (row=0; row<height; row++) {
		for (col=0; col<width; col++) {
			pixel = pBitmap[col+row*width];
			//printf("%d\n", pixel);
			alpha = (pixel >> 24) & 0xff;
			red = (pixel >> 16) & 0xff;
			green = (pixel >> 8) & 0xff;
			blue = pixel & 0xff;
			bw = (int) (red*0.299 + green*0.587 + blue*0.114);
			pBitmap[col+row*width] = (alpha<<24) + (bw<<16) + (bw<<8) + (bw);
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &end);

	
	printf("Execution time = %ld : %ld\n", diff(start, end).tv_sec, diff(start, end).tv_nsec); 
	return 0;
}