/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/* $Header: /home/kbs/jutta/src/gsm/gsm-1.0/src/RCS/lpc.c,v 1.1 1992/10/28 00:15:50 jutta Exp $ */

#ifdef _MED_C89_
#include <stdio.h>
#endif

#include "fr_lpc.h"
#include "fr_etsi_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 *  4.2.4 .. 4.2.7 LPC ANALYSIS SECTION
 */

/* 4.2.4 */


static void Autocorrelation(
    Word16       * s,       /* [0..159] IN/OUT  */
    Word32   * L_ACF,   /* [0..8]   OUT     */
    Word16     * pshwVadScalauto)
/*
 *  The goal is to compute the array L_ACF[k].  The signal s[i] must
 *  be scaled in order to avoid an overflow situation.
 */
{
     Word32    k, i;

    Word16        smax, scalauto;
    /*  Word32    L_temp; */

#ifdef  USE_FLOAT_MUL
    float       float_s[160];
    /*  Word32    L_temp2; */
#endif

    /*  Dynamic scaling of the array  s[0..159]
     */

    /*  Search for the maximum*/
    smax = CODEC_OpVecMaxAbs(s, 160, 0);

    /*  Computation of the scaling factor.
     */
    if (smax == 0) scalauto = 0;
    else {
        scalauto = 4 - gsm_norm( (Word32)smax << 16 );/* sub(4,..) */
    }

    /*  Scaling of the array s[0...159]
     */

    if (scalauto > 0) {

#ifdef USE_FLOAT_MUL
#define SCALE(n) \
    case n: for (k = 0; k <= 159; k++) \
            float_s[k] = (float)    \
                (s[k] = GSM_MULT_R(s[k], 16384 >> (n-1)));\
        break;
#else
#define SCALE(n) \
    case n: CODEC_OpVcMultR(s, 160, (16384 >> (n-1)), s);\
        break;
#endif /* USE_FLOAT_MUL */

        switch (scalauto) {
        SCALE(1)
        SCALE(2)
        SCALE(3)
        SCALE(4)
        }
# undef SCALE
    }
# ifdef USE_FLOAT_MUL
    else for (k = 0; k <= 159; k++) float_s[k] = (float) s[k];
# endif

    /*  Compute the L_ACF[..].
     */
    {
# ifdef USE_FLOAT_MUL
         float * sp = float_s;
         float   sl = *sp;
# else
        Word16  * sp = s;
        Word16    sl = *sp;
# endif

#   define STEP(k)   L_ACF[k] += (Word32)(sl * sp[ -(k) ]);
#   define NEXTI     sl = *++sp


    for (k = 9; k--; L_ACF[k] = 0) ;

    STEP (0);
    NEXTI;
    STEP(0); STEP(1);
    NEXTI;
    STEP(0); STEP(1); STEP(2);
    NEXTI;
    STEP(0); STEP(1); STEP(2); STEP(3);
    NEXTI;
    STEP(0); STEP(1); STEP(2); STEP(3); STEP(4);
    NEXTI;
    STEP(0); STEP(1); STEP(2); STEP(3); STEP(4); STEP(5);
    NEXTI;
    STEP(0); STEP(1); STEP(2); STEP(3); STEP(4); STEP(5); STEP(6);
    NEXTI;
    STEP(0); STEP(1); STEP(2); STEP(3); STEP(4); STEP(5); STEP(6); STEP(7);

    for (i = 8; i <= 159; i++) {

        NEXTI;

        STEP(0);
        STEP(1); STEP(2); STEP(3); STEP(4);
        STEP(5); STEP(6); STEP(7); STEP(8);
    }

    for (k = 9; k--; L_ACF[k] <<= 1) ;

    }

    /*   Rescaling of the array s[0..159]
     */
    if (scalauto > 0)
    {
        CODEC_OpVecShl(&s[0], 160, scalauto, &s[0]);
    }

    *pshwVadScalauto = scalauto;
}

#if defined(USE_FLOAT_MUL) && defined(FAST)

static void Fast_Autocorrelation(
    Word16 * s,       /* [0..159] IN/OUT  */
    Word32 * L_ACF)   /* [0..8]   OUT     */
{
     int    k, i;
    float f_L_ACF[9];
    float scale;

    float          s_f[160];
     float *sf = s_f;

    for (i = 0; i < 160; ++i) sf[i] = s[i];
    for (k = 0; k <= 8; k++) {
         float L_temp2 = 0;
         float *sfl = sf - k;
        for (i = k; i < 160; ++i) L_temp2 += sf[i] * sfl[i];
        f_L_ACF[k] = L_temp2;
    }
    scale = MAX_LONGWORD / f_L_ACF[0];

    for (k = 0; k <= 8; k++) {
        L_ACF[k] = f_L_ACF[k] * scale;
    }
}
#endif  /* defined (USE_FLOAT_MUL) && defined (FAST) */

/* 4.2.5 */

static void Reflection_coefficients(
    Word32    * L_ACF,        /* 0...8    IN  */
    Word16   * r         /* 0...7    OUT     */
)
{
    int    i, m, n;
    Word16   temp;

    Word16        ACF[9]; /* 0..8 */
    Word16        P[  9]; /* 0..8 */
    Word16        K[  9]; /* 2..8 */

    /*  Schur recursion with 16 bits arithmetic.
     */

    if (L_ACF[0] == 0) {    /* everything is the same. */
        for (i = 8; i--; *r++ = 0) ;
        return;
    }

    temp = gsm_norm( L_ACF[0] );


    /* ? overflow ? */
    for (i = 0; i <= 8; i++) ACF[i] = SASR( L_ACF[i] << temp, 16 );

    /*   Initialize array P[..] and K[..] for the recursion.
     */

    CODEC_OpVecCpy(&K[1], &ACF[1], 7);
    CODEC_OpVecCpy(&P[0], &ACF[0], 9);

    /*   Compute reflection coefficients
     */
    for (n = 1; n <= 8; n++, r++) {

        temp = P[1];
        temp = GSM_ABS(temp);
        if (P[0] < temp) {
            for (i = n; i <= 8; i++) *r++ = 0;
            return;
        }

        *r = gsm_div( temp, P[0] );

        if (P[1] > 0) *r = -*r;     /* r[n] = sub(0, r[n]) */
        if (n == 8) return;

        /*  Schur recursion
         */
        temp = GSM_MULT_R( P[1], *r );
        P[0] = GSM_ADD( P[0], temp );

        for (m = 1; m <= 8 - n; m++) {
            temp     = GSM_MULT_R( K[ m   ],    *r );
            P[m]     = GSM_ADD(    P[ m+1 ],  temp );

            temp     = GSM_MULT_R( P[ m+1 ],    *r );
            K[m]     = GSM_ADD(    K[ m   ],  temp );
        }
    }
}

/* 4.2.6 */

static void Transformation_to_Log_Area_Ratios(
     Word16   * r             /* 0..7    IN/OUT */
)
/*
 *  The following scaling for r[..] and LAR[..] has been used:
 *
 *  r[..]   = integer( real_r[..]*32768. ); -1 <= real_r < 1.
 *  LAR[..] = integer( real_LAR[..] * 16384 );
 *  with -1.625 <= real_LAR <= 1.625
 */
{
     Word16   temp;
     int    i;


    /* Computation of the LAR[0..7] from the r[0..7]
     */
    for (i = 1; i <= 8; i++, r++) {

        temp = *r;
        temp = GSM_ABS(temp);

        if (temp < 22118) {
            temp >>= 1;
        } else if (temp < 31130) {
            temp -= 11059;
        } else {
            temp -= 26112;
            temp <<= 2;
        }

        *r = *r < 0 ? -temp : temp;
    }
}

/* 4.2.7 */

void Quantization_and_coding(
     Word16 * LAR     /* [0..7]   IN/OUT  */
)
{
     Word16   temp;
    /*  UWord32   utmp; reported as unused */


    /*  This procedure needs four tables; the following equations
     *  give the optimum scaling for the constants:
     *
     *  A[0..7] = integer( real_A[0..7] * 1024 )
     *  B[0..7] = integer( real_B[0..7] *  512 )
     *  MAC[0..7] = maximum of the LARc[0..7]
     *  MIC[0..7] = minimum of the LARc[0..7]
     */

#undef  STEP
#define STEP( A, B, MAC, MIC )      \
        temp = GSM_MULT( A,   *LAR );   \
        temp = GSM_ADD(  temp,   B );   \
        temp = GSM_ADD(  temp, 256 );   \
        temp = SASR(     temp,   9 );   \
        *LAR  =  temp>MAC ? MAC - MIC : (temp<MIC ? 0 : temp - MIC); \
        LAR++;

    STEP(  20480,     0,  31, -32 );
    STEP(  20480,     0,  31, -32 );
    STEP(  20480,  2048,  15, -16 );
    STEP(  20480, -2560,  15, -16 );

    STEP(  13964,    94,   7,  -8 );
    STEP(  15360, -1792,   7,  -8 );
    STEP(   8534,  -341,   3,  -4 );
    STEP(   9036, -1144,   3,  -4 );

#undef   STEP
}

void Gsm_LPC_Analysis(
    Word8          fast,
    Word16         * s,       /* 0..159 signals   IN/OUT  */
    Word16         * LARc,    /* 0..7   LARc's    OUT */
    Word32       * aswVadLacf,
    Word16       * pshwVadScalauto,
    Word16       * ashwDtxLaruq)
{
    Word32    L_ACF[9];

    Word32                         swCnt = 0;

#if defined(USE_FLOAT_MUL) && defined(FAST)
    if (fast) Fast_Autocorrelation (s, L_ACF );
    else
#endif

    Autocorrelation(s, L_ACF, pshwVadScalauto);

    for ( swCnt = 0 ; swCnt < 9 ; swCnt++ )
    {
        aswVadLacf[swCnt] = L_ACF[swCnt];
    }

    Reflection_coefficients(L_ACF, LARc);

    Transformation_to_Log_Area_Ratios (LARc);

    CODEC_OpVecCpy(&ashwDtxLaruq[0], &LARc[0], 8);

    Quantization_and_coding(LARc);
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
