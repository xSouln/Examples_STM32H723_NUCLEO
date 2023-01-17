/* fp_mul_comba_7.i
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */



#ifdef TFM_MUL7
int fp_mul_comba7(fp_int *A, fp_int *B, fp_int *C)
{
   fp_digit c0, c1, c2;
#ifndef WOLFSSL_SMALL_STACK
   fp_digit at[14];
#else
   fp_digit *at;
#endif

#ifdef WOLFSSL_SMALL_STACK
   at = (fp_digit*)XMALLOC(sizeof(fp_digit) * 14, NULL, DYNAMIC_TYPE_TMP_BUFFER);
   if (at == NULL)
       return FP_MEM;
#endif

   XMEMCPY(at, A->dp, 7 * sizeof(fp_digit));
   XMEMCPY(at+7, B->dp, 7 * sizeof(fp_digit));
   COMBA_START;

   COMBA_CLEAR;
   /* 0 */
   MULADD(at[0], at[7]); 
   COMBA_STORE(C->dp[0]);
   /* 1 */
   COMBA_FORWARD;
   MULADD(at[0], at[8]);    MULADD(at[1], at[7]); 
   COMBA_STORE(C->dp[1]);
   /* 2 */
   COMBA_FORWARD;
   MULADD(at[0], at[9]);    MULADD(at[1], at[8]);    MULADD(at[2], at[7]); 
   COMBA_STORE(C->dp[2]);
   /* 3 */
   COMBA_FORWARD;
   MULADD(at[0], at[10]);    MULADD(at[1], at[9]);    MULADD(at[2], at[8]);    MULADD(at[3], at[7]); 
   COMBA_STORE(C->dp[3]);
   /* 4 */
   COMBA_FORWARD;
   MULADD(at[0], at[11]);    MULADD(at[1], at[10]);    MULADD(at[2], at[9]);    MULADD(at[3], at[8]);    MULADD(at[4], at[7]); 
   COMBA_STORE(C->dp[4]);
   /* 5 */
   COMBA_FORWARD;
   MULADD(at[0], at[12]);    MULADD(at[1], at[11]);    MULADD(at[2], at[10]);    MULADD(at[3], at[9]);    MULADD(at[4], at[8]);    MULADD(at[5], at[7]); 
   COMBA_STORE(C->dp[5]);
   /* 6 */
   COMBA_FORWARD;
   MULADD(at[0], at[13]);    MULADD(at[1], at[12]);    MULADD(at[2], at[11]);    MULADD(at[3], at[10]);    MULADD(at[4], at[9]);    MULADD(at[5], at[8]);    MULADD(at[6], at[7]); 
   COMBA_STORE(C->dp[6]);
   /* 7 */
   COMBA_FORWARD;
   MULADD(at[1], at[13]);    MULADD(at[2], at[12]);    MULADD(at[3], at[11]);    MULADD(at[4], at[10]);    MULADD(at[5], at[9]);    MULADD(at[6], at[8]); 
   COMBA_STORE(C->dp[7]);
   /* 8 */
   COMBA_FORWARD;
   MULADD(at[2], at[13]);    MULADD(at[3], at[12]);    MULADD(at[4], at[11]);    MULADD(at[5], at[10]);    MULADD(at[6], at[9]); 
   COMBA_STORE(C->dp[8]);
   /* 9 */
   COMBA_FORWARD;
   MULADD(at[3], at[13]);    MULADD(at[4], at[12]);    MULADD(at[5], at[11]);    MULADD(at[6], at[10]); 
   COMBA_STORE(C->dp[9]);
   /* 10 */
   COMBA_FORWARD;
   MULADD(at[4], at[13]);    MULADD(at[5], at[12]);    MULADD(at[6], at[11]); 
   COMBA_STORE(C->dp[10]);
   /* 11 */
   COMBA_FORWARD;
   MULADD(at[5], at[13]);    MULADD(at[6], at[12]); 
   COMBA_STORE(C->dp[11]);
   /* 12 */
   COMBA_FORWARD;
   MULADD(at[6], at[13]); 
   COMBA_STORE(C->dp[12]);
   COMBA_STORE2(C->dp[13]);
   C->used = 14;
   C->sign = A->sign ^ B->sign;
   fp_clamp(C);
   COMBA_FINI;

#ifdef WOLFSSL_SMALL_STACK
   XFREE(at, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
   return FP_OKAY;
}
#endif
