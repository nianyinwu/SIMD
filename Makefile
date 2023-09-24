all:version_C.c version_simd.c
	gcc -O3 version_C.c -lrt -o version_C
	gcc -msse -msse2 -O3 version_simd.c -o version_simd 

clean:
	rm -f version_C version_simd
