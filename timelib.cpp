/*! \file timelib.c
	\brief Time handling library source file
*/

#include "timelib.h"
#include "convertlib.h"

extern char resdir[MAXDIR];

typedef struct
{
	uint32_t mjd;
	double pmx;
	double pmy;
	double dutc;
	uint32_t ls;
} iersstruc;

static vector<iersstruc> iers;
static uint32_t iersbase=0;

#define MAXLEAPS 26
double leaps[MAXLEAPS] =
{41370.,41498.,41682.,42047.,42412.,42777.,43143.,43508.,43873.,44238.,44785.,45150.,45515.,46246.,47160.,47891.,48256.,48803.,49168.,49533.,50082.,50629.,51178.,53735.,54831.,56108.};

//! \addtogroup timelib_functions
//! @{

//! Current UTC in Modified Julian Days
/*!
	\param offset MJD offset to be apllied to turn actual time in to
	simulated time.
	\return simulated time in Modified Julian Days.
*/
double currentmjd(double offset)
{
	struct timeval mytime;
//	struct tm *mytm;
//	time_t thetime;
	double mjd;

	gettimeofday(&mytime, NULL);
	/*
	thetime = mytime.tv_sec;
	mytm = gmtime(&thetime);
	mjd = cal2mjd2(mytm->tm_year+1900,mytm->tm_mon+1,mytm->tm_mday);
	mjd += ((mytm->tm_hour + (mytm->tm_min + (mytm->tm_sec + mytime.tv_usec / 1000000.) / 60.) / 60.) / 24.);
	*/
	mjd = unix2utc(mytime);

	return mjd+offset;
}

//! Unix time to UTC
/*! Convert Unix time in a timeval structure to a double precision number representing Mofidied Julian Day.
 * \param unixtime Unix time to be converted.
 * \return UTC as Modified Julian day.
 */
double unix2utc(struct timeval unixtime)
{
	double utc;
	struct tm *mytm;
	time_t thetime;

	thetime = unixtime.tv_sec;
	mytm = gmtime(&thetime);
	utc = cal2mjd2(mytm->tm_year+1900,mytm->tm_mon+1,mytm->tm_mday);
	utc += ((mytm->tm_hour + (mytm->tm_min + (mytm->tm_sec + unixtime.tv_usec / 1000000.) / 60.) / 60.) / 24.);

	return utc;
}

//! UTC to Unix time
/*! Convert UTC in Modified Julian Day to a timeval structure representing Unix time.
 * \param UTC as Modified Julian Day.
 * \return Timeval structure with Unix time.
 */
struct timeval utc2unix(double utc)
{
	struct timeval unixtime;
	struct tm unixtm;
	double day, doy, fd;

	mjd2ymd(utc, &unixtm.tm_year, &unixtm.tm_mon, &day, &doy);
	unixtm.tm_year -= 1900;
	unixtm.tm_mon -= 1;
	unixtm.tm_mday = day;
	fd = fmod(utc, 1.);
	unixtm.tm_hour = fd * 24.;
	fd -= unixtm.tm_hour / 24.;
	unixtm.tm_min = fd * 1440.;
	fd -= unixtm.tm_min / 1440.;
	unixtm.tm_sec = fd * 86400.;
	fd -= unixtm.tm_sec / 86400.;
	unixtime.tv_sec = mktime(&unixtm);
	unixtime.tv_usec = fd * 1000000.;

	return unixtime;
}


//! MJD to Year, Month, and Decimal Day
/*! Convert Modified Julian Day to Calendar Year, Month, Decimal Day.
	\param mjd Modified Julian Day
	\param year Pointer to return Calendar Year
	\param month Pointer to return Calendar Month
	\param day Pointer to return Decimal Day of the Month
	\param doy Pointer to return Decimal Day of the Year
	\return 0, otherwise negative error
*/
int32_t mjd2ymd(double mjd, int32_t *year, int32_t *month, double *day, double *doy)
{
	int32_t a, b, c, d, e, z, alpha;
	double f;

	mjd += 2400001.;
	z = (int32_t)mjd;
	f = mjd - z;

	if (z<2299161)
		a = z;
	else
	{
		alpha = (int32_t)((z - 1867216.25)/36524.25);
		a = z +1 + alpha - (int32_t)(alpha/4);
	}

	b = a + 1524;
	c = (int32_t)((b - 122.1)/365.25);
	d = (int32_t)(365.25*c);
	e = (int32_t)((b - d)/30.6001);

	*day = b - d - (int32_t)(30.6001 * e) + f;
	if (e < 14)
		*month = e - 1;
	else
		*month = e - 13;
	if (*month > 2)
		*year = c - 4716;
	else
		*year = c - 4715;
	*doy = (int32_t)((275 * *month)/9) - (2-isleap(*year))*(int32_t)((*month+9)/12) + *day - 30;
	return (0);
}

void mjd2cal( double djm, int *iy, int *im, int *id, double *fd, int *j)
/*
**  - - - - - - - -
**   s l a D j c l
**  - - - - - - - -
**
**  Modified Julian Day to Gregorian year, month, day,
**  and fraction of a day.
**
**  Given:
**     djm      double     Modified Julian Day (JD-2400000.5)
**
**  Returned:
**     *iy      int        year
**     *im      int        month
**     *id      int        day
**     *fd      double     fraction of day
**     *j       int        status:
**                      -1 = unacceptable date (before 4701BC March 1)
**
**  The algorithm is derived from that of Hatcher 1984 (QJRAS 25, 53-55).
**
**  Last revision:   12 March 1998
**
**  Copyright P.T.Wallace.  All rights reserved.
*/
{
	double f, d;
	long jd, n4, nd10;

	/* Check if date is acceptable */
	if ( ( djm <= -2395520.0 ) || ( djm >= 1e9 ) ) {
		*j = -1;
		return;
	} else {
		*j = 0;

		/* Separate day and fraction */
		f = fmod ( djm, 1.0 );
		if ( f < 0.0 ) f += 1.0;
		d = djm - f;
		d = round ( d );

		/* Express day in Gregorian calendar */
		jd = (long) round ( d ) + 2400001;
		n4 = 4L*(jd+((6L*((4L*jd-17918L)/146097L))/4L+1L)/2L-37L);
		nd10 = 10L*(((n4-237L)%1461L)/4L)+5L;
		*iy = (int) (n4/1461L-4712L);
		*im = (int) (((nd10/306L+2L)%12L)+1L);
		*id = (int) ((nd10%306L)/10L+1L);
		*fd = f;
		*j = 0;
	}
}

double cal2mjd2(int32_t year, int32_t month, double day)
{
	double mjd;
	int32_t a, b;

	if (year < -4712)
		return (0.);

	switch (month)
	{
	case 1:
	case 2:
		year--;
		month += 12;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		a = (int32_t)(year / 100);
		if ((year > 1582) || (year == 1582 && month > 10) || (year == 1582 && month == 10 && day > 4))
			b = 2 - a + (int32_t)(a/4);
		else
			b = 0;
		break;
	default:
		return (0.);
	}
	mjd = (int32_t)(365.25 * (year+4716)) + (int32_t)(30.6001*(month+1)) + day + b - 2401525.;
	return (mjd);
}

void cal2mjd( int iy, int im, int id, double *djm, int *j )
/*
**  - - - - - - - -
**   s l a C l d j
**  - - - - - - - -
**
**  Gregorian calendar to Modified Julian Day.
**
**  Given:
**     iy,im,id     int    year, month, day in Gregorian calendar
**
**  Returned:
**     *djm         double Modified Julian Day (JD-2400000.5) for 0 hrs
**     *j           int    status:
**                           0 = OK
**                           1 = bad year   (MJD not computed)
**                           2 = bad month  (MJD not computed)
**                           3 = bad day    (MJD computed)
**
**  The year must be -4699 (i.e. 4700BC) or later.
**
**  The algorithm is derived from that of Hatcher 1984 (QJRAS 25, 53-55).
**
**  Last revision:   29 August 1994
**
**  Copyright P.T.Wallace.  All rights reserved.
*/
{
	long iyL, imL;

	/* Month lengths in days */
	static int mtab[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };



	/* Validate year */
	if ( iy < -4699 ) { *j = 1; return; }

	/* Validate month */
	if ( ( im < 1 ) || ( im > 12 ) ) { *j = 2; return; }

	/* Allow for leap year */
	mtab[1] = ( ( ( iy % 4 ) == 0 ) &&
				( ( ( iy % 100 ) != 0 ) || ( ( iy % 400 ) == 0 ) ) ) ?
				  29 : 28;

	/* Validate day */
	*j = ( id < 1 || id > mtab[im-1] ) ? 3 : 0;

	/* Lengthen year and month numbers to avoid overflow */
	iyL = (long) iy;
	imL = (long) im;

	/* Perform the conversion */
	*djm = (double)
		   ( ( 1461L * ( iyL - ( 12L - imL ) / 10L + 4712L ) ) / 4L
			 + ( 306L * ( ( imL + 9L ) % 12L ) + 5L ) / 10L
			 - ( 3L * ( ( iyL - ( 12L - imL ) / 10L + 4900L ) / 100L ) ) / 4L
			 + (long) id - 2399904L );
}

//! Julian Century
/*! Caculate the number of centuries since J2000 for the provided date.
	\param mjd Date in Modified Julian Day.
	\return Julian century in decimal form.
*/
double utc2jcen(double mjd)
{
	static double lmjd=0.;
	static double lcalc=0.;

	if (mjd != lmjd)
	{
		lcalc = (utc2tt(mjd)-51544.5) / 36525.;
		lmjd = mjd;
	}
	return (lcalc);
}

//! Nutation values
/*! Calculate the nutation values from the JPL Ephemeris for the provided UTC date.
 * Values are in order: Psi, Epsilon, dPsi, dEpsilon. Units are radians and
 * radians/second.
	\param mjd UTC in Modified Julian Day.
	\return Nutation values in an ::rvector.
*/
rvector utc2nuts(double mjd)
{
	static double lmjd=0.;
	static uvector lcalc={{{0.,0.,0.},0.}};

	if (mjd != lmjd)
	{
		jplnut(utc2tt(mjd),(double *)&lcalc.a4);
		lmjd = mjd;
	}
	return (lcalc.r);
}

//! Nutation Delta Psi value.
/*! Calculate the Delta Psi value (nutation in longitude), for use in the Nutation
	matrix, for the provided UTC date.
	\param mjd UTC in Modified Julian Day.
	\return Delta Psi in radians.
*/
double utc2dpsi(double mjd)
{
	static double lmjd=0.;
	static double lcalc=0.;
	rvector nuts;

	if (mjd != lmjd)
	{
		nuts = utc2nuts(mjd);
		lcalc = nuts.col[0];
		lmjd = mjd;
	}
	return (lcalc);
}

//! Nutation Delta Epsilon value.
/*! Calculate the Delta Psi value (nutation in obliquity), for use in the Nutation
	matrix, for the provided UTC date.
	\param mjd UTC in Modified Julian Day.
	\return Delta Psi in radians.longitudilon
*/
double utc2depsilon(double mjd)
{
	static double lmjd=0.;
	static double lcalc=0.;
	rvector nuts;

	if (mjd != lmjd)
	{
		nuts = utc2nuts(mjd);
		lcalc = nuts.col[1];
		lmjd = mjd;
	}
	return (lcalc);
}

//! Nutation Epsilon value.
/*! Calculate the Epsilon value (obliquity of the ecliptic), for use in the Nutation
	matrix, for the provided UTC date.
	\param mjd UTC in Modified Julian Day.
	\return Epsilon in radians.
*/
double utc2epsilon(double mjd)
{
	static double lmjd=0.;
	static double lcalc=0.;
	double jcen;

	if (mjd != lmjd)
	{
		jcen = utc2jcen(mjd);
		lcalc = DAS2R*(84381.406+jcen*(-46.836769+jcen*(-.0001831+jcen*(0.0020034+jcen*(-0.000000576+jcen*(-0.0000000434))))));
		lcalc = ranrm(lcalc);
		lmjd = mjd;
	}
	return (lcalc);
}

//! Nutation L value.
/*! Calculate the L value,  for use in the Nutation matrix, for the provided UTC date.
	\param mjd UTC in Modified Julian Day.
	\return L in radians.
*/
double utc2L(double mjd)
{
	static double lmjd=0.;
	static double lcalc=0.;
	double jcen;

	if (mjd != lmjd)
	{
		jcen = utc2jcen(mjd);
		lcalc = DAS2R*(485868.249036+jcen*(1717915923.2178+jcen*(31.8792+jcen*(.051635+jcen*(-.0002447)))));
		lcalc = ranrm(lcalc);
		lmjd = mjd;
	}
	return (lcalc);
}

//! Nutation L prime value.
/*! Calculate the L prime value,  for use in the Nutation matrix, for the provided UTC date.
	\param mjd UTC in Modified Julian Day.
	\return L prime in radians.
*/
double utc2Lp(double mjd)
{
	static double lmjd=0.;
	static double lcalc=0.;
	double jcen;

	if (mjd != lmjd)
	{
		jcen = utc2jcen(mjd);
		lcalc = DAS2R*(1287104.79305+jcen*(129596581.0481+jcen*(-.5532+jcen*(.000136+jcen*(-.00001149)))));
		lcalc = ranrm(lcalc);
		lmjd = mjd;
	}
	return (lcalc);
}

//! Nutation F value.
/*! Calculate the F value,  for use in the Nutation matrix, for the provided UTC date.
	\param mjd UTC in Modified Julian Day.
	\return F in radians.
*/
double utc2F(double mjd)
{
	static double lmjd=0.;
	static double lcalc=0.;
	double jcen;

	if (mjd != lmjd)
	{
		jcen = utc2jcen(mjd);
		lcalc = DAS2R*(335779.526232+jcen*(1739527262.8478+jcen*(-12.7512+jcen*(-.001037+jcen*(.00000417)))));
		lcalc = ranrm(lcalc);
		lmjd = mjd;
	}
	return (lcalc);
}

//! Nutation D value.
/*! Calculate the D value,  for use in the Nutation matrix, for the provided UTC date.
	\param mjd UTC in Modified Julian Day.
	\return D in radians.
*/
double utc2D(double mjd)
{
	static double lmjd=0.;
	static double lcalc=0.;
	double jcen;

	if (mjd != lmjd)
	{
		jcen = utc2jcen(mjd);
		lcalc = DAS2R*(1072260.70369+jcen*(1602961601.209+jcen*(-6.3706+jcen*(.006593+jcen*(-.00003169)))));
		lcalc = ranrm(lcalc);
		lmjd = mjd;
	}
	return (lcalc);
}

//! Nutation omega value.
/*! Calculate the omega value,  for use in the Nutation matrix, for the provided UTC date.
	\param mjd UTC in Modified Julian Day.
	\return Omega in radians.
*/
double utc2omega(double mjd)
{
	static double lmjd=0.;
	static double lcalc=0.;
	double jcen;

	if (mjd != lmjd)
	{
		jcen = utc2jcen(mjd);
		lcalc = DAS2R*(450160.398036+jcen*(-6962890.5431+jcen*(7.4722+jcen*(.007702+jcen*(-.00005939)))));
		lcalc = ranrm(lcalc);
		lmjd = mjd;
	}
	return (lcalc);
}

//! Calculate DUT1
/*! Calculate DUT1 = UT1 - UTC, based on lookup in IERS Bulletin A.
	\param mjd UTC in Modified Julian Day.
	\return DUT1 in Modified Julian Day, otherwise 0.
*/
double utc2dut1(double mjd)
{
	static double lmjd=0.;
	static double lcalc = 0.;
	double frac;
//	uint32_t mjdi;
	uint32_t iersidx;

	if (mjd != lmjd)
	{
		if (load_iers() && iers.size() > 2)
		{
			if ((uint32_t)mjd >= iersbase)
			{
				if ((iersidx=(uint32_t)mjd-iersbase) > iers.size())
				{
					iersidx = iers.size() - 2;
				}
			}
			else
			{
				iersidx = 0;
			}
//			mjdi = (uint32_t)mjd;
			frac = mjd - (uint32_t)mjd;
			lcalc = ((frac*iers[1+iersidx].dutc+(1.-frac)*iers[iersidx].dutc)/86400.);
			lmjd = mjd;
		}
		else
			lcalc = 0.;
	}
	return (lcalc);
}

//! Convert UTC to UT1
/*! Convert Coordinated Universal Time to Universal Time by correcting for the offset
	* between them at the given time. Table of DUT1 is first initialized from disk if it
	* hasn't yet been.
	\param mjd UTC in Modified Julian Day.
	\return UTC1 in Modified Julian Day, otherwise 0.
*/
double utc2ut1(double mjd) 
{
	static double lmjd=0.;
	static double lut=0.;

	if (mjd != lmjd)
	{
		if (load_iers())
		{
			lut = mjd + utc2dut1(mjd);
			lmjd = mjd;
		}
		else
			lut = 0.;
	}
	return (lut);
}

//! Julian Century.
/*! Convert time supplied in Modified Julian Day to the Julian Century.
	\param mjd Time in Modified Julian Day.
	\return Decimal century.
*/
double julcen(double mjd)
{
	return ((mjd - 51544.5) / 36525.);
}

//! Convert UTC to TDB.
/*! Convert Coordinated Universal Time to Barycentric Dynamical Time by correcting for
 * the mean variations as a function of Julian days since 4713 BC Jan 1.5.
 \param mjd UTC in Modified Julian Day.
 \return TDB in Modified Julian Day, otherwise 0.
*/
double utc2tdb(double mjd)
{
	static double lmjd=0.;
	static double ltdb=0.;
	double tt, g;

	if (mjd != lmjd)
	{
		tt = utc2tt(mjd);
		g = 6.2400756746 + .0172019703436*(mjd-51544.5);
		ltdb = (tt + (.001658*sin(g)+.000014*sin(2*g))/86400.);
		lmjd = mjd;
	}
	return (ltdb);
}

//! Convert TT to UTC.
/*! Convert Terrestrial Dynamical Time to Coordinated Universal Time by correcting for
 * the appropriate number of Leap Seconds. Leap Second table is first initialized
 * from disk if it hasn't yet been.
 \param mjd TT in Modified Julian Day.
 \return UTC in Modified Julian Day, otherwise 0.
*/
double tt2utc(double mjd)
{
	uint32_t iersidx;

	if (load_iers() && iers.size() > 1)
	{
		if ((uint32_t)mjd >= iersbase)
		{
			if ((iersidx=(uint32_t)mjd-iersbase) > iers.size())
			{
				iersidx = iers.size() - 1;
			}
		}
		else
		{
			iersidx = 0;
		}
		return (mjd-(32.184+iers[iersidx].ls)/86400.);
	}
	else
		return (0.);
}

//! Convert UTC to TT.
/*! Convert Coordinated Universal Time to Terrestrial Dynamical Time by correcting for
 * the appropriate number of Leap Seconds. Leap Second table is first initialized from
 * disk if it hasn't yet been.
 \param mjd UTC in Modified Julian Day.
 \return TT in Modified Julian Day, otherwise 0.
*/
double utc2tt(double mjd)
{
	static double lmjd=0.;
	static double ltt=0.;
	uint32_t iersidx=0;

	if (mjd != lmjd)
	{
		if (load_iers() && iers.size() > 1)
		{
			if ((uint32_t)mjd >= iersbase)
			{
				if ((iersidx=(uint32_t)mjd-iersbase) > iers.size())
				{
					iersidx = iers.size() - 1;
				}
			}
			else
			{
				iersidx = 0;
			}
			ltt = (mjd+(32.184+iers[iersidx].ls)/86400.);
			lmjd = mjd;
			return (ltt);
		}
		else
		{
			return (0.);
		}
	}
	return (ltt);
}

//! Convert UTC to GPS
/*! Convert Coordinated Universal Time to GPS Time, correcting for the appropriate
 * number of leap seconds. Leap Second table is first initialized from
 * disk if it hasn't yet been.
 * \param utc UTC expressed in Modified Julian Days
 * \return GPS Time expressed in Modified Julian Days, otherwise 0.
 */
double utc2gps(double utc)
{
	double gps;

	if ((gps=utc2tt(utc)) == 0.)
	{
		return (0.);
	}

	gps -= 51.184 / 86400.;

	return (gps);
}

//! Convert GPS to UTC
/*! Convert GPS Time to Coordinated Universal Time, correcting for the appropriate
 * number of leap seconds. Leap Second table is first initialized from
 * disk if it hasn't yet been.
 * \param utc GPS Time expressed in Modified Julian Days
 * \return UTC expressed in Modified Julian Days, otherwise 0.
 */
double gps2utc(double gps)
{
	double utc;

	gps += 51.184 / 86400.;

	if ((utc=tt2utc(gps)) == 0.)
	{
		return (0.);
	}

	return (utc);
}

//! GPS Weeks and Seconds from GPS time
/*! Calculate the appropriate GPS week and remaining seconds from the provided
 * GPS time.
 * \param gps GPS time expressed as Modified Julian Day
 * \param week Pointer to the returned GPS week.
 * \param seconds Ponter to the returned GPS seconds.
 */
void gps2week(double gps, uint32_t& week, double& seconds)
{
	double elapsed;

	elapsed = gps - 44244.;
	week = (uint32_t)(elapsed / 7.);
	seconds = 86400. * (elapsed - week * 7.);
//	*week %= 1024;
}

//! GPS Time from GPS Week and Seconds
/*! Calculate the appropriate GPS Time from the provided GPS Week and Seconds.
 * \param week GPS Week.
 * \param seconds GPS Seconds.
 * \return GPS Time as Modified Julian Day.
 */
double week2gps(uint32_t week, double seconds)
{
	double elapsed;

	elapsed = 44244. + week * 7. + seconds / 86400.;

	return elapsed;
}

//! Year from MJD
/*! Return the Decimal Year for the provided MJD
	\param mjd Modified Julian Data
	\return Decimal year.
*/
double mjd2year(double mjd)
{
	double day, doy, dyear;
	int32_t month, year;

	mjd2ymd(mjd,&year,&month,&day,&doy);
	dyear = year + (doy-1) / (365.+isleap(year));
	return (dyear);
}

//! Earth RotationA Angle
/*! Calculate the Earth RotationA Angle for a given Earth RotationA Time based on the
 * provided UTC.
	\param mjd Coordinated Universal Time as Modified Julian Day.
	\return Earth RotationA Angle, theta, in radians.
*/
double utc2theta(double mjd)
{
	static double lmjd=0.;
	static double ltheta=0.;
	double ut1;

	if (mjd != lmjd)
	{
		ut1 = utc2ut1(mjd);
		ltheta = D2PI * (.779057273264 + 1.00273781191135448 * (ut1 - 51544.5));
		ltheta = ranrm(ltheta);
	}

	return (ltheta);
}

//! UTC to GAST
/*! Convert current UTC to Greenwhich Apparent Sidereal Time. Accounts for nutations.
	\param mjd UTC as Modified Julian Day
	\return GAST as Modified Julian Day
*/
double utc2gast(double mjd)
{
	static double lmjd=0.;
	static double lgast=0.;
	double omega, F, D;

	if (mjd != lmjd)
	{
		omega = utc2omega(mjd);
		F = utc2F(mjd);
		D = utc2D(mjd);
		lgast = utc2gmst(mjd) + utc2dpsi(mjd) * cos(utc2epsilon(mjd));
		lgast += DAS2R * .00264096 * sin(omega);
		lgast += DAS2R * .00006352 * sin(2.*omega);
		lgast += DAS2R * .00001175 * sin(2.*F - 2.*D + 3.*omega);
		lgast += DAS2R * .00001121 * sin(2.*F - 2.*D + omega);
		lgast = ranrm(lgast);
		lmjd = mjd;
	}
	return (lgast);
}

//! UTC (Modified Julian Day) to GMST
/*! Convert current UT to Greenwhich Mean Sidereal Time
	\param mjd UT as Modified Julian Day
	\return GMST as radians
*/
double utc2gmst(double mjd)
{
	static double lmjd=0.;
	static double lcalc=0.;
	double jcen;

	if (mjd != lmjd)
	{
		jcen = utc2jcen(mjd);
		lcalc = utc2theta(mjd) + DS2R*(.014506+jcen*(4612.156534+jcen*(1.3915817+jcen*(-.00000044+jcen*(-.000029956+jcen*(-.0000000368))))))/15.;
		lcalc = ranrm(lcalc);
		lmjd = mjd;
	}

	return (lcalc);
}

double ranrm(double angle)
{
	double result;

	result = fmod(angle,D2PI);

	return (result >= 0.)?result:result+D2PI;
}
//! Check for Leap year
/*! Check whether the specified year (Gregorian or Julian calendar) is a Leap year.
	\param year Year to check
	\return 0 if not a Leap year, 1 if a Leap year
*/
int16_t isleap(int32_t year)
{
	if (!(year % 4))
	{
		if (!(year%100))
		{
			if (!(year%400))
				return (1);
		}
	}
	return (0);
}

//! Load IERS Polar Motion, UT1-UTC, Leap Seconds.
/*! Allocate and load array for storing data from IERS file. Each
 * record includes the MJD, the Polar Motion for X and Y in radians,
 * UT1-UTC in seconds of time, and the number of Leap Seconds since
 * the creation of TAI in 1958. The following are then defined:
 * - TT = TAI +32.184
 * - TAI = UTC + Leap_Seconds
 * - UT1 = UTC + (UT1-UTC)
 \return Number of records.
*/
int32_t load_iers()
{
	char fname[200];
	FILE *fdes;
	iersstruc tiers;

	if (iers.size() == 0)
	{
		sprintf(fname,"%s/general/iers_pm_dut_ls.txt",get_resdir());
		if ((fdes=fopen(fname,"r")) != NULL)
		{
			while (fgets(fname,100,fdes))
			{
				sscanf(fname,"%u %lg %lg %lg %u",&tiers.mjd,&tiers.pmx,&tiers.pmy,&tiers.dutc,&tiers.ls);
				iers.push_back(tiers);
			}
			fclose(fdes);
		}
		if (iers.size())
			iersbase = iers[0].mjd;
	}
	return (iers.size());
}

//! Leap Seconds
/*! Returns number of leap seconds for provided Modified Julian Day.
	\param mjd Provided time.
	\return Number of leap seconds, or 0.
*/
int32_t leap_seconds(double mjd)
{
	uint32_t iersidx;

	if (load_iers() && iers.size() > 1)
	{
		if ((uint32_t)mjd >= iersbase)
		{
			if ((iersidx=(uint32_t)mjd-iersbase) > iers.size())
			{
				iersidx = iers.size() - 1;
			}
		}
		else
		{
			iersidx = 0;
		}
		return (iers[iersidx].ls);
	}
	else
		return (0);
}

//! Polar motion
/*! Returns polar motion in radians for provided Modified Julian Day.
	\param mjd Provided time.
	\return Polar motion in ::rvector.
*/
cvector polar_motion(double mjd)
{
	cvector pm;
	double frac;
//	uint32_t mjdi;
	uint32_t iersidx;

	pm = cv_zero();
	if (load_iers() && iers.size() > 2)
	{
		if ((uint32_t)mjd >= iersbase)
		{
			if ((iersidx=(uint32_t)mjd-iersbase) > iers.size())
			{
				iersidx = iers.size() - 2;
			}
		}
		else
		{
			iersidx = 0;
		}
//		mjdi = (uint32_t)mjd;
		frac = mjd - (uint32_t)mjd;
		pm = cv_zero();
		pm.x = frac*iers[1+iersidx].pmx+(1.-frac)*iers[iersidx].pmx;
		pm.y = frac*iers[1+iersidx].pmy+(1.-frac)*iers[iersidx].pmy;
	}

	return (pm);
}

//!  Timeval subtraction
/*! Determines the difference between two timeval structures 
	 \param x,y timeval structure with members long int tv_sec, tv_usec
	 \param result timeval structure difference between x and y
	 \return 1 if result is negative
*/

int timeval_subtract (struct timeval* result, struct timeval* x, struct timeval* y)
{
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait.
	 tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

//! Elapsed time 
/*! Calculates the difference in milliseconds between two timeval structures 
	 \param a,b timeval structure with members long int tv_sec, tv_usec
	 \param dif timeval structure difference between a and b
	 \return elapsed time in milliseconds
*/

float elapsed_time(struct timeval a,struct timeval b)
{
	struct timeval dif;

	/*Subtract the timevals*/
	timeval_subtract(&dif,&a,&b);

	/* Calculate and return difference in milliseconds */
	return -dif.tv_sec*1000.0-dif.tv_usec/1000.0;
}

//! ISO 8601 version of time
/*! Represent the given UTC as an extended calendar format ISO 8601
 * string in the format:
 * YYYY-MM-DDTHH:MM:SS
 * \param utc Coordinated Universal Time expressed in Modified Julian Days.
 * \return C++ String containing the ISO 8601 date.
 */
string utc2iso8601(double utc)
{
	char answer[25];
	int iy, im, id, j, ihh, imm, iss;
	double fd;

	mjd2cal(utc, &iy, &im, &id, &fd, &j);
	ihh = (int)(24 * fd);
	fd -= ihh / 24.;
	imm = (int)(1440 * fd);
	fd -= imm / 1440.;
	iss = (int)(86400 * fd);
	sprintf(answer, "%04d-%02d-%02dT%02d:%02d:%02d", iy, im, id, ihh, imm, iss);

	string sanswer = answer;
	return sanswer;
}

//! @}
