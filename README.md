#SIMD
1. 程式內容
將變數bw alpha red green blue都宣告為__m128i且作對齊
並宣告__m128 v, v1 ,v2分別存放主要運算中要乘的數(0.299,0.587,0.114)，宣告__m128i s存放要AND的數(0xFF)
使用SIMD的資料型態_m128i設定pBitmap128指標，並將其指向pBitmap，使pBitmap中元素變成每4個為一組
所以總共要運算的次數會變成(height*weight)/4
主要運算的部分：(同時做四個元素，因為__m128i是四個元素為一組)
(1)使用SIMD instruction _mm_srai_epi32(pBitmap128[i],24)先將pBitmap128[i]往右移24 bits(有signed)，
   再使用_mm_and_si128(前述結果,s) 與s(0xFF)作AND得到alpha
   使用SIMD instruction _mm_srai_epi32(pBitmap128[i],16)先將pBitmap128[i]往右移16 bits(有signed)，
   再使用_mm_and_si128(前述結果,s) 與s(0xFF)作AND得到red
   使用SIMD instruction _mm_srai_epi32(pBitmap128[i],8)先將pBitmap128[i]往右移8 bits(有signed)，
   再使用_mm_and_si128(前述結果,s) 與s(0xFF)作AND得到green
   使用SIMD instruction _mm_and_si128(pBitmap128[i],s) 與s(0xFF)作AND得到blue
(2)使用SIMD instruction _mm_cvtepi32_ps將red與green作轉型(4個int->4個float)後，
   接著使用_mm_mul_ps將red與v相乘、green相乘v1，最後使用_mm_add_ps將前述結果相加(float)存放於變數t
   然後使用SIMD instruction _mm_cvtepi32_ps將blue作轉型(4個int->4個float)並使用_mm_mul_ps將其結果與v2相乘，
   接著使用_mm_add_ps將其相乘結果與t作相加，最後使用_mm_cvttps_epi32再將結果轉型(4個float->4個int)存放於bw
(3)使用SIMD instruction _mm_slli_epi32將alpha左移24 bits、bw左移16 bits，再使用_mm_add_epi32將兩者相加(1)
   使用SIMD instruction _mm_slli_epi32將bw左移8 bits，再使用_mm_add_epi32將前述結果與bw作相加(2)
   最後將(1)(2)使用_mm_add_epi32作相加，並結果存回pBitmap128[i]

(diff計算時間差)

2. 執行環境
CPU : Intel(R) Core(TM) i5-1035G1 CPU @ 1.00GHz
flag : fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr 
sse sse2 ss ht syscall nx pdpe1gb rdtscp lm constant_tsc rep_good nopl xtopology tsc_reliable 
nonstop_tsc cpuid pni pclmulqdq ssse3 fma cx16 pcid sse4_1 sse4_2 movbe popcnt aes xsave avx 
f16c rdrand hypervisor lahf_lm abm 3dnowprefetch invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced 
fsgsbase bmi1 avx2 smep bmi2 erms invpcid avx512f avx512dq rdseed adx smap avx512ifma clflushopt 
avx512cd sha_ni avx512bw avx512vl xsaveopt xsavec xgetbv1 xsaves avx512vbmi umip avx512_vbmi2 gfni 
vaes vpclmulqdq avx512_vnni avx512_bitalg avx512_vpopcntdq rdpid fsrm flush_l1d arch_capabilities

3. 主要運算的執行時間
Non-SIMD version: 0 : 634682466
SIMD version: 0 : 348171205

4. 如何編譯程式
(1)
version_C.c：
在wsl的視窗中輸入:gcc -O3 version_C.c -lrt -o version_C
version_simd.c：
在wsl的視窗中輸入:gcc -msse -msse2 -O3 version_simd.c -o version_simd 
(2)
使用Makefile，在wsl的視窗中輸入make	

5. 如何執行程式
在編譯完程式後會產生version_C與version_simd執行檔，輸入./version_C與./version_simd執行程式，
視窗會顯示其主要運算執行時間

6. 使用指令
SSE：__m128 _mm_set_ps、__m128 _mm_mul_ps
SSE2:__m128i _mm_add_epi32、__m128 _mm_cvtepi32_ps、
	 __m128i _mm_cvttps_epi32、__m128i _mm_set_epi32
	 __m128i _mm_srai_epi32、__m128i _mm_slli_epi32
	 __m128i _mm_and_si128
