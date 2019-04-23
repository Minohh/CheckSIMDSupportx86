#include <stdio.h>

#define CPUID_EAX 0
#define CPUID_EBX 1
#define CPUID_ECX 2
#define CPUID_EDX 3

#define OCR0_EAX 100

enum SUPPORT{
    /* 0 */
    CPU_MMX = 0,
    CPU_SSE,
    CPU_SSE2,
    CPU_SSE3,
    CPU_SSSE3,
    CPU_FMA,
    CPU_SSE4_1,
    CPU_SSE4_2,
    CPU_AES,
    CPU_AVX,
    CPU_AVX2,
    /* 10 */
    CPU_AVX512,
    CPU_XSAVE,
    CPU_OSXSAVE,

    /* 20 */
    OS_MMX = 20,
    OS_XMM,
    OS_YMM,
    OS_ZMM0_15,
    OS_ZMM16_31,
    OS_OPMASK
};

typedef struct SIMD_Info {
    int b;
    int eax;
    int get_reg;
    int verify;
    char *name;
    char *description;
} SIMD_Info;

SIMD_Info simd_list[32] = {
    { CPU_MMX,      1, CPUID_EDX, 23, "MMX",        "CPU Support MMX" },
    { CPU_SSE,      1, CPUID_EDX, 25, "SSE",        "CPU Support SSE" },
    { CPU_SSE2,     1, CPUID_EDX, 26, "SSE2",       "CPU Support SSE2" },
    { CPU_SSE3,     1, CPUID_ECX,  0, "SSE3",       "CPU Support SSE3" },
    { CPU_SSSE3,    1, CPUID_ECX,  9, "SSSE3",      "CPU Support SSSE3" },
    { CPU_FMA,      1, CPUID_ECX, 12, "FMA",        "CPU Support FMA" },
    { CPU_SSE4_1,   1, CPUID_ECX, 19, "SSE4.1",     "CPU Support SSE4.1" },
    { CPU_SSE4_2,   1, CPUID_ECX, 20, "SSE4.2",     "CPU Support SSE4.2" },
    { CPU_AES,      1, CPUID_ECX, 25, "AES",        "CPU Support AES" },
    { CPU_AVX,      1, CPUID_ECX, 28, "AVX",        "CPU Support AVX" },
    { CPU_XSAVE,    1, CPUID_ECX, 26, "XSAVE",      "CPU Support XSAVE" },
    { CPU_OSXSAVE,  1, CPUID_ECX, 27, "OSXSAVE",    "CPU Support OSXSAVE" },
    { CPU_AVX2,     7, CPUID_EBX,  5, "AVX2",       "CPU Support AVX2" },
    { CPU_AVX512,   7, CPUID_EBX, 16, "AVX512",     "CPU Support AVX512" },
    
    { OS_MMX,      -1, OCR0_EAX,   0, "MMX",        "OS Support MMX" },
    { OS_XMM,      -1, OCR0_EAX,   1, "XMM",        "OS Support XMM (SSE series)" },
    { OS_YMM,      -1, OCR0_EAX,   2, "YMM",        "OS Support YMM (AVX/AVX2)" },
    { OS_ZMM0_15,  -1, OCR0_EAX,   6, "ZMM0-ZMM15", "OS Support ZMM0 - ZMM15 (AVX512)" },
    { OS_ZMM16_31, -1, OCR0_EAX,   7, "ZMM16-ZMM31", "OS Support ZMM16 - ZMM31 (AVX512)" },
    { OS_OPMASK,   -1, OCR0_EAX,   5, "OPMASK",     "OS Support OPMASK (AVX512)" },
    
    { -1,          -1,       -1,  -1, "",           "" }
};

inline static void cpuid (unsigned int output[4], unsigned int EAX, unsigned int ECX) {	
    unsigned int a, b, c, d;
    __asm("cpuid"  : "=a"(a),"=b"(b),"=c"(c),"=d"(d) : "a"(EAX),"c"(ECX) : );
    output[0] = a;
    output[1] = b;
    output[2] = c;
    output[3] = d;
}

inline static unsigned int xgetbv (unsigned int ECX) {
    unsigned int ret = 0;
    __asm("xgetbv" : "=a"(ret) : "c"(ECX) : );
    return ret;
}

int main(){
    unsigned int flag = 0;
    unsigned int CPUInfo[4] = {0};
    unsigned int XCR0_EAX = 0;

    int i;
    int last_eax_val = 0;
    SIMD_Info entry = {0};

    /* CPU Support */
    for (i = 0; i < sizeof(simd_list); i++) {
        entry = simd_list[i];
        if (entry.b == -1)
            break;

        /*  CPUID */
        if (entry.b < OS_MMX){
            if (entry.eax != last_eax_val) {
                cpuid(CPUInfo, entry.eax, 0);
                last_eax_val = entry.eax;
            }
            if (CPUInfo[entry.get_reg] & (1 << entry.verify))
                flag |= (1 << entry.b);
        } else {
            break;
        }
    }

    /* OS Support*/
    XCR0_EAX = xgetbv(0);
    for (i++ ; i < sizeof(simd_list); i++) {
        entry = simd_list[i]; 
        if (entry.b == -1)
            break;

        if (XCR0_EAX & (1 << entry.verify))
            flag |= (1 << entry.b);
    }


    /*  print */
    for (i = 0; i < sizeof(simd_list); i++) {
        entry = simd_list[i];
        if (entry.b == -1)
            break;

        if (flag & (1 << entry.b)) {
            printf("%s\n", entry.description);
        }
    }
    //printf("ECX=%x, EDX=%x, XCR0_EAX=%x\n", ECX, EDX, XCR0_EAX);
    return 0;
}
