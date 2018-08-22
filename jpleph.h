#ifndef _JPLEPH_H
#define _JPLEPH_H 1

/***************************************************************************
*******                  JPLEPH.H                                  *********
****************************************************************************
**  This header file is used both by ASC2EPH and TESTEPH programs.        **
****************************************************************************
**  Written: May 28, 1997 by PAD   **  Last modified: June 23,1997 by PAD **
**  Modified further by Bill Gray,  Jun-Aug 2001                          **
****************************************************************************
**  PAD: dr. Piotr A. Dybczynski,          e-mail: dybol@phys.amu.edu.pl  **
**   Astronomical Observatory of the A.Mickiewicz Univ., Poznan, Poland   **
***************************************************************************/

/* By default,  in Windoze 32,  the JPL ephemeris functions are compiled
   into a DLL.  This is not really all that helpful at present,  but may
   be useful to people who want to use the functions from languages other
   than C. */

#include "configCosmos.h"

//#ifdef _WIN32
//#define DLL_FUNC __stdcall
//#else
#define DLL_FUNC
//#endif

#include <stdio.h>

void * DLL_FUNC jpl_init_ephemeris( const char *ephemeris_filename,
char nam[][6], double *val);
void DLL_FUNC jpl_close_ephemeris( void *ephem);
int DLL_FUNC jpl_state( void *ephem, const double et, const int list[12],
double pv[][6], double nut[4], const int bary);
int DLL_FUNC jpl_pleph( void *ephem, const double et, const int ntarg,
const int ncent, double rrd[], const int calc_velocity);
double DLL_FUNC jpl_get_double( const void *ephem, const int value);
double DLL_FUNC jpl_get_long( const void *ephem, const int value);
int DLL_FUNC make_sub_ephem( const void *ephem, const char *sub_filename,
const double start_jd, const double end_jd);

/* Following are constants used in          */
/* jpl_get_double( ) and jpl_get_long( ):   */

#define JPL_EPHEM_START_JD               0
#define JPL_EPHEM_END_JD                 8
#define JPL_EPHEM_STEP                  16
#define JPL_EPHEM_N_CONSTANTS           24
#define JPL_EPHEM_AU_IN_KM              28
#define JPL_EPHEM_EARTH_MOON_RATIO      36
#define JPL_EPHEM_EPHEMERIS_VERSION    200
#define JPL_EPHEM_KERNEL_SIZE          204
#define JPL_EPHEM_KERNEL_RECORD_SIZE   208
#define JPL_EPHEM_KERNEL_NCOEFF        212
#define JPL_EPHEM_KERNEL_SWAP_BYTES    216

/* Right now,  DEs 403 and 405 have the maximum kernel size,  of 2036.    */
/* This value may need to be updated the next time JPL releases a new DE: */

#define MAX_KERNEL_SIZE 2036

/***** THERE IS NO NEED TO MODIFY THE REST OF THIS SOURCE (I hope) *********/


/* A JPL binary ephemeris header contains five doubles and */
/* (up to) 41 long integers,  so:                          */
#define JPL_HEADER_SIZE (5 * sizeof( double) + 41 * sizeof( int32_t))

#pragma pack(1)

struct jpl_eph_data {
	double ephem_start, ephem_end, ephem_step;
	int32_t ncon;
	double au;
	double emrat;
	int32_t ipt[13][3];
	int32_t ephemeris_version;
	int32_t kernel_size, recsize, ncoeff;
	int32_t swap_bytes;
	int32_t curr_cache_loc;
	double pvsun[6];
	double *cache;
	void *iinfo;
	FILE *ifile;
};

struct interpolation_info
{
	double pc[18],vc[18], twot;
	int np, nv;
};

#pragma pack()

#endif
