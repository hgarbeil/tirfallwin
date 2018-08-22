#ifndef GEOMAG_H
#define GEOMAG_H

/*! \file geomag.h
	\brief geomag include file
	A library of routines to calculate the magnetic field of the Earth.
	\defgroup geomag Earth geomagnetic field calculation
*/

#include "configCosmos.h"

#include "convertlib.h"

void geomag_front (gvector pos, double year, rvector *comp);

#endif
