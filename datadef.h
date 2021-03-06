#ifndef _DATADEF_H
#define _DATADEF_H 1

/*! \file datadef.h
*	\brief Data Management support library definitions file
*/

#include "configCosmos.h"
#include <cstring>
#include <string>

//! \ingroup datalib
//! \defgroup datalib_constants Data Management support library contants
//! @{

#define DATA_LOG_TYPE_SOH 0
#define DATA_LOG_TYPE_EVENT 1

//! @}

//! \ingroup datalib
//! \defgroup datalib_typedefs Data Management support library type definitions
//! @{
typedef struct
{
	string node;
	string agent;
	string name;
	string type;
	string path;
} filestruc;
//! @}

#endif
