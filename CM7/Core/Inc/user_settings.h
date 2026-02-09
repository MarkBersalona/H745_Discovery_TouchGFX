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
