/*************************************************************
  ZFFT.C   --  fast Fourier transform algorithm
    Based on Norm Brenner's neat bit-reverser, as given in
    "Numerical Recipes ..." by W.H.Press et al
        (Cambridge Univ Press, 1986)
*************************************************************/
#include <math.h>
#include <stdlib.h>
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
#define  PI	M_PI

typedef struct { float r, i; } cmplex;
static cmplex *wsto=0
        ;
static int len_sto=0, flag_sto=0
        ;
static float norm_fac=0.0
        ;

/*****************************  ZFFT  *******************************/
int two_pwr(int) ;
int zfft(cmplex *fdat,int len,int flag)
/* = 0 for forward, !0 for reverse transform */
{
    int k, m, n, isign
            ;
    cmplex *fi, *fj, *flim, *wptr
            ;
    cmplex w, temp
            ;
    float dtemp
            ;

    isign = (flag) ? 1 : 0 ;
    if ( (fdat == 0) || (len <= 0) )  return -1;  /*  nothing to transform*/

    /* convert len, if necessary, to next lower pwr of 2  */
    len = two_pwr( len);

    /* allocate space for trig array, if necessary */
    if (len != len_sto)
    {
        if ( wsto )  free ( (char*)wsto);
        wptr = wsto = (cmplex*)malloc( len*sizeof(cmplex) );
        dtemp = ( isign) ? -PI : PI;
        /*  -- forward or reverse
    */
        norm_fac = 1.0/sqrt( (double)len);
    }

    wptr = wsto ;

    /* shuffle the array -- for the entire data array, reverse the bit-order
    of each index # (i.e. switch the data to its bit-reversed location)
*/
    m = len;
    flim = fdat + len;
    for (fi = (fj = fdat) ; fi < flim; fi++ )
    {
        /*  switch only at the the lower value
            of the pair ( i.e. don't re-switch! ): --
    */
        if (fi < fj)  temp = *fi, *fi = *fj, *fj = temp;
        /*  now calc the reversed-order value for the next index i : --
    */
        for ( m=(len>>1);  (m>1) && ( (fj-fdat) >= m) ; m >>= 1  ) fj -= m;
        fj += m ;
        /*  now for the trig, if needed --
    */
        if ( len != len_sto )
        {
            if ( fabs(dtemp) > 1.0e-100)
            {
                wptr->r = cos( (double)dtemp);	/*  dtemp = PI/(2**(m-1)), hence -- */
                wptr->i = sin((double)dtemp);	/* -- wsto = exp( j PI/(2**(m-1)))  */
                dtemp *= 0.5;
            }
            else
            {
                wptr->r = 1.0;
                wptr->i = 0.0;
            }
            wptr++;
        }
        else
            if (flag_sto != isign)
            {
                wptr->i = -(wptr->i);
                wptr++;
            }
    }

    /*  store the params to save -- */
    len_sto = len;
    flag_sto = isign;


    /* perform the transform
*/
    for ( m = 1, wptr=wsto; m < len; wptr++     )	/*  m = 1, 2, 4, 8, 16 ...   */
    {
        k = m<<1;

        /*  initial trig values  */
        w.r = 1.0;
        w.i = 0.0;

        for ( n = 0; n < m; n++)
        {
            for (fi = fdat+n; fi < flim; fi += k )
            {
                fj = fi+m;

                /*  temp = w * fj  */
                temp.r = w.r * fj->r - w.i * fj->i;
                temp.i = w.r * fj->i + w.i * fj->r;

                /*  fj = fi - temp  */
                fj->r = fi->r - temp.r;
                fj->i = fi->i - temp.i;

                /*  fi + fi + temp  */
                fi->r += temp.r;
                fi->i += temp.i;
            }

            /*  w = w * wptr  -- and, wptr = wsto = exp( j PI/(2**(m-1)))  */
            temp.r = w.r;
            w.r = w.r * wptr->r - w.i * wptr->i;
            w.i = w.i * wptr->r + temp.r * wptr->i;
        }
        m = k;	/*  --  next power of 2  */
    }
    for ( fj=fdat; fj<flim; fj++ )
    {
        fj->r *= norm_fac;
        fj->i *= norm_fac;
    }

    //  MSW
    free(wsto);
    wsto=0;
    len_sto=0;
    flag_sto=0;
    norm_fac=0.0;

    return 0 ;
}


int two_pwr(int n)
{
    int j;

    j=0;
    while ( n)
    {
        j = ( j) ? j << 1 : 1 ;
        n >>= 1;
    }
    return( j);
}
