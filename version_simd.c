#include <stdio.h>
#include <time.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <string.h>
#define height 17280
#define width  30720

struct timespec diff(struct timespec start, struct timespec end)
{   struct timespec temp;
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
	int row, col;
	__m128i bw __attribute__((aligned(16)));		
	__m128i alpha __attribute__((aligned(16))),
			red __attribute__((aligned(16))),
			green __attribute__((aligned(16))),
			blue __attribute__((aligned(16)));
	__m128 t __attribute__((aligned(16)));
	__m128 v __attribute__((aligned(16)))= _mm_set_ps(0.299f,0.299f,0.299f,0.299f);
	__m128 v1 __attribute__((aligned(16)))= _mm_set_ps(0.587f,0.587f,0.587f,0.587f);
	__m128 v2 __attribute__((aligned(16)))= _mm_set_ps(0.114f,0.114f,0.114f,0.114f);
	__m128i s __attribute__((aligned(16)))= _mm_set_epi32(0xff,0xff,0xff,0xff);
	/*Align data for good performance*/
	
	int *pBitmap ;
	void *ptr;	
	posix_memalign(&ptr, 16, height * width * sizeof(int));
	pBitmap = (int*) ptr;
	/*Align data for good performance*/
	
	for (row=0; row<height; row++) {
		for (col=0; col<width; col++) {
			pBitmap[col+row*width] = rand();
		}
	}
	
	__m128i *pBitmap128 = (__m128i *)pBitmap;	/*Let pBitmap be vectors each of them has four data*/
	int n = (height*width)/4 ;	/* groups of four, so totalnum/4 */
    clock_gettime(CLOCK_MONOTONIC, &start);		/*Record start time*/
	for(int i =0; i<n;i++){
			alpha = _mm_and_si128(_mm_srai_epi32(pBitmap128[i],24),s);	/*SIMD instruction alpha = (pBitmap128[i] shift right 24 bits) AND s(0XFF) */
			red = _mm_and_si128(_mm_srai_epi32(pBitmap128[i],16),s);	/*SIMD instruction red = (pBitmap128[i] shift right 16 bits) AND s(0XFF) */
			green = _mm_and_si128(_mm_srai_epi32(pBitmap128[i],8),s);	/*SIMD instruction green = (pBitmap128[i] shift right 8 bits) AND s(0XFF) */
			blue = _mm_and_si128(pBitmap128[i],s);	/*SIMD instruction blue = (pBitmap128[i]) AND s(0XFF) */
			t = _mm_add_ps(_mm_mul_ps(_mm_cvtepi32_ps(red),v),_mm_mul_ps(_mm_cvtepi32_ps(green),v1));
			/*SIMD instruction t = (red * 0.299) + (green*0.587) ; _mm_cvtepi32_ps : convert int to float */
			bw = _mm_cvttps_epi32(_mm_add_ps(t,_mm_mul_ps(_mm_cvtepi32_ps(blue),v2)));
			/*SIMD instruction bw = (int)(t + (blue*0.114)) ; _mm_cvtps_epi32 : convert float to int */			
			pBitmap128[i] = _mm_add_epi32(_mm_add_epi32(_mm_slli_epi32(alpha,24),_mm_slli_epi32(bw,16)), _mm_add_epi32(bw,_mm_slli_epi32(bw,8)));
			/*SIMD instruction pBitmap128[i] = (alpha shift left 24 bits + bw shift left 16 bits) + ( bw shift left 8 bits + bw)*/
	}
    clock_gettime(CLOCK_MONOTONIC, &end);		/*Record end time*/


	printf("Execution time = %ld : %ld\n", diff(start, end).tv_sec, diff(start, end).tv_nsec);

	return 0;
}
