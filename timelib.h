/*! \file timelib.h
	\brief timelib include file
	A library providing functions for handling various types of time.
	\defgroup timelib Time handling library
*/

#ifndef _TIMELIB_H
#define _TIMELIB_H 1

#include "configCosmos.h"

#include "mathlib.h"
#include "convertdef.h"

#include <sys/time.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

//! \ingroup timelib
//! \defgroup timelib_constants Time handling constants
//! @{

#define MJD2JD(mjd) (double)((mjd) + 2400000.5)
#define JD2MJD(jd) (double)((jd) - 2400000.5)
//! @}

//! \ingroup timelib
//! \defgroup timelib_typedefs Time handling type definitions
//! @{
typedef struct
{
	double mjd;
	int32_t year;
	int32_t month;
	int32_t day;
	double fd;
	double tt_mjd;
	double gmst_rad;
} timestruc;

//! @}

//! \ingroup timelib
//! \defgroup timelib_functions Time handling functions
//! @{

double currentmjd(double offset);
double unix2utc(struct timeval unixtime);
struct timeval utc2unix(double utc);
double mjd2year(double mjd);
double cal2mjd2(int32_t year, int32_t month, double day);
void cal2mjd( int iy, int im, int id, double *djm, int *j );
int32_t mjd2ymd(double mjd, int32_t *year, int32_t *month, double *day, double *doy);
void mjd2cal( double djm, int *iy, int *im, int *id, double *fd, int *j);
double utc2epsilon(double mjd);
double utc2depsilon(double mjd);
double utc2dpsi(double mjd);
double utc2L(double mjd);
double utc2Lp(double mjd);
double utc2F(double mjd);
double utc2D(double mjd);
double utc2omega(double mjd);
double utc2tt(double mjd);
double utc2gps(double utc);
double gps2utc(double gps);
void gps2week(double gps, uint32_t& week, double& seconds);
double week2gps(uint32_t week, double seconds);
double utc2ut1(double mjd);
double utc2dut1(double mjd);
double utc2tdb(double mjd);
double utc2tdb(double mjd);
double utc2gmst(double mjd);
double utc2gast(double mjd);
rvector utc2nuts(double mjd);
double utc2theta(double mjd);
double utc2jcen(double mjd);
double tt2utc(double mjd);
double tt2tdb(double mjd);
string utc2iso8601(double mjd);
double julcen(double mjd);
cvector polar_motion(double mjd);
int32_t leap_seconds(double mjd);
double ranrm(double angle);
int16_t isleap(int32_t year);
int32_t load_iers();
int timeval_subtract (struct timeval* result, struct timeval* x, struct timeval* y);
float elapsed_time(struct timeval a,struct timeval b);
//! @}

#endif
