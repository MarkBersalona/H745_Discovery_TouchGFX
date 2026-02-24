/* Keep the Cube pack configuration */
#include "wolfSSL.I-CUBE-wolfSSL_conf.h"

/* Include wolfSSL CMAC routines and definitions */
#ifndef WOLFSSL_CMAC
#define WOLFSSL_CMAC
#endif
// Include wolfSSL AES
#ifndef WOLFSSL_AES_DIRECT
#define WOLFSSL_AES_DIRECT
#endif

#ifndef HAVE_AES_ECB
#define HAVE_AES_ECB
#endif
#ifndef HAVE_AESCCM
#define HAVE_AESCCM
#endif

////////////////////////////////////////////////////////////////////////////////////
// TEST MAB 2026.02.18
// Enable these defines to use static memory and possibly no fallback to dynamic
//#define WOLFSSL_STATIC_MEMORY  // Use static instead of dynamically-allocated memory
//#define WOLFSSL_NO_MALLOC      // If static memory runs out, don't fallback to dynamic
////////////////////////////////////////////////////////////////////////////////////
