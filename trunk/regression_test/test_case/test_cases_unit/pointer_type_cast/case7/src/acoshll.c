//(C) Copyright 2016-2021 Xilinx, Inc.
//All Rights Reserved.
//
//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License. (edited)
/*							my_sqrtl.c
 *
 *	Square root, long double precision
 *
 *
 *
 * SYNOPSIS:
 *
 * long double x, y, my_sqrtl();
 *
 * y = my_sqrtl( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the square root of x.
 *
 * Range reduction involves isolating the power of two of the
 * argument and using a polynomial approximation to obtain
 * a rough value for the square root.  Then Heron's iteration
 * is used three times to converge to an accurate value.
 *
 * Note, some arithmetic coprocessors such as the 8087 and
 * 68881 produce correctly rounded square roots, which this
 * routine will not.
 *
 * ACCURACY:
 *
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0,10        30000       8.1e-20     3.1e-20
 *
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * sqrt domain        x < 0            0.0
 *
 */

/*
Cephes Math Library Release 2.2:  December, 1990
Copyright 1984, 1990 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include<math.h>
#include "mconf.h"

#define SQRT2 1.4142135623730950488017E0L

/* Variable for error reporting.  See mtherr.c.  */
int merror = 0;


long double my_sqrtl(long double x)
{
int e;
long double z, w;
#ifndef UNK
short *q;
#endif
long double my_frexpl(), my_ldexpl();

if( x <= 0.0 )
	{
	if( x < 0.0 )
#ifdef UNK
#else
    ;
#endif
	return( 0.0 );
	}
w = x;
/* separate exponent and significand */
#ifdef UNK
z = my_frexpl( x, &e );
#endif

/* Note, frexp and ldexp are used in order to
 * handle denormal numbers properly.
 */
#ifdef IBMPC
z = my_frexpl( x, &e );
q = (short *)&x; /* point to the exponent word */
q += 7;
/*
e = ((*q >> 4) & 0x0fff) - 0x3fe;
*q &= 0x000f;
*q |= 0x3fe0;
z = x;
*/
#endif
#ifdef MIEEE
z = my_frexpl( x, &e );
q = (short *)&x;
/*
e = ((*q >> 4) & 0x0fff) - 0x3fe;
*q &= 0x000f;
*q |= 0x3fe0;
z = x;
*/
#endif

/* approximate square root of number between 0.5 and 1
 * relative error of linear approximation = 7.47e-3
 */
/*
x = 0.4173075996388649989089L + 0.59016206709064458299663L * z;
*/

/* quadratic approximation, relative error 6.45e-4 */
x = ( -0.20440583154734771959904L  * z
     + 0.89019407351052789754347L) * z
     + 0.31356706742295303132394L;

/* adjust for odd powers of 2 */
if( (e & 1) != 0 )
	x *= SQRT2;

/* re-insert exponent */
#ifdef UNK
x = my_ldexpl( x, (e >> 1) );
#endif
#ifdef IBMPC
x = my_ldexpl( x, (e >> 1) );
/*
*q += ((e >>1) & 0x7ff) << 4;
*q &= 077777;
*/
#endif
#ifdef MIEEE
x = my_ldexpl( x, (e >> 1) );
/*
*q += ((e >>1) & 0x7ff) << 4;
*q &= 077777;
*/
#endif

/* Newton iterations: */
#ifdef UNK
x += w/x;
x = my_ldexpl( x, -1 );	/* divide by 2 */
x += w/x;
x = my_ldexpl( x, -1 );
x += w/x;
x = my_ldexpl( x, -1 );
x += w/x;
x = my_ldexpl( x, -1 );
#endif

/* Note, assume the square root cannot be denormal,
 * so it is safe to use integer exponent operations here.
 */
#ifdef IBMPC
x += w/x;
/* *q -= 1;*/
x *= 0.5L;
x += w/x;
/* *q -= 1;*/
x *= 0.5L;
x += w/x;
/* *q -= 1;*/
x *= 0.5L;
x += w/x;
/* *q -= 1;*/
x *= 0.5L;
#endif
#ifdef MIEEE
x += w/x;
*q -= 1;
x += w/x;
*q -= 1;
x += w/x;
*q -= 1;
x += w/x;
*q -= 1;
#endif

return(x);
}


/*							polevll.c
 *							p1evll.c
 *
 *	Evaluate polynomial
 *
 *
 *
 * SYNOPSIS:
 *
 * int N;
 * long double x, y, coef[N+1], polevl[];
 *
 * y = polevll( x, coef, N );
 *
 *
 *
 * DESCRIPTION:
 *
 * Evaluates polynomial of degree N:
 *
 *                     2          N
 * y  =  C  + C x + C x  +...+ C x
 *        0    1     2          N
 *
 * Coefficients are stored in reverse order:
 *
 * coef[0] = C  , ..., coef[N] = C  .
 *            N                   0
 *
 *  The function p1evll() assumes that coef[N] = 1.0 and is
 * omitted from the array.  Its calling arguments are
 * otherwise the same as polevll().
 *
 *
 * SPEED:
 *
 * In the interest of speed, there are no checks for out
 * of bounds arithmetic.  This routine is used by most of
 * the functions in the library.  Depending on available
 * equipment features, the user may wish to rewrite the
 * program in microcode or assembly language.
 *
 */


/*
Cephes Math Library Release 2.2:  July, 1992
Copyright 1984, 1987, 1988, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/


/* Polynomial evaluator:
 *  P[0] x^n  +  P[1] x^(n-1)  +  ...  +  P[n]
 */
double polevll(double x, void *PP, int n)
{
register double y;
double *P;

P = (double *) PP;
y = *P++;
do
	{
	y = y * x + *P++;
	}
while( --n );
return(y);
}



/* Polynomial evaluator:
 *  x^n  +  P[0] x^(n-1)  +  P[1] x^(n-2)  +  ...  +  P[n]
 */
double p1evll(double x, void *PP, int n)
{
register double y;
double *P;

P = (double *) PP;
n -= 1;
y = x + *P++;
do
	{
	y = y * x + *P++;
	}
while( --n );
return( y );
}


/*                                                      ceill()
 *                                                      floorl()
 *                                                      my_frexpl()
 *                                                      my_ldexpl()
 *                                                      fabsl()
 *							signbitl()
 *							isnanl()
 *							isfinitel()
 *
 *      Floating point numeric utilities
 *
 *
 *
 * SYNOPSIS:
 *
 * long double x, y;
 * long double ceill(), floorl(), my_frexpl(), my_ldexpl(), fabsl();
 * int signbitl(), isnanl(), isfinitel();
 * int expnt, n;
 *
 * y = floorl(x);
 * y = ceill(x);
 * y = my_frexpl( x, &expnt );
 * y = my_ldexpl( x, n );
 * y = fabsl( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * All four routines return a long double precision floating point
 * result.
 *
 * floorl() returns the largest integer less than or equal to x.
 * It truncates toward minus infinity.
 *
 * ceill() returns the smallest integer greater than or equal
 * to x.  It truncates toward plus infinity.
 *
 * my_frexpl() extracts the exponent from x.  It returns an integer
 * power of two to expnt and the significand between 0.5 and 1
 * to y.  Thus  x = y * 2**expn.
 *
 * my_ldexpl() multiplies x by 2**n.
 *
 * fabsl() returns the absolute value of its argument.
 *
 * signbitl(x) returns 1 if the sign bit of x is 1, else 0.
 *
 * These functions are part of the standard C run time library
 * for some but not all C compilers.  The ones supplied are
 * written in C for IEEE arithmetic.  They should
 * be used only if your compiler library does not already have
 * them.
 *
 * The IEEE versions assume that denormal numbers are implemented
 * in the arithmetic.  Some modifications will be required if
 * the arithmetic has abrupt rather than gradual underflow.
 */


/*
Cephes Math Library Release 2.2:  July, 1992
Copyright 1984, 1987, 1988, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/


#define DENORMAL 1

#ifdef UNK
char *unkmsg = "ceill(), floorl(), my_frexpl(), my_ldexpl() must be rewritten!\n";
#undef UNK
#define MIEEE 1
#define EXPOFS 0
#endif

#ifdef IBMPC
#define NBITS 113
#define EXPOFS 7
#endif

#ifdef MIEEE
#define NBITS 113
#define EXPOFS 0
#endif

extern long double MAXNUML;


long double fabsl(long double x)
{
if( x < 0 )
        return( -x );
else
        return( x );
}



long double ceill(long double x)
{
long double y;
long double floorl();

#ifdef UNK
return(0.0L);
#endif

y = floorl(x);
if( y < x )
        y += 1.0L;
return(y);
}




/* Bit clearing masks: */

static unsigned short bmask[] = {
0xffff,
0xfffe,
0xfffc,
0xfff8,
0xfff0,
0xffe0,
0xffc0,
0xff80,
0xff00,
0xfe00,
0xfc00,
0xf800,
0xf000,
0xe000,
0xc000,
0x8000,
0x0000,
};


long double floorl(long double x)
{
union FloorUnion u;
int e, j;

#ifdef UNK
return(0.0L);
#endif

u.y = x;
/* find the exponent (power of 2) */
e = (u.sh[EXPOFS] & 0x7fff) - 0x3fff;

if( e < 0 )
        {
        if( u.y < 0 )
                return( -1.0L );
        else
                return( 0.0L );
        }

#ifdef IBMPC
j = 0;
#endif

#ifdef MIEEE
j = 7;
#endif

e = (NBITS - 1) - e;
/* clean out 16 bits at a time */
while( e >= 16 )
        {
#ifdef IBMPC
        u.sh[j++] = 0;
#endif

#ifdef MIEEE
        u.sh[j--] = 0;
#endif
        e -= 16;
        }

/* clear the remaining bits */
if( e > 0 )
        u.sh[j] &= bmask[e];

if( (x < 0.0L) && (u.y != x) )
        u.y -= 1.0L;

return(u.y);
}


long double my_frexpl(long double x, int *pw2)
{
union FrexplUnion u;
int i, k;

u.y = x;

#ifdef UNK
return(0.0L);
#endif

/* find the exponent (power of 2) */
i  = u.sh[EXPOFS] & 0x7fff;

if( i == 0 )
        {
        if( u.y == 0.0L )
                {
                *pw2 = 0;
                return(0.0L);
                }
/* Number is denormal or zero */
#if DENORMAL
/* Handle denormal number. */
do
        {
        u.y *= 2.0L;
        i -= 1;
        k  = u.sh[EXPOFS] & 0x7fff;
        }
while( (k == 0) && (i > -115) );
i = i + k;
#else
        *pw2 = 0;
        return(0.0L);
#endif /* DENORMAL */
        }

*pw2 = i - 0x3ffe;
u.sh[EXPOFS] = 0x3ffe;
return( u.y );
}





long double my_ldexpl(long double x, int pw2)
{
    union LdexplUnion u;
    long e;

#ifdef UNK
return(0.0L);
#endif

u.y = x;
while( (e = (u.sh[EXPOFS] & 0x7fffL)) == 0 )
        {
#if DENORMAL
        if( u.y == 0.0L )
                {
                return( 0.0L );
                }
/* Input is denormal. */
        if( pw2 > 0 )
                {
                u.y *= 2.0L;
                pw2 -= 1;
                }
        if( pw2 < 0 )
                {
                if( pw2 < -113 )
                        return(0.0L);
                u.y *= 0.5L;
                pw2 += 1;
                }
        if( pw2 == 0 )
                return(u.y);
#else
        return( 0.0L );
#endif
        }

e = e + pw2;

/* Handle overflow */
if( e > 0x7ffeL )
        {
          e = u.sh[EXPOFS];
          u.y = 0.0L;
          u.sh[EXPOFS] = e | 0x7fff;
          return( u.y );
        }
u.sh[EXPOFS] &= 0x8000;
/* Handle denormalized results */
if( e < 1 )
        {
#if DENORMAL
        if( e < -113 )
                return(0.0L);
        u.sh[EXPOFS] |= 1;
        while( e < 1 )
                {
                u.y *= 0.5L;
                e += 1;
                }
        e = 0;
#else
        return(0.0L);
#endif
        }

u.sh[EXPOFS] |= e & 0x7fff;
return(u.y);
}

/* Return 1 if x is a number that is Not a Number, else return 0.  */
#ifdef NANS
int isnanl(long double x)
{
union IsnanlUnion u;
u.d = x;

if( sizeof(int) == 4 )
	{
#ifdef IBMPC	    
	if( ((u.s[7] & 0x7fff) == 0x7fff)
	    && ((u.i[3] & 0x7fff) | u.i[2] | u.i[1] | u.i[0]))
		return 1;
#endif
#ifdef MIEEE
	if( ((u.i[0] & 0x7fff0000) == 0x7fff0000)
	    && ((u.i[0] & 0x7fff) | u.i[1] | u.i[2] | u.i[3]))
		return 1;
#endif
	return(0);
	}
else
	{ /* size int not 4 */
#ifdef IBMPC
	if( (u.s[7] & 0x7fff) == 0x7fff)
		{
		if((u.s[6] & 0x7fff) | u.s[5] | u.s[4] | u.s[3] | u.s[2] | u.s[1] | u.s[0])
			return(1);
		}
#endif
#ifdef MIEEE
	if( (u.s[0] & 0x7fff) == 0x7fff)
		{
		if((u.s[1] & 0x7fff) | (u.s[2] & 0x7fff) | u.s[3] | u.s[4] | u.s[5] | u.s[6] | u.s[7])
			return(1);
		}
#endif
	return(0);
	} /* size int not 4 */

#else
int isnanl(long double x)
{
/* No NANS.  */
return(0);
#endif
}


/* Return 1 if x is not infinite and is not a NaN.  */

#ifdef INFINITIES
int isfinitel(long double x)
{
union IsfinitelUnion u;
u.d = x;

if( sizeof(int) == 4 )
	{
#ifdef IBMPC
	if( (u.s[7] & 0x7fff) != 0x7fff)
		return 1;
#endif
#ifdef MIEEE
	if( (u.i[0] & 0x7fff0000) != 0x7fff0000)
		return 1;
#endif
	return(0);
	}
else
	{
#ifdef IBMPC
	if( (u.s[7] & 0x7fff) != 0x7fff)
		return 1;
#endif
#ifdef MIEEE
	if( (u.s[0] & 0x7fff) != 0x7fff)
		return 1;
#endif
	return(0);
	}
#else
/* No INFINITY.  */
int isfinitel(long double x)
{
return(1);
#endif
}


/* Return 1 if the sign bit of x is 1, else 0.  */
int signbitl(long double x)
{
union SignbitUnion u;
u.d = x;

if( sizeof(int) == 4 )
	{
#ifdef IBMPC
	return( u.s[7] < 0 );
#endif
#ifdef DEC
error no such DEC format
#endif
#ifdef MIEEE
	return( u.i[0] < 0 );
#endif
	}
else
	{
#ifdef IBMPC
	return( u.s[7] < 0 );
#endif
#ifdef DEC
error no such DEC format
#endif
#ifdef MIEEE
	return( u.s[0] < 0 );
#endif
	}
}


/*							my_acoshl.c
 *
 *	Inverse hyperbolic cosine, 128-bit long double precision
 *
 *
 *
 * SYNOPSIS:
 *
 * long double x, y, my_acoshl();
 *
 * y = my_acoshl( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns inverse hyperbolic cosine of argument.
 *
 * If 1 <= x < 1.5, a rational approximation
 *
 *	sqrt(2z) * P(z)/Q(z)
 *
 * where z = x-1, is used.  Otherwise,
 *
 * acosh(x)  =  log( x + sqrt( (x-1)(x+1) ).
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      1,3        100,000      4.1e-34     7.3e-35
 *
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * my_acoshl domain      |x| < 1            0.0
 *
 */

/*							acosh.c	*/

/*
Cephes Math Library Release 2.2:  January, 1991
Copyright 1984, 1991 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/




/* acosh(1+x) = sqrt(2x) * R(x), interval 0 < x < 0.5
 * Theoretical peak relative error = 8.2e-36
 * relative peak error spread = 9.9e-8
 */
static double P[10] = {
 1.895467874386341763387398084072833727168E-1L,
 6.443902084393244878979969557171256604767E1L,
 3.914593556594721458616408528941154205393E3L,
 9.164040999602964494412169748897754668733E4L,
 1.065909694792026382660307834723001543839E6L,
 6.899169896709615182428217047370629406305E6L,
 2.599781868717579447900896150777162652518E7L,
 5.663733059389964024656501196827345337766E7L,
 6.606302846870644033621560858582696134512E7L,
 3.190482951215438078279772140481195200593E7L
};
static double Q[9] = {
/* 1.000000000000000000000000000000000000000E0L, */
 1.635418024331924674147953764918262009321E2L,
 7.290983678312632723073455563799692165828E3L,
 1.418207894088607063257675159183397062114E5L,
 1.453154285419072886840913424715826321357E6L,
 8.566841438576725234955968880501739464425E6L,
 3.003448667795089562511136059766833630017E7L,
 6.176592872899557661256383958395266919654E7L,
 6.872176426138597206811541870289420510034E7L,
 3.190482951215438078279772140481195226621E7L
};


extern double LOGE2L;

double my_acoshl(double x)
{
double a, z;

if( x < 1.0L )
	{
#ifdef UNK
	return(0.0L);
#endif
	}

if( x > 1.0e17L )
	return( logf(x) + LOGE2L );

z = x - 1.0L;

if( z < 0.5L )
	{
	a = my_sqrtl(2.0L*z) * (polevll(z, P, 9) / p1evll(z, Q, 9) );
	return( a );
	}

a = my_sqrtl( z*(x+1.0L) );
return( logf(x + a) );
}

#pragma ACCEL kernel
double acoshl_kernel()
{
	return my_acoshl(0.5);
}
