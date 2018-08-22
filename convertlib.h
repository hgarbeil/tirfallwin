/*! \file convertlib.h
	\brief convertlib include file
	A library providing functions for conversion between the various
	position and attitude coordinate systems used in orbital mechanics.
*/

#ifndef _CONVERTLIB_H
#define _CONVERTLIB_H 1

#include "configCosmos.h"

#include "datalib.h"
#include "mathlib.h"
#include "jpleph.h"
#include "timelib.h"
#include "memlib.h"
#include "convertdef.h"
//#include "demlib.h"
#include "geomag.h"

//#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <cstring>

//! \ingroup conversion
//! \defgroup conversion_functions Coordinate conversion functions
//! @{

void loc_clear(locstruc *loc);
void jplnut(double mjd, double nuts[]);
void jplpos(long from, long to, double mjd, cartpos *pos);
void jpllib(double utc,rmatrix *rm, rmatrix *drm);
//void jpllib(double utc, aattstruc *pos);
void geoc2geod(cartpos *geoc, geoidpos *geod);
void geos2geoc(spherpos *geos, cartpos *geoc);
void geod2geoc(geoidpos *geod, cartpos *geoc);
void geoc2geos(cartpos *geoc, spherpos *geos);
void selg2selc(geoidpos *selg, cartpos *selc);
void pos_extra(locstruc *loc);
void pos_clear(locstruc *loc);
void pos_baryc(locstruc *loc);
void pos_eci(locstruc *loc);
void pos_sci(locstruc *loc);
void pos_geoc(locstruc *loc);
void pos_geos(locstruc *loc);
void pos_geod(locstruc *loc);
void pos_selc(locstruc *loc);
void pos_selg(locstruc *loc);
void pos_baryc2eci(locstruc *loc);
void pos_eci2baryc(locstruc *loc);
void pos_baryc2sci(locstruc *loc);
void pos_sci2baryc(locstruc *loc);
void pos_eci2geoc(locstruc *loc);
void pos_eci2selc(locstruc *loc);
void pos_geoc2eci(locstruc *loc);
void pos_geoc2geod(locstruc *loc);
void pos_geod2geoc(locstruc *loc);
void pos_geoc2geos(locstruc *loc);
void pos_geos2geoc(locstruc *loc);
void pos_eci2sci(locstruc *loc);
void pos_sci2eci(locstruc *loc);
void pos_sci2selc(locstruc *loc);
void pos_selc2selg(locstruc *loc);
void pos_selc2sci(locstruc *loc);
void pos_selg2selc(locstruc *loc);
void pos_selc2eci(locstruc *loc);
void eci2kep(cartpos *eci,kepstruc *kep);
void kep2eci(kepstruc *kep,cartpos *eci);
double rearth(double lat);
double mjd2year(double mjd);
void att_extra(locstruc *loc);
void att_clear(locstruc *loc);
void att_icrf(locstruc *loc);
void att_lvlh(locstruc *loc);
void att_geoc(locstruc *loc);
void att_selc(locstruc *loc);
void att_topo(locstruc *loc);
void att_planec2topo(locstruc *loc);
void att_topo2planec(locstruc *loc);
void att_icrf2geoc(locstruc *loc);
void att_icrf2lvlh(locstruc *loc);
void att_icrf2selc(locstruc *loc);
void att_geoc2icrf(locstruc *loc);
void att_planec2lvlh(locstruc *loc);
void att_lvlh2planec(locstruc *loc);
void att_lvlh2icrf(locstruc *loc);
void att_selc2icrf(locstruc *loc);
void loc_update(locstruc *loc);
double mjd2gmst(double mjd);
void euler2dcm( char *order, double phi, double theta, double psi, double rmat[3][3] );
void icrs2itrs(double utc, rmatrix *rnp, rmatrix *rm, rmatrix *drm, rmatrix *ddrm);
void itrs2icrs(double utc, rmatrix *rnp, rmatrix *rm, rmatrix *drm, rmatrix *ddrm);
void true2mean(double ep1, rmatrix *pm);
void mean2true(double ep0, rmatrix *pm);
void icrs2mean(double ep1, rmatrix *pm);
void mean2icrs(double ep0, rmatrix *pm);
void mean2mean(double ep0, double ep1, rmatrix *pm);
void geoc2topo(gvector gs, rvector geoc, rvector *topo);
void topo2azel(rvector tpos, float *az, float *el);
int lines2eci(double mjd, vector<linestruc> tle, cartpos *eci);
int line2eci(double mjd, linestruc tle, cartpos *eci);
linestruc get_line(uint16_t index, vector<linestruc> tle);
/*
void cal2mjd( int iy, int im, int id, double *djm, int *j );
void mjd2cal( double djm, int *iy, int *im, int *id, double *fd, int *j);
double utc2tt(double mjd);
double utc2gps(double utc);
double mjd2gmst(double mjd);
*/
int32_t load_lines(char *fname, vector<linestruc>& tle);
int32_t load_stk(char *filename, stkstruc *stkdata);
int stk2eci(double utc, stkstruc *stk, cartpos *eci);

//! @}

#endif
