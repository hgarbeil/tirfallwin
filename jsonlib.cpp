/*! \file jsonlib.c
	\brief JSON support source file
*/
#include "jsonlib.h"

#include<iostream>
#include <limits>
using namespace std;

static char tbuf[JSON_MAX_DATA];

char comp_type_string[DEVICE_TYPE_COUNT][10]={
	"pload",
	"ssen",
	"imu",
	"rw",
	"mtr",
	"cpu",
	"gps",
	"ant",
	"rxr",
	"txr",
	"tcv",
	"strg",
	"batt",
	"htr",
	"motr",
	"tsen",
	"thst",
	"prop",
	"swch",
	"rot",
	"stt",
	"mcc",
	"tcu",
	"bus",
	"psen",
	"suchi"
};

/*! \ingroup jsonlib
* \defgroup jsonlib_namespace JSON Name Space
* @{
* A hierarchical set of variable names meant to describe everything in COSMOS. Each name
* maps to a variable of a type specified in \ref json_type. Names are restricted to
* alphabetic characters, and can be no longer than ::COSMOS_MAX_NAME. Once mapped through use
* of ::json_setup, these names will be tied to elements of the ::cosmosstruc.
*/

//! @}

//! \addtogroup jsonlib_functions JSON support library functions
//! @{

//! Initialize JSON pointer map
/*! Using the supplied ::cosmosstruc, assign storage for each of the groups and entries
 * for each of the non Node based elements to the JSON Name Map.
	\param cdata Pointer to ::cosmosstruc to be mapped.
	\return 0, or a negative ::error.
*/
cosmosstruc *json_create()
{
	cosmosstruc *cdata = NULL;
	unitstruc tunit;

	if ((cdata = new cosmosstruc[2]) == NULL) return (NULL);

	memset(cdata, 0, 2*sizeof(cosmosstruc));

	cdata->jmapped = 0;

	cdata->glossary.resize(1);
	cdata->piece.resize(1);
	cdata->device.resize(1);
//	json_clear_cosmosstruc(JSON_GROUP_NODE, cdata);
	cdata->agent.resize(1);
	cdata->event.resize(1);
	cdata->user.resize(1);
	cdata->glossary.resize(1);
	cdata->port.resize(1);
	cdata->unit.resize(JSON_UNIT_COUNT);
	cdata->target.resize(100);
//	json_clear_cosmosstruc(JSON_GROUP_TARGET, cdata);
	cdata->jmap.resize(JSON_MAX_HASH);
	cdata->emap.resize(JSON_MAX_HASH);

	if (cdata->target.size() != 100 ||
		cdata->unit.size() != JSON_UNIT_COUNT ||
		cdata->jmap.size() != JSON_MAX_HASH ||
		cdata->emap.size() != JSON_MAX_HASH ||
		cdata->piece.size() != 1 ||
		cdata->device.size() != 1 ||
		cdata->agent.size() != 1 ||
		cdata->event.size() != 1 ||
		//		cdata->physics.size() != 1 ||
//		cdata->node.size() != 1 ||
		cdata->user.size() != 1 ||
		cdata->glossary.size() != 1 ||
		cdata->port.size() != 1)
	{
		delete [] cdata;
		return (NULL);
	}

	// Create JSON Map unit table
	for (uint16_t i=0; i<cdata->unit.size(); ++i)
	{
		// SI Units
		tunit.type = JSON_UNIT_TYPE_IDENTITY;
		tunit.p0 = tunit.p1 = tunit.p2 = 0.;
		switch (i)
		{
		case JSON_UNIT_NONE:
			tunit.name = "";
			break;
		case JSON_UNIT_ACCELERATION:
			tunit.name = "m/s2";
			break;
		case JSON_UNIT_ANGLE:
			tunit.name = "rad";
			break;
		case JSON_UNIT_AREA:
			tunit.name = "m2";
			break;
		case JSON_UNIT_CAPACITANCE:
			tunit.name = "F";
			break;
		case JSON_UNIT_CHARGE:
			tunit.name = "C";
			break;
		case JSON_UNIT_CURRENT:
			tunit.name = "A";
			break;
		case JSON_UNIT_DENSITY:
			tunit.name = "kg/m3";
			break;
		case JSON_UNIT_ENERGY:
			tunit.name = "j";
			break;
		case JSON_UNIT_FORCE:
			tunit.name = "N";
			break;
		case JSON_UNIT_FREQUENCY:
			tunit.name = "Hz";
			break;
		case JSON_UNIT_INTENSITY:
			tunit.name = "Ca";
			break;
		case JSON_UNIT_ISP:
			tunit.name = "s";
			break;
		case JSON_UNIT_LENGTH:
			tunit.name = "m";
			break;
		case JSON_UNIT_LUMINANCE:
			tunit.name = "Cd/m2";
			break;
		case JSON_UNIT_MAGDENSITY:
			tunit.name = "T";
			break;
		case JSON_UNIT_MAGFIELD:
			tunit.name = "A/m";
			break;
		case JSON_UNIT_MAGFLUX:
			tunit.name = "Wb";
			break;
		case JSON_UNIT_MASS:
			tunit.name = "kg";
			break;
		case JSON_UNIT_PRESSURE:
			tunit.name = "Pa";
			break;
		case JSON_UNIT_RESISTANCE:
			tunit.name = "ohm";
			break;
		case JSON_UNIT_SOLIDANGLE:
			tunit.name = "sr";
			break;
		case JSON_UNIT_SPEED:
			tunit.name = "m/s";
			break;
		case JSON_UNIT_TEMPERATURE:
			tunit.name = "K";
			break;
		case JSON_UNIT_TIME:
			tunit.name = "s";
			break;
		case JSON_UNIT_VOLTAGE:
			tunit.name = "V";
			break;
		case JSON_UNIT_VOLUME:
			tunit.name = "m3";
			break;
		}
		cdata->unit[i].push_back(tunit);

		// Alternate Units
		tunit.type = JSON_UNIT_TYPE_IDENTITY;
		tunit.p0 = tunit.p1 = tunit.p2 = 0.;
		switch (i)
		{
		case JSON_UNIT_NONE:
			break;
		case JSON_UNIT_ACCELERATION:
			tunit.name = "g";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = 1./9.80665;
			cdata->unit[i].push_back(tunit);
			break;
		case JSON_UNIT_ANGLE:
			tunit.name = "deg";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = RTOD;
			cdata->unit[i].push_back(tunit);
			break;
		case JSON_UNIT_AREA:
			tunit.name = "ft2";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = 10.76391;
			cdata->unit[i].push_back(tunit);
			break;
		case JSON_UNIT_CAPACITANCE:
			break;
		case JSON_UNIT_CHARGE:
			break;
		case JSON_UNIT_CURRENT:
			break;
		case JSON_UNIT_DENSITY:
			break;
		case JSON_UNIT_ENERGY:
			tunit.name = "erg";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = 1e7;
			cdata->unit[i].push_back(tunit);
			break;
		case JSON_UNIT_FORCE:
			tunit.name = "dyn";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = 1e5;
			cdata->unit[i].push_back(tunit);
			tunit.name = "lb";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = .22481;
			cdata->unit[i].push_back(tunit);
			break;
		case JSON_UNIT_FREQUENCY:
			break;
		case JSON_UNIT_INTENSITY:
			break;
		case JSON_UNIT_ISP:
			break;
		case JSON_UNIT_LENGTH:
			tunit.name = "km";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = 1e-3;
			cdata->unit[i].push_back(tunit);
			tunit.name = "cm";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = 1e2;
			cdata->unit[i].push_back(tunit);
			tunit.name = "ft";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = 3.280833;
			cdata->unit[i].push_back(tunit);
			break;
		case JSON_UNIT_LUMINANCE:
			break;
		case JSON_UNIT_MAGDENSITY:
			tunit.name = "nT";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = 1e9;
			cdata->unit[i].push_back(tunit);
			break;
		case JSON_UNIT_MAGFIELD:
			break;
		case JSON_UNIT_MAGFLUX:
			break;
		case JSON_UNIT_MASS:
			tunit.name = "g";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = 1e3;
			cdata->unit[i].push_back(tunit);
			break;
		case JSON_UNIT_PRESSURE:
			break;
		case JSON_UNIT_RESISTANCE:
			break;
		case JSON_UNIT_SOLIDANGLE:
			break;
		case JSON_UNIT_SPEED:
			tunit.name = "km/s";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = 1e-3;
			cdata->unit[i].push_back(tunit);
			tunit.name = "cm/s";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = 1e2;
			cdata->unit[i].push_back(tunit);
			break;
		case JSON_UNIT_TEMPERATURE:
			tunit.name = "\260C";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p0 = -273.15;
			tunit.p1 = 1.;
			cdata->unit[i].push_back(tunit);
			break;
		case JSON_UNIT_TIME:
			tunit.name = "min";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = 1./60.;
			cdata->unit[i].push_back(tunit);
			tunit.name = "hr";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = 1./3600.;
			cdata->unit[i].push_back(tunit);
			tunit.name = "day";
			tunit.type = JSON_UNIT_TYPE_POLY;
			tunit.p1 = 1./86400.;
			cdata->unit[i].push_back(tunit);
			break;
		case JSON_UNIT_VOLTAGE:
			break;
		case JSON_UNIT_VOLUME:
			break;
		}
	}

	// Here is where we add entries for all the single element names.
	json_addbaseentry(cdata);

	return (cdata);
}

//! Remove JSON pointer map
/*! Frees up all space assigned to JSON pointer map. Includes any space allocated
 * through ::json_addentry.
*/
void json_destroy(cosmosstruc *cdata)
{
	for (uint16_t i=0; i<2; ++i)
	{
		cdata[i].node.ant.resize(0);
		cdata[i].node.batt.resize(0);
		cdata[i].node.bus.resize(0);
		cdata[i].node.comp.resize(0);
		cdata[i].node.cpu.resize(0);
		cdata[i].node.gps.resize(0);
		cdata[i].node.htr.resize(0);
		cdata[i].node.imu.resize(0);
		cdata[i].node.mcc.resize(0);
		cdata[i].node.motr.resize(0);
		cdata[i].node.mtr.resize(0);
		cdata[i].node.tcu.resize(0);
		cdata[i].node.pload.resize(0);
		cdata[i].node.prop.resize(0);
		cdata[i].node.psen.resize(0);
		cdata[i].node.rot.resize(0);
		cdata[i].node.rw.resize(0);
		cdata[i].node.ssen.resize(0);
		cdata[i].node.strg.resize(0);
		cdata[i].node.stt.resize(0);
		cdata[i].node.suchi.resize(0);
		cdata[i].node.swch.resize(0);
		cdata[i].node.tcv.resize(0);
		cdata[i].node.txr.resize(0);
		cdata[i].node.rxr.resize(0);
		cdata[i].node.thst.resize(0);
		cdata[i].node.tsen.resize(0);
		cdata[i].device.resize(0);
	}

	delete [] cdata;
	cdata = NULL;
}

//! Calculate JSON HASH
/*! Simple hash function (TCPL Section 6.6 Table Lookup)
	\param string String to calculate the hash for.
	\return The hash, as an unsigned 16 bit number.
*/

uint16_t json_hash(const char *string)
{
	uint16_t hashval;

	for (hashval = 0; *string != '\0'; string++)
		hashval = *string + 31 * hashval;
	return (hashval % JSON_MAX_HASH);
}

//! Add an entry to the JSON Namespace map.
/*! Allocates the space for a new ::jsonentry and then enters the information
 * associating a pointer with an entry in the name table. The name will be of form "name" if it is a
 * scalar, "name_iii" if it is a first level array, "name_iii_iii" if it is second
 * level, where "iii" is the zero filled index for the appropriate level.
	\param name variable name from the JSON Data Name Space
	\param d1 array index for first level, otherwise -1
	\param d2 array index for second level, otherwise -1
	\param offset Offset to the data from the beginning of its group.
	\param type COSMOS JSON Data Type.
	\param group COSMOS JSON Data Group.
	\param cdata Pointer to ::cosmosstruc.
	\param unit Index into JMAP unit table.
	\return The current number of entries, if successful, 0 if the entry could not be
	added, or if enough memory could not be allocated to hold the JSON stream.
*/
uint16_t json_addentry(const char *name, int16_t d1, int16_t d2, ptrdiff_t offset, uint16_t type, uint16_t group, cosmosstruc *cdata, uint16_t unit)
{
	jsonentry tentry;
	int32_t namelen;
	uint32_t csize;
	uint16_t hash;
	char ename[COSMOS_MAX_NAME];

	// Determine extended name
	strcpy(ename,name);
	if (d1 >= 0)
		sprintf(&ename[strlen(ename)],"_%03d",d1);
	if (d2 >= 0)
		sprintf(&ename[strlen(ename)],"_%03d",d2);

	namelen = strlen(ename)+1;
	if ((tentry.name = (char *)calloc(1,namelen)) == NULL)
	{
		return (0);
	}

	hash = json_hash(ename);

	// Populate the entry
	tentry.alarm_index = 0;
	tentry.alert_index = 0;
	tentry.maximum_index = 0;
	tentry.minimum_index = 0;
	tentry.unit_index = unit;
	tentry.type = type;
	tentry.group = group;
	strcpy(tentry.name,ename);
	tentry.offset = offset;
	csize = cdata->jmap[hash].size();
	cdata->jmap[hash].push_back(tentry);
	if (cdata->jmap[hash].size() != csize+1)
	{
		free(tentry.name);
		return (0);
	}

	++cdata->jmapped;

	return (cdata->jmapped);
}

uint16_t json_addentry(const char *name, int16_t d1, int16_t d2, ptrdiff_t offset, uint16_t type, uint16_t group, cosmosstruc *cdata)
{
	return (json_addentry(name, d1, d2, offset, type, group, cdata, 0));
}

//! Number of items in current JSON map
/*! Returns the number of JSON items currently mapped.
	\param hash JSON HASH value.
	\param cdata Pointer to ::cosmosstruc being used.
	\return Number of JSON items mapped, otherwise zero.
*/
uint32_t json_count_hash(uint16_t hash, cosmosstruc *cdata)
{
	return (cdata->jmap[hash].size());
}

//! Number of items in the current JSON map
/*! Returns the number of JSON items currently mapped.
	\param cdata Pointer to ::cosmosstruc being used.
	\return total number of JSON items mapped, otherwise zero.
*/

uint32_t json_count_total(cosmosstruc *cdata)
{
	uint32_t i = 0;

	for(uint32_t j = 0; j < cdata->jmap.size(); ++j)
		i += json_count_hash(j,cdata);

	return i;
}



//! Maximum jstring size
/*! Returns the maximum size that a jstring can be.
	\return The maximum size of jstring
*/
uint32_t json_length(jstring *jstring)
{
	return (jstring->length);
}

//! Length of the current ::jsonmap jstring
/*! Returns the current size of the jstring associated with the
 * indicated ::jsonmap
	\return The NULL terminated length of the jstring
*/
uint32_t json_index(jstring *jstring)
{
	return (jstring->index);
}

//! Initialize JSON output
/*! Prepares the indicated ::jsonmap for populating the jstring
	\return 0 if successful, otherwise negative error.
*/
int32_t json_startout(jstring *jstring)
{

	if (jstring == NULL)	{
		return (JSON_ERROR_JSTRING);
	}

	if (jstring->length == 0)
	{
		jstring->length = 1024;
		jstring->string = (char *)calloc(1024,1);
	}

	memset((void *)jstring->string,0,jstring->length);
	jstring->index = 0;
	return (0);
}

//! Terminate JSON output
/*! Closes off and null terminates the jstring in the indicated ::jsonmap and calculates the final
 * length.
	\return The final NULL terminated jstring. NULL if unsuccessful.
*/
int32_t json_stopout(jstring *jstring)
{
	jstring->index = strlen(jstring->string);
	return(0);
}

//! Name of indexed entry in COSMOS name space.
/*! Return the COSMOS name space name for the provided hash and index.
	\param hash Entry in the hash table.
	\param index Index into a particular entry in the hash table.
	\return Pointer to a string with the name, or NULL.
*/
/*
char *json_name_index(jsonhandle handle, cosmosstruc *cdata)
{
	//	int32_t iretn;

	if (!cdata || !cdata->jmapped)
		return (NULL);

	if (cdata->jmap[handle.hash].size() > handle.index)
	{
		return (cdata->jmap[handle.hash][handle.index].name);
	}
	else
		return (NULL);
}
*/

//! Double precision value of entry in COSMOS name space.
/*! Return the Double Precision equivalent of the entry for the provided hash and index.
	\param hash Entry in the hash table.
	\param index Index into a particular entry in the hash table.
	\return Double precision value, or 0.
*/
/*
double json_get_double_index(jsonhandle handle, cosmosstruc *cdata)
{
	double value;
	//	int32_t iretn;

	if (!cdata || !cdata->jmapped)
		return (NAN);

	if (cdata->jmap[handle.hash].size() > handle.index)
	{
		value = json_get_double_name(json_name_index(handle.hash,handle.index,cdata),cdata);
		return (value);
	}
	else
		return (NAN);
}
*/

//! Perform JSON output for a single indexed JSON item
/*! Populates the jstring for the indicated ::jsonmap by index.
	\param hash The JSON HASH of the desired variable.
	\param index The JSON index of the desired variable for that hash.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_handle(jstring *jstring, jsonhandle handle, cosmosstruc *cdata)
{
	int32_t iretn;
	uint8_t *data;

	if (!cdata || !cdata->jmapped)
		return (JSON_ERROR_NOJMAP);

	if ((iretn=json_out_character(jstring,'{')))
		return (iretn);

	data = json_ptr_of_offset(cdata->jmap[handle.hash][handle.index].offset,cdata->jmap[handle.hash][handle.index].group,cdata);
	if ((iretn=json_out_value(jstring,cdata->jmap[handle.hash][handle.index].name,data,cdata->jmap[handle.hash][handle.index].type,cdata)))
		return (iretn);

	if ((iretn=json_out_character(jstring,'}')))
		return (iretn);

	return (iretn);
}

int32_t json_out_value(jstring *jstring, char *name, uint8_t *data, uint16_t type, cosmosstruc *cdata)
{
	int32_t iretn;

	if (!cdata || !cdata->jmapped)
		return (JSON_ERROR_NOJMAP);

	if ((iretn=json_out_name(jstring,name)))
		return (iretn);

	switch (type)
	{
	case JSON_TYPE_UINT16:
		if ((iretn=json_out_uint16(jstring,*(uint16_t *)data)))
			return (iretn);
		break;
	case JSON_TYPE_UINT32:
		if ((iretn=json_out_uint32(jstring,*(uint32_t *)data)))
			return (iretn);
		break;
	case JSON_TYPE_INT16:
		if ((iretn=json_out_int16(jstring,*(int16_t *)data)))
			return (iretn);
		break;
	case JSON_TYPE_INT32:
		if ((iretn=json_out_int32(jstring,*(int32_t *)data)))
			return (iretn);
		break;
	case JSON_TYPE_FLOAT:
		if ((iretn=json_out_float(jstring,*(float *)data)))
			return (iretn);
		break;
	case JSON_TYPE_DOUBLE:
	case JSON_TYPE_TIMESTAMP:
		if ((iretn=json_out_double(jstring,*(double *)data)))
			return (iretn);
		break;
	case JSON_TYPE_STRING:
		if ((iretn=json_out_string(jstring,(char *)data,COSMOS_MAX_DATA)))
			return (iretn);
		break;
	case JSON_TYPE_NAME:
		if ((iretn=json_out_string(jstring,(char *)data,COSMOS_MAX_NAME)))
			return (iretn);
		break;
	case JSON_TYPE_GVECTOR:
		if ((iretn=json_out_gvector(jstring,*(gvector *)data)))
			return (iretn);
		break;
	case JSON_TYPE_CVECTOR:
		if ((iretn=json_out_cvector(jstring,*(cvector *)data)))
			return (iretn);
		break;
	case JSON_TYPE_QUATERNION:
		if ((iretn=json_out_quaternion(jstring,*(quaternion *)data)))
			return (iretn);
		break;
	case JSON_TYPE_RMATRIX:
		if ((iretn=json_out_rmatrix(jstring,*(rmatrix *)data)))
			return (iretn);
		break;
	case JSON_TYPE_DCM:
		if ((iretn=json_out_dcm(jstring,*(rmatrix *)data)))
			return (iretn);
		break;
	case JSON_TYPE_RVECTOR:
	case JSON_TYPE_TVECTOR:
		if ((iretn=json_out_rvector(jstring,*(rvector *)data)))
			return (iretn);
		break;
	case JSON_TYPE_POS_SELG:
	case JSON_TYPE_POS_GEOD:
		if ((iretn=json_out_geoidpos(jstring,*(geoidpos *)data)))
			return (iretn);
		break;
	case JSON_TYPE_POS_GEOS:
		if ((iretn=json_out_spherpos(jstring,*(spherpos *)data)))
			return (iretn);
		break;
	case JSON_TYPE_CARTPOS:
	case JSON_TYPE_POS_GEOC:
	case JSON_TYPE_POS_SELC:
	case JSON_TYPE_POS_ECI:
	case JSON_TYPE_POS_SCI:
	case JSON_TYPE_POS_BARYC:
		if ((iretn=json_out_cartpos(jstring,*(cartpos *)data)))
			return (iretn);
		break;
	case JSON_TYPE_DCMATT:
		if ((iretn=json_out_dcmatt(jstring,*(dcmatt *)data)))
			return (iretn);
		break;
	case JSON_TYPE_QATT:
	case JSON_TYPE_QATT_TOPO:
	case JSON_TYPE_QATT_GEOC:
	case JSON_TYPE_QATT_LVLH:
	case JSON_TYPE_QATT_ICRF:
	case JSON_TYPE_QATT_SELC:
		if ((iretn=json_out_qatt(jstring,*(qatt *)data)))
			return (iretn);
		break;
	case JSON_TYPE_HBEAT:
		if ((iretn=json_out_beatstruc(jstring,*(beatstruc *)data)))
			return (iretn);
		break;
	case JSON_TYPE_LOC_POS:
		if ((iretn=json_out_posstruc(jstring,*(posstruc *)data)))
			return (iretn);
		break;
	case JSON_TYPE_LOC_ATT:
		if ((iretn=json_out_attstruc(jstring,*(attstruc *)data)))
			return (iretn);
		break;
	case JSON_TYPE_LOC:
		if ((iretn=json_out_locstruc(jstring,*(locstruc *)data)))
			return (iretn);
		break;
	}

	return (iretn);
}

//! Extend JSON stream
/*! Append the indicated string to the current JSON stream, extending it if necessary.
	\param string String to be appended.
	\return 0 if successful, negative error number otherwise.
*/
int32_t json_append(jstring *jstring,char *tstring)
{
	uint16_t tlen;
	char *tjstring;

	tlen = strlen(tstring);
	if (tlen+jstring->index > jstring->length-3)
	{
		if ((tjstring = (char *)calloc(jstring->length+1024,1)) == NULL)
			return (JSON_ERROR_DATA_LENGTH);
		strncpy(tjstring,jstring->string,jstring->length);
		jstring->length += 1024;
		free(jstring->string);
		jstring->string = tjstring;
	}
	strcpy(&jstring->string[jstring->index],tstring);
	jstring->index += strlen(tstring);
	return (0);
}

//! Single character to JSON
/*! Appends an entry for the single character to the current JSON stream.
	\param character Character to be added to string in the raw.
	\return 0 if successful, otherwise negative error.
*/
int32_t json_out_character(jstring *jstring,char character)
{
	char tstring[2] = {0,0};
	int32_t iretn;

	tstring[0] = character;
	if ((iretn=json_append(jstring,tstring)) < 0)
		return (iretn);

	return (0);
}

//! Object name to JSON
/*! Appends an entry for the name piece of a JSON object (including ":") to the current JSON
 * string.
	\param name The Object Name
	\return  0 if successful, otherwise negative error.
*/
int32_t json_out_name(jstring *jstring,char *name)
{
	int32_t iretn;

	if ((iretn=json_out_string(jstring,name,COSMOS_MAX_NAME)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,':')) < 0)
		return (iretn);
	return (0);
}

//! Signed 16 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 16 bit signed integer.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_int16(jstring *jstring,int16_t value)
{
	int32_t iretn;
	char tstring[15];

	sprintf(tstring,"%d",value);

	iretn = json_append(jstring,tstring);
	return (iretn);
}

//! Signed 32 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 32 bit signed integer.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_int32(jstring *jstring,int32_t value)
{
	int32_t iretn;
	char tstring[15];

	sprintf(tstring,"%d",value);

	iretn = json_append(jstring,tstring);
	return (iretn);
}

//! Unsigned 16 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 16 bit unsigned integer.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_uint16(jstring *jstring,uint16_t value)
{
	int32_t iretn;
	char tstring[15];

	sprintf(tstring,"%u",value);

	iretn = json_append(jstring,tstring);
	return (iretn);
}

//! Unsigned 32 bit integer to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated 32 bit unsigned integer.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_uint32(jstring *jstring,uint32_t value)
{
	int32_t iretn;
	char tstring[15];

	sprintf(tstring,"%u",value);

	iretn = json_append(jstring,tstring);
	return (iretn);
}

//! Single precision floating point32_t to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated float.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_float(jstring *jstring,float value)
{
	int32_t iretn = 0;
	char tstring[15];

	if (isfinite(value))
	{
		sprintf(tstring,"%.8g",value);
		iretn = json_append(jstring,tstring);
	}

	return (iretn);
}

//! Perform JSON output for a single nonindexed double
/*! Appends a JSON entry to the current JSON stream for the indicated double.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_double(jstring *jstring,double value)
{
	int32_t iretn = 0;
	char tstring[30];

	if (isfinite(value))
	{
		sprintf(tstring,"%.17g",value);
		iretn = json_append(jstring,tstring);
	}

	return (iretn);
}

//! String to JSON
/*! Appends a JSON entry to the current JSON stream for the string variable.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_string(jstring *jstring,char *string, uint16_t len)
{
	int32_t iretn;
	uint16_t i;
	char tstring[5];

	if (len > JSON_MAX_DATA)
		len = JSON_MAX_DATA;

	if ((iretn=json_out_character(jstring,'"')) < 0)
		return (iretn);

	for (i=0; i<strlen(string); i++)
	{
		if (i > len)
			break;
		if (string[i] < 32)
		{
			if ((iretn=json_out_character(jstring,'\\')) < 0)
				return (iretn);
			switch (string[i])
			{
			case '"':
			case '\\':
			case '/':
				if ((iretn=json_out_character(jstring,string[i])) < 0)
					return (iretn);
				break;
			case '\b':
				if ((iretn=json_out_character(jstring,'b')) < 0)
					return (iretn);
				break;
			case '\f':
				if ((iretn=json_out_character(jstring,'f')) < 0)
					return (iretn);
				break;
			case '\n':
				if ((iretn=json_out_character(jstring,'n')) < 0)
					return (iretn);
				break;
			case '\r':
				if ((iretn=json_out_character(jstring,'r')) < 0)
					return (iretn);
				break;
			case '\t':
				if ((iretn=json_out_character(jstring,'t')) < 0)
					return (iretn);
				break;
			default:
				sprintf(tstring,"%04x",string[i]);
				if ((iretn=json_out_string(jstring,tstring,4)) < 0)
					return (iretn);
			}
		}
		else
		{
			if (string[i] < 127)
			{
				if ((iretn=json_out_character(jstring,string[i])) < 0)
					return (iretn);
			}
			else
			{
				if ((iretn=json_out_character(jstring,'\\')) < 0)
					return (iretn);
				sprintf(tstring,"%04x",string[i]);
				if ((iretn=json_out_string(jstring,tstring,4)) < 0)
					return (iretn);
			}
		}
	}
	if ((iretn=json_out_character(jstring,'"')) < 0)
		return (iretn);

	return (iretn);
}

//! ::gvector to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::gvector.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_gvector(jstring *jstring,gvector value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'{')) < 0)
		return (iretn);

	// Output Latitude
	if ((iretn=json_out_name(jstring,(char *)"lat")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.lat)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Longitude
	if ((iretn=json_out_name(jstring,(char *)"lon")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.lon)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Height
	if ((iretn=json_out_name(jstring,(char *)"h")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.h)) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,'}')) < 0)
		return (iretn);
	return (0);
}

//! ::svector to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::svector.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_svector(jstring *jstring,svector value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'{')) < 0)
		return (iretn);

	// Output Latitude
	if ((iretn=json_out_name(jstring,(char *)"phi")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.phi)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Longitude
	if ((iretn=json_out_name(jstring,(char *)"lambda")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.lambda)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Height
	if ((iretn=json_out_name(jstring,(char *)"r")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.r)) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,'}')) < 0)
		return (iretn);
	return (0);
}

//! ::rvector to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::rvector.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_rvector(jstring *jstring,rvector value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'[')) < 0)
		return (iretn);

	// Output Col[0]
	if ((iretn=json_out_double(jstring,value.col[0])) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Col[1]
	if ((iretn=json_out_double(jstring,value.col[1])) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Col[2]
	if ((iretn=json_out_double(jstring,value.col[2])) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,']')) < 0)
		return (iretn);

	return (0);
}

//! ::quaternion to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::quaternion.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_quaternion(jstring *jstring,quaternion value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'{')) < 0)
		return (iretn);

	// Output Orientation
	if ((iretn=json_out_name(jstring,(char *)"d")) < 0)
		return (iretn);
	if ((iretn=json_out_cvector(jstring,value.d)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output RotationA
	if ((iretn=json_out_name(jstring,(char *)"w")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.w)) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,'}')) < 0)
		return (iretn);
	return (0);
}

//! ::cvector to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::cvector.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_cvector(jstring *jstring,cvector value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'{')) < 0)
		return (iretn);

	// Output X
	if ((iretn=json_out_name(jstring,(char *)"x")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.x)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Y
	if ((iretn=json_out_name(jstring,(char *)"y")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.y)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Z
	if ((iretn=json_out_name(jstring,(char *)"z")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.z)) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,'}')) < 0)
		return (iretn);
	return (0);
}

//! ::cartpos to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::cartpos.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_cartpos(jstring *jstring,cartpos value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'{')) < 0)
		return (iretn);

	// Output Time
	if ((iretn=json_out_name(jstring,(char *)"utc")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.utc)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Position
	if ((iretn=json_out_name(jstring,(char *)"pos")) < 0)
		return (iretn);
	if ((iretn=json_out_rvector(jstring,value.s)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Velocity
	if ((iretn=json_out_name(jstring,(char *)"vel")) < 0)
		return (iretn);
	if ((iretn=json_out_rvector(jstring,value.v)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Acceleration
	if ((iretn=json_out_name(jstring,(char *)"acc")) < 0)
		return (iretn);
	if ((iretn=json_out_rvector(jstring,value.a)) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,'}')) < 0)
		return (iretn);
	return (0);
}

//! ::geoidpos to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::geoidpos.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_geoidpos(jstring *jstring,geoidpos value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'{')) < 0)
		return (iretn);

	// Output Time
	if ((iretn=json_out_name(jstring,(char *)"utc")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.utc)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Position
	if ((iretn=json_out_name(jstring,(char *)"pos")) < 0)
		return (iretn);
	if ((iretn=json_out_gvector(jstring,value.s)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Velocity
	if ((iretn=json_out_name(jstring,(char *)"vel")) < 0)
		return (iretn);
	if ((iretn=json_out_gvector(jstring,value.v)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Acceleration
	if ((iretn=json_out_name(jstring,(char *)"acc")) < 0)
		return (iretn);
	if ((iretn=json_out_gvector(jstring,value.a)) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,'}')) < 0)
		return (iretn);
	return (0);
}

//! ::spherpos to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::spherpos.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_spherpos(jstring *jstring,spherpos value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'{')) < 0)
		return (iretn);

	// Output Time
	if ((iretn=json_out_name(jstring,(char *)"utc")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.utc)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Position
	if ((iretn=json_out_name(jstring,(char *)"pos")) < 0)
		return (iretn);
	if ((iretn=json_out_svector(jstring,value.s)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Velocity
	if ((iretn=json_out_name(jstring,(char *)"vel")) < 0)
		return (iretn);
	if ((iretn=json_out_svector(jstring,value.v)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Acceleration
	if ((iretn=json_out_name(jstring,(char *)"acc")) < 0)
		return (iretn);
	if ((iretn=json_out_svector(jstring,value.a)) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,'}')) < 0)
		return (iretn);
	return (0);
}

//! ::posstruc to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::posstruc.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_posstruc(jstring *jstring,posstruc value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'{')) < 0)
		return (iretn);

	// Output Time
	if ((iretn=json_out_name(jstring,(char *)"utc")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.utc)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Barycentric
	if ((iretn=json_out_name(jstring,(char *)"baryc")) < 0)
		return (iretn);
	if ((iretn=json_out_cartpos(jstring,value.baryc)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Earth Centered Inertial
	if ((iretn=json_out_name(jstring,(char *)"eci")) < 0)
		return (iretn);
	if ((iretn=json_out_cartpos(jstring,value.eci)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Selene Centered Inertial
	if ((iretn=json_out_name(jstring,(char *)"sci")) < 0)
		return (iretn);
	if ((iretn=json_out_cartpos(jstring,value.sci)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Geocentric
	if ((iretn=json_out_name(jstring,(char *)"geoc")) < 0)
		return (iretn);
	if ((iretn=json_out_cartpos(jstring,value.geoc)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Selenocentric
	if ((iretn=json_out_name(jstring,(char *)"selc")) < 0)
		return (iretn);
	if ((iretn=json_out_cartpos(jstring,value.selc)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Geodetic
	if ((iretn=json_out_name(jstring,(char *)"geod")) < 0)
		return (iretn);
	if ((iretn=json_out_geoidpos(jstring,value.geod)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Selenographic
	if ((iretn=json_out_name(jstring,(char *)"selg")) < 0)
		return (iretn);
	if ((iretn=json_out_geoidpos(jstring,value.selg)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Geocentric Spherical
	if ((iretn=json_out_name(jstring,(char *)"geos")) < 0)
		return (iretn);
	if ((iretn=json_out_spherpos(jstring,value.geos)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,'}')) < 0)
		return (iretn);
	return (0);
}

//! ::attstruc to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::attstruc.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_attstruc(jstring *jstring,attstruc value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'{')) < 0)
		return (iretn);

	// Output Time
	if ((iretn=json_out_name(jstring,(char *)"utc")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.utc)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Topocentric
	if ((iretn=json_out_name(jstring,(char *)"topo")) < 0)
		return (iretn);
	if ((iretn=json_out_qatt(jstring,value.topo)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output LVLH
	if ((iretn=json_out_name(jstring,(char *)"lvlh")) < 0)
		return (iretn);
	if ((iretn=json_out_qatt(jstring,value.lvlh)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Geocentric
	if ((iretn=json_out_name(jstring,(char *)"geoc")) < 0)
		return (iretn);
	if ((iretn=json_out_qatt(jstring,value.geoc)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Selenocentric
	if ((iretn=json_out_name(jstring,(char *)"selc")) < 0)
		return (iretn);
	if ((iretn=json_out_qatt(jstring,value.selc)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output ICRF
	if ((iretn=json_out_name(jstring,(char *)"icrf")) < 0)
		return (iretn);
	if ((iretn=json_out_qatt(jstring,value.icrf)) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,'}')) < 0)
		return (iretn);
	return (0);
}

//! ::locstruc to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::locstruc.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_locstruc(jstring *jstring,locstruc value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'{')) < 0)
		return (iretn);

	// Output Time
	if ((iretn=json_out_name(jstring,(char *)"utc")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.utc)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Position
	if ((iretn=json_out_name(jstring,(char *)"pos")) < 0)
		return (iretn);
	if ((iretn=json_out_posstruc(jstring,value.pos)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Attitude
	if ((iretn=json_out_name(jstring,(char *)"att")) < 0)
		return (iretn);
	if ((iretn=json_out_attstruc(jstring,value.att)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Earth Magnetic Field
	if ((iretn=json_out_name(jstring,(char *)"bearth")) < 0)
		return (iretn);
	if ((iretn=json_out_rvector(jstring,value.bearth)) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,'}')) < 0)
		return (iretn);
	return (0);
}

/*! Appends a JSON entry to the current JSON stream for the indicated ::dcmatt.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_dcmatt(jstring *jstring,dcmatt value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'{')) < 0)
		return (iretn);

	// Output Position
	if ((iretn=json_out_name(jstring,(char *)"pos")) < 0)
		return (iretn);
	if ((iretn=json_out_dcm(jstring,value.s)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Velocity
	if ((iretn=json_out_name(jstring,(char *)"vel")) < 0)
		return (iretn);
	if ((iretn=json_out_rvector(jstring,value.v)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Acceleration
	if ((iretn=json_out_name(jstring,(char *)"acc")) < 0)
		return (iretn);
	if ((iretn=json_out_rvector(jstring,value.a)) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,'}')) < 0)
		return (iretn);
	return (0);
}

//! ::qatt to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::qatt.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_qatt(jstring *jstring,qatt value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'{')) < 0)
		return (iretn);

	// Output Time
	if ((iretn=json_out_name(jstring,(char *)"utc")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.utc)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Position
	if ((iretn=json_out_name(jstring,(char *)"pos")) < 0)
		return (iretn);
	if ((iretn=json_out_quaternion(jstring,value.s)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Velocity
	if ((iretn=json_out_name(jstring,(char *)"vel")) < 0)
		return (iretn);
	if ((iretn=json_out_rvector(jstring,value.v)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Acceleration
	if ((iretn=json_out_name(jstring,(char *)"acc")) < 0)
		return (iretn);
	if ((iretn=json_out_rvector(jstring,value.a)) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,'}')) < 0)
		return (iretn);
	return (0);
}

//! ::dcm to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::dcm.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_dcm(jstring *jstring,rmatrix value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'[')) < 0)
		return (iretn);

	// Output Row[0]
	if ((iretn=json_out_rvector(jstring,value.row[0])) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Row[1]
	if ((iretn=json_out_rvector(jstring,value.row[1])) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Row[2]
	if ((iretn=json_out_rvector(jstring,value.row[2])) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,']')) < 0)
		return (iretn);
	return (0);
}

//! ::rmatrix to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated
 * ::rmatrix.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_rmatrix(jstring *jstring,rmatrix value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'[')) < 0)
		return (iretn);

	// Output Row[0]
	if ((iretn=json_out_rvector(jstring,value.row[0])) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Row[1]
	if ((iretn=json_out_rvector(jstring,value.row[1])) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Row[2]
	if ((iretn=json_out_rvector(jstring,value.row[2])) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Row[3]
	if ((iretn=json_out_rvector(jstring,value.row[3])) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,']')) < 0)
		return (iretn);
	return (0);
}

//! ::beatstruc to JSON
/*! Appends a JSON entry to the current JSON stream for the indicated ::beatstruc.
	\param value The JSON data of the desired variable
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_beatstruc(jstring *jstring,beatstruc value)
{
	int32_t iretn;

	if ((iretn=json_out_character(jstring,'{')) < 0)
		return (iretn);

	// Output Ntype
	if ((iretn=json_out_name(jstring,(char *)"ntype")) < 0)
		return (iretn);
	if ((iretn=json_out_int32(jstring,value.ntype)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output IP Address
	if ((iretn=json_out_name(jstring,(char *)"addr")) < 0)
		return (iretn);
	if ((iretn=json_out_string(jstring,value.addr,16)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output IP Port
	if ((iretn=json_out_name(jstring,(char *)"port")) < 0)
		return (iretn);
	if ((iretn=json_out_int32(jstring,value.port)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Buffer Size
	if ((iretn=json_out_name(jstring,(char *)"bsz")) < 0)
		return (iretn);
	if ((iretn=json_out_int32(jstring,value.bsz)) < 0)
		return (iretn);
	if ((iretn=json_out_character(jstring,',')) < 0)
		return (iretn);

	// Output Beat Period
	if ((iretn=json_out_name(jstring,(char *)"bprd")) < 0)
		return (iretn);
	if ((iretn=json_out_double(jstring,value.bprd)) < 0)
		return (iretn);

	if ((iretn=json_out_character(jstring,'}')) < 0)
		return (iretn);
	return (0);
}

//! Perform JSON output for a single element of a 1D named JSON item.
/*! Calls ::json_out for the indexed element of the named JSON vector.
	\param token The JSON name for the desired variable
	\param index The desired element number
	\param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_1d(jstring *jstring,char *token, uint16_t index, cosmosstruc *cdata)
{
	char name[COSMOS_MAX_NAME+1];
	int32_t iretn;

	if (strlen(token) > COSMOS_MAX_NAME+4)
		return (JSON_ERROR_NAME_LENGTH);

	if (index > 999)
		return (JSON_ERROR_INDEX_SIZE);

	sprintf(name,"%s_%03u",token,index);
	iretn = json_out(jstring,name,cdata);
	return (iretn);
}

//! Perform JSON output for a single element of a 2D named JSON item.
/*! Calls ::json_out for the indexed element of the named JSON matrix.
	\param token The JSON name for the desired variable
	\param row The desired row number
	\param col The desired column number
	\param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_2d(jstring *jstring,char *token, uint16_t row, uint16_t col, cosmosstruc *cdata)
{
	char name[COSMOS_MAX_NAME+1];
	int32_t iretn;

	if (strlen(token) > COSMOS_MAX_NAME+8)
		return (JSON_ERROR_NAME_LENGTH);

	if (row > 999 || col > 999)
		return (JSON_ERROR_INDEX_SIZE);

	sprintf(name,"%s_%03u_%03u",token,row,col);
	iretn = json_out(jstring,name,cdata);
	return (iretn);
}

//! Perform JSON output for a single named JSON item
/*! Populates the jstring for the indicated ::jsonmap through reference to JSON name.
	\param jstring The jstring into which to store the result.
	\param token The JSON name for the desired variable.
	\param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return  0 if successful, negative error otherwise
*/
int32_t json_out(jstring *jstring,const char *token, cosmosstruc *cdata)
{
	jsonhandle h;

	if (!cdata || !cdata->jmapped)
		return (JSON_ERROR_NOJMAP);

	h.hash = json_hash(token);

	if (cdata->jmap.size() == 0)
		return (JSON_ERROR_NOJMAP);

	for (h.index=0; h.index<cdata->jmap[h.hash].size(); ++h.index)
		if (!strcmp(token,cdata->jmap[h.hash][h.index].name))
			return (json_out_handle(jstring,h,cdata));

	return (JSON_ERROR_NOENTRY);
}

//! Output a list of named JSON items.
/*! Populates the jstring for the indicated ::jsonmap through reference to a list of JSON
 * names.
	\param jstring The jstring into which to store the result.
	\param tokens The comma separated list of JSON names for the desired variables.
	\return  0 if successful, negative error otherwise
*/
int32_t json_out_list(jstring *jstring,const char *tokens, cosmosstruc *cdata)
{
	char tstring[COSMOS_MAX_NAME];
	const char **pointer;
	int32_t iretn;

	pointer = &tokens;
	while ((*pointer)[0] != 0 && (*pointer)[0] != '{')
		(*pointer)++;
	if ((iretn=json_parse_character(pointer,'{')))
		return (iretn);
	do
	{
		if ((iretn=json_parse_string(pointer,tstring)))
			return (iretn);
		json_out(jstring,tstring,cdata);
	} while (!json_parse_character(pointer,','));
	if ((iretn=json_parse_character(pointer,'}')) && iretn!=JSON_ERROR_EOS)
		return (iretn);
	return (0);
}


//! Output JSON items from wildcard.
/*! Populates the jstring for the indicated ::jsonmap through reference to a regular expression matching of JSON
 * names.
	\param jstring The jstring into which to store the result.
	\param wildcard The regular expression to match against.
	\return  0 if successful, negative error otherwise
*/

int32_t json_out_wildcard(jstring *jstring,const char *wildcard, cosmosstruc *cdata)
{
	int32_t iretn;
	jsonhandle h;

	for (h.hash=0; h.hash<cdata->jmap.size(); ++h.hash)
	{
		for (h.index=0; h.index<cdata->jmap[h.hash].size(); ++h.index)
		{
			if (string_cmp(wildcard,cdata->jmap[h.hash][h.index].name))
			{
				iretn = json_out_handle(jstring,h,cdata);
			}
		}
	}
	return (iretn);
}

//! Address from offset
/*! Calculate the actual address of an offset into either static or
 * dynamic space, using the provide cdata static and dynamic addresses.
 \param offset An offset taken from a ::jsonentry
 \param cdata->stat The cdata static address. NULL if internal.
 \param cdata->dyn The cdata dynamic address. NULL if internal.
 \return A pointer, castable into any desired type.
*/
uint8_t *json_ptr_of_offset(ptrdiff_t offset, uint16_t group, cosmosstruc *cdata)
{
	uint8_t *data;

	switch (group)
	{
	case JSON_GROUP_AGENT:
		data = offset+(uint8_t *)cdata->agent.data();
		break;
	case JSON_GROUP_PHYSICS:
		//		data = offset+(uint8_t *)cdata->physics.data();
		data = offset+(uint8_t *)&(cdata->physics);
		break;
	case JSON_GROUP_EVENT:
		data = offset+(uint8_t *)cdata->event.data();
		break;
	case JSON_GROUP_NODE:
		data = offset+(uint8_t *)&(cdata->node);
		break;
	case JSON_GROUP_DEVICE:
		data = offset+(uint8_t *)cdata->device.data();
		break;
	case JSON_GROUP_PIECE:
		data = offset + (uint8_t *)cdata->piece.data();
		break;
	case JSON_GROUP_TARGET:
		data = offset + (uint8_t *)cdata->target.data();
		break;
	case JSON_GROUP_USER:
		data = offset + (uint8_t *)cdata->user.data();
		break;
	case JSON_GROUP_PORT:
		data = offset+(uint8_t *)cdata->port.data();
		break;
	}
	return (data);
}

//! Info on Namespace address
/*! Return a pointer to the Namespace Entry structure containing the
 * information for a the namespace value that matches a given memory address.
 \param ptr Address of a variable that may match a namespace name.
 \param cdata Address of the ::cosmosstruc that may contain the ptr.
 \return Pointer to the ::jsonentry for the token, or NULL.
*/
jsonentry *json_entry_of_ptr(uint8_t *ptr, cosmosstruc *cdata)
{
	uint16_t n, m;
	uint16_t group;
	ptrdiff_t offset;

	if (!cdata || !cdata->jmapped)
		return (NULL);

	offset = -1;

	if ((offset = (uint8_t *)ptr - (uint8_t *)&(cdata->node)) > 0 && offset < (ptrdiff_t)sizeof(nodestruc))
	{
		group = JSON_GROUP_NODE;
	}
	else
	{
		offset = -1;
		offset = std::numeric_limits<std::ptrdiff_t>::max();
	}
	if (offset < 0 && (offset=(uint8_t *)ptr - (uint8_t *)&(cdata->node)) > 0 && offset < (ptrdiff_t)sizeof(agentstruc))
	{
		group = JSON_GROUP_AGENT;
	}
	else
	{
		offset = -1;
	}
	if (offset < 0 && (offset=(uint8_t *)ptr - (uint8_t *)&(cdata->node)) > 0 && offset < (ptrdiff_t)sizeof(physicsstruc))
	{
		group = JSON_GROUP_PHYSICS;
	}
	else
	{
		offset = -1;
	}
	if (offset < 0 && (offset=(uint8_t *)ptr - (uint8_t *)&(cdata->node)) > 0 && offset < (ptrdiff_t)sizeof(eventstruc))
	{
		group = JSON_GROUP_EVENT;
	}
	else
	{
		offset = -1;
	}

	if (offset == -1)
		return ((jsonentry *)NULL);

	for (m=0; m<cdata->jmap.size(); m++)
	{
		for (n=0; n<cdata->jmap[m].size(); n++)
		{
			if (cdata->jmap[m][n].group == group && cdata->jmap[m][n].offset == offset)
			{
				return ((jsonentry *)&cdata->jmap[m][n]);
			}
		}
	}
	return ((jsonentry *)NULL);
}

//! Info on Namespace name
/*! Return a pointer to the Namespace Entry structure containing the
 * information for a given name.
 \param token Namespace name to look up
 \return Pointer to the ::jsonentry for the token, or NULL.
*/
jsonentry *json_entry_of_name(const char *token, cosmosstruc *cdata)
{
	int16_t hash;
	uint16_t n;

	if (!cdata || !cdata->jmapped)
		return (NULL);

	hash = json_hash(token);

	if (cdata->jmap[hash].size() == 0)
		return ((jsonentry *)NULL);

	for (n=0; n<cdata->jmap[hash].size(); n++)
	{
		if (!strcmp(token,cdata->jmap[hash][n].name))
		{
			return ((jsonentry *)&cdata->jmap[hash][n]);
		}
	}
	return ((jsonentry *)NULL);
}

//! Type of namespace name.
/*! Return the ::json_type of the token in the ::jsonmap.
 \param token the JSON name for the desired variable
 \return The ::json_type, otherwise 0.
*/
uint16_t json_type_of_name(const char *token, cosmosstruc *cdata)
{
	jsonentry *entry;

	if (token[0] == '(')
		return (JSON_TYPE_EQUATION);

	if ((entry=json_entry_of_name(token, cdata)) != NULL)
	{
		return (entry->type);
	}
	return (0);
}

//! Return the data pointer that matches a JSON name
/*! Look up the provided JSON data name in the indicated ::jsonmap and return the
 * associated data pointer.
 \param token the JSON name for the desired variable
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
 \return The associated data pointer, if succesful, otherwise NULL
*/
uint8_t *json_ptrto(const char *token, cosmosstruc *cdata)
{
	jsonentry *ptr;

	if ((ptr=json_entry_of_name(token,cdata)) == NULL)
		return ((uint8_t *)NULL);
	else
		return (json_ptr_of_offset(ptr->offset,ptr->group,cdata));
}

//! Return the data pointer that matches a singly indexed JSON name
/*! Look up the provided JSON data name with the indicated index in the current ::jsonmap
 * and return the associated data pointer.
	\param token the JSON name for the desired variable
	\param index1 Primary index.
	\return The associated data pointer, if succesful, otherwise NULL
*/
uint8_t *json_ptrto_1d(const char *token, uint16_t index1, cosmosstruc *cdata)
{
	char tstring[5+COSMOS_MAX_NAME];

	if (index1 > 999)
		return ((uint8_t *)NULL);

	// Create extended name, shortening if neccessary
	sprintf(tstring,"%.*s_%03u",COSMOS_MAX_NAME,token,index1);

	return (json_ptrto(token,cdata));

}

//! Return the data pointer that matches a doubly indexed JSON name
/*! Look up the provided JSON data name with the indicated indices in the current ::jsonmap
 * and return the associated data pointer.
	\param token the JSON name for the desired variable
	\param index1 Primary index.
	\param index2 Secondary index.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return The associated data pointer, if succesful, otherwise NULL
*/
uint8_t *json_ptrto_2d(const char *token, uint16_t index1, uint16_t index2, cosmosstruc *cdata)
{
	char tstring[9+COSMOS_MAX_NAME];

	// Return error if index too large
	if (index1 > 999 || index2 > 999)
		return ((uint8_t *)NULL);

	// Create extended name, shortening if necessary
	sprintf(tstring,"%.*s_%03u_%03u",COSMOS_MAX_NAME,token,index1,index2);

	return (json_ptrto(token,cdata));

}

//! Return integer from name.
/*! If the named value can in any way be interepreted as a number,
 * return it as a signed 32 bit integer.
 \param token Valid COSMOS Namespace name.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
 \return Value cast as a signed 32 bit integer, or 0.
*/
int32_t json_get_int_name(const char *token, cosmosstruc *cdata)
{
	int32_t value=0;
	jsonentry *ptr;

	if ((ptr=json_entry_of_name(token,cdata)) == NULL)
		return (0);

	switch (ptr->type)
	{
	case JSON_TYPE_UINT16:
		value = (int32_t)(*(uint16_t *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)));
		break;
	case JSON_TYPE_UINT32:
		value = (int32_t)(*(uint32_t *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)));
		break;
	case JSON_TYPE_INT16:
		value = (int32_t)(*(int16_t *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)));
		break;
	case JSON_TYPE_INT32:
		value = (int32_t)(*(int32_t *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)));
		break;
	case JSON_TYPE_FLOAT:
		value = (int32_t)(*(float *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)));
		break;
	case JSON_TYPE_DOUBLE:
	case JSON_TYPE_TIMESTAMP:
		value = (int32_t)(*(double *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)) + .5);
		break;
	case JSON_TYPE_STRING:
		value = atol((char *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)));
		break;
	}

	return (value);

}

//! Return string from name.
/*! If the named value is a string, just copy it. Otherwise, print
whatever numerical value as a string. Return a pointer to an internal
storage buffer for the string. Note: this value is changed each time
you call this function.
	\param token Valid COSMOS Namespace name.
	\param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return Pointer to a char buffer containing the string.
*/
char *json_get_string_name(const char *token, cosmosstruc *cdata)
{
	jsonentry *ptr;

	if ((ptr=json_entry_of_name(token,cdata)) == NULL)
		return (tbuf);

	switch (ptr->type)
	{
	case JSON_TYPE_UINT16:
		sprintf(tbuf,"%u",(*(uint16_t *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata))));
		break;
	case JSON_TYPE_UINT32:
		sprintf(tbuf,"%u",(*(uint32_t *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata))));
		break;
	case JSON_TYPE_INT16:
		sprintf(tbuf,"%d",(*(int16_t *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata))));
		break;
	case JSON_TYPE_INT32:
		sprintf(tbuf,"%d",(*(int32_t *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata))));
		break;
	case JSON_TYPE_FLOAT:
		sprintf(tbuf,"%.8g",(*(float *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata))));
		break;
	case JSON_TYPE_DOUBLE:
	case JSON_TYPE_TIMESTAMP:
		sprintf(tbuf,"%.17g",(*(double *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)) + .5));
		break;
	case JSON_TYPE_STRING:
	case JSON_TYPE_NAME:
		strcpy(tbuf,(char *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)));
		break;
	}

	return (tbuf);
}

//! Return integer from 1d name.
/*! If the named 1d indexed value can in any way be interepreted as a number,
 * return it as a signed 32 bit integer.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
 \return Value cast as a signed 32 bit integer, or 0.
*/
int32_t json_get_int_name_1d(const char *token, int32_t index1, cosmosstruc *cdata)
{
	int32_t value;
	char tstring[5+COSMOS_MAX_NAME];

	if (index1 > 999)
		return (0);

	// Create extended name, shortening if neccessary
	sprintf(tstring,"%.*s_%03u",COSMOS_MAX_NAME,token,index1);

	value = json_get_int_name(tstring,cdata);

	return (value);
}

//! Return integer from 2d name.
/*! If the named 2d indexed value can in any way be interepreted as a number,
 * return it as a signed 32 bit integer.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.
 \param index2 2d index.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
 \return Value cast as a signed 32 bit integer, or 0.
*/
int32_t json_get_int_name_2d(const char *token, int32_t index1, int32_t index2, cosmosstruc *cdata)
{
	int32_t value;
	char tstring[9+COSMOS_MAX_NAME];

	if (index1 > 999)
		return (0);

	// Create extended name, shortening if neccessary
	sprintf(tstring,"%.*s_%03u_%03u",COSMOS_MAX_NAME,token,index1,index2);

	value = json_get_int_name(tstring,cdata);

	return (value);
}

//! Return unsigned integer from name.
/*! If the named value can in any way be interepreted as a number,
 * return it as an unsigned 32 bit integer.
 \param token Valid COSMOS Namespace name.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
 \return Value cast as an unsigned 32 bit integer, or 0.
*/
uint32_t json_get_uint_name(const char *token, cosmosstruc *cdata)
{
	uint32_t value=0;
	jsonentry *ptr;

	if ((ptr=json_entry_of_name(token,cdata)) == NULL)
		return (0);

	switch (ptr->type)
	{
	case JSON_TYPE_UINT16:
		value = (uint32_t)(*(uint16_t *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)));
		break;
	case JSON_TYPE_UINT32:
		value = (uint32_t)(*(uint32_t *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)));
		break;
	case JSON_TYPE_INT16:
		value = (uint32_t)(*(int16_t *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)));
		break;
	case JSON_TYPE_INT32:
		value = (uint32_t)(*(int32_t *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)));
		break;
	case JSON_TYPE_FLOAT:
		value = (uint32_t)(*(float *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)));
		break;
	case JSON_TYPE_DOUBLE:
	case JSON_TYPE_TIMESTAMP:
		value = (uint32_t)(*(double *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)) + .5);
		break;
	case JSON_TYPE_STRING:
		value = (uint32_t)atol((char *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata)));
		break;
	}

	return (value);

}

//! Return unsigned integer from 1d name.
/*! If the named 1d indexed value can in any way be interepreted as a number,
 * return it as an unsigned 32 bit integer.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
 \return Value cast as an unsigned 32 bit integer, or 0.
*/
uint32_t json_get_uint_name_1d(const char *token, int32_t index1, cosmosstruc *cdata)
{
	uint32_t value;
	char tstring[5+COSMOS_MAX_NAME];

	if (index1 > 999)
		return (0);

	// Create extended name, shortening if neccessary
	sprintf(tstring,"%.*s_%03u",COSMOS_MAX_NAME,token,index1);


	value = json_get_uint_name(tstring,cdata);

	return (value);
}

//! Return unsigned integer from 2d name.
/*! If the named 2d indexed value can in any way be interepreted as a number,
 * return it as an unsigned 32 bit integer.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.
 \param index2 2d index.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
 \return Value cast as an unsigned 32 bit integer, or 0.
*/
uint32_t json_get_uint_name_2d(const char *token, int32_t index1, int32_t index2, cosmosstruc *cdata)
{
	uint32_t value;
	char tstring[9+COSMOS_MAX_NAME];

	if (index1 > 999)
		return (0);

	// Create extended name, shortening if neccessary
	sprintf(tstring,"%.*s_%03u_%03u",COSMOS_MAX_NAME,token,index1,index2);

	value = json_get_uint_name(tstring,cdata);

	return (value);
}

//! Return double from name.
/*! If the named value can in any way be interepreted as a number,
 * return it as a double.
 \param token Valid COSMOS Namespace name.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
 \return Value cast as a double, or 0.
*/
double json_get_double_name(const char *token, cosmosstruc *cdata)
{
	double value=0.;
	jsonentry *ptr;

	if (!isnan(value=json_equation(&token,cdata)))
		return (value);

	if ((ptr=json_entry_of_name(token,cdata)) == NULL)
		return (NAN);

	if (!isnan(value=json_get_double_entry(ptr, cdata)))
		return (value);

	return (NAN);
}

//! Return double from entry.
/*! If the named value can in any way be interepreted as a number,
 * return it as a double.
 \param ptr Pointer to a valid COSMOS Namespace entry.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
 \return Value cast as a double, or 0.
*/
double json_get_double_entry(jsonentry *ptr, cosmosstruc *cdata)
{
	uint8_t *data;
	double value;

	data = json_ptr_of_offset(ptr->offset,ptr->group,cdata);

	switch (ptr->type)
	{
	case JSON_TYPE_UINT16:
		value = (double)(*(uint16_t *)(data));
		break;
	case JSON_TYPE_UINT32:
		value = (double)(*(uint32_t *)(data));
		break;
	case JSON_TYPE_INT16:
		value = (double)(*(int16_t *)(data));
		break;
	case JSON_TYPE_INT32:
		value = (double)(*(int32_t *)(data));
		break;
	case JSON_TYPE_FLOAT:
		value = (double)(*(float *)(data));
		break;
	case JSON_TYPE_DOUBLE:
	case JSON_TYPE_TIMESTAMP:
		value = (double)(*(double *)(data));
		break;
	case JSON_TYPE_STRING:
		value = atol((char *)data);
		break;
	}

	return (value);

}

//! Return double from 1d name.
/*! If the named 1d indexed value can in any way be interepreted as a number,
 * return it as a double.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
 \return Value cast as a double, or 0.
*/
double json_get_double_name_1d(const char *token, int32_t index1, cosmosstruc *cdata)
{
	double value;
	char tstring[5+COSMOS_MAX_NAME];

	if (index1 > 999)
		return (0);

	// Create extended name, shortening if neccessary
	sprintf(tstring,"%.*s_%03u",COSMOS_MAX_NAME,token,index1);


	value = json_get_double_name(tstring,cdata);

	return (value);
}

//! Return double from 2d name.
/*! If the named 2d indexed value can in any way be interepreted as a number,
 * return it as a double.
 \param token Valid COSMOS Namespace name.
 \param index1 1d index.
 \param index2 2d index.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
 \return Value cast as a double, or 0.
*/
double json_get_double_name_2d(const char *token, int32_t index1, int32_t index2, cosmosstruc *cdata)
{
	double value;
	char tstring[9+COSMOS_MAX_NAME];

	if (index1 > 999)
		return (0);

	// Create extended name, shortening if neccessary
	sprintf(tstring,"%.*s_%03u_%03u",COSMOS_MAX_NAME,token,index1,index2);


	value = json_get_double_name(tstring,cdata);

	return (value);
}

//! Set name from double.
/*! If the provided double can in any way be placed in the name it
 * will.
 \param value Double precision value to be stored in the name space.
 \param token Valid COSMOS Namespace name.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
 \return Negative error or zero.
*/
int32_t json_set_double_name(double value, char *token, cosmosstruc *cdata)
{
	utype *nval;
	jsonentry *ptr;

	if ((ptr=json_entry_of_name(token,cdata)) == NULL)
		return (0);

	nval = (utype *)(json_ptr_of_offset(ptr->offset,ptr->group,cdata));
	switch (ptr->type)
	{
	case JSON_TYPE_UINT16:
		nval->u16 = (uint16_t)(value + .5);
		break;
	case JSON_TYPE_UINT32:
		nval->u32 = (uint32_t)(value + .5);
		break;
	case JSON_TYPE_INT16:
		nval->i16 = (int16_t)(value + .5);
		break;
	case JSON_TYPE_INT32:
		nval->i32 = (int32_t)(value + .5);
		break;
	case JSON_TYPE_FLOAT:
		nval->f = value;
		break;
	case JSON_TYPE_DOUBLE:
	case JSON_TYPE_TIMESTAMP:
		nval->d = value;
		break;
	}
	return (0);
}

//! Return the results of a JSON equation.
/*! At its minimum, a JSON equation will be defined as any two JSON
 * names separated by an operation. Equations bracketed by '()' will
 * be treated as names in their own right and will therefore require a
 * matching operation, and name or bracketed equation. The result will
 * always be assumed to be double precision, even in the case of
 * booleans. Valid operations are:
	- '+': addition
	- '-': subtraction
	- '*': multiplication
	- '/': division
	- '&': logical AND
	- '|': logical OR
	- '>': logical Greater Than
	- '<': logical Less Than
	- '=': logical Equal
  \param pointer Pointer to a pointer to a JSON stream.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
  \return Result of the equation, or NAN.
*/
double json_equation(const char** opointer, cosmosstruc *cdata)
{
	char equation[JSON_MAX_DATA+1];
	int32_t iretn;
	jsonhandle handle;

	if ((iretn=json_parse_equation(opointer, equation)) < 0)
		return (NAN);

	if (cdata->emap.size() == 0)
		return (NAN);

	if ((iretn=json_equation_map(equation, cdata, &handle)) < 0)
	{
		return (NAN);
	}

	return(json_equation_entry(&cdata->emap[handle.hash][handle.index], cdata));
}

//! Return the results of a known JSON equation handle
/*! Calculate a ::json_equation using already looked up values for the hash and index.
	\param handle Values for hash and index that point to an entry in the map.
	\param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return Result of the equation, or NAN.
*/
double json_equation_handle(jsonhandle *handle, cosmosstruc *cdata)
{
	return(json_equation_entry(&cdata->emap[handle->hash][handle->index], cdata));
}

//! Return the results of a known JSON equation entry
/*! Calculate a ::json_equation using already looked up entry from the map.
	\param ptr Pointer to a ::jsonequation from the map.
	\param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return Result of the equation, or NAN.
*/
double json_equation_entry(jsonequation *ptr, cosmosstruc *cdata)
{
	double a[2], c=NAN;

	for (uint16_t i=0; i<2; ++i)
	{
		switch(ptr->operand[i].type)
		{
		case JSON_OPERAND_NULL:
			break;
		case JSON_OPERAND_CONSTANT:
			a[i] = ptr->operand[i].value;
			break;
		case JSON_OPERAND_EQUATION:
			a[i] = json_equation_entry(&cdata->emap[ptr->operand[i].data.hash][ptr->operand[i].data.index], cdata);
			break;
		case JSON_OPERAND_NAME:
			a[i] = json_get_double_entry(&cdata->jmap[ptr->operand[i].data.hash][ptr->operand[i].data.index],cdata);
			break;
		}
	}

	switch(ptr->operation)
	{
	case JSON_OPERATION_NOT:
		c = !a[0];
		break;
	case JSON_OPERATION_COMPLEMENT:
		c = ~(uint32_t)(a[0]);
		break;
	case JSON_OPERATION_ADD:
		c = a[0] + a[1];
		break;
	case JSON_OPERATION_SUBTRACT:
		c = a[0] - a[1];
		break;
	case JSON_OPERATION_MULTIPLY:
		c = a[0] * a[1];
		break;
	case JSON_OPERATION_DIVIDE:
		c = a[0] / a[1];
		break;
	case JSON_OPERATION_AND:
		c = a[0] && a[1];
		break;
	case JSON_OPERATION_OR:
		c = a[0] || a[1];
		break;
	case JSON_OPERATION_GT:
		c = a[0] > a[1];
		break;
	case JSON_OPERATION_LT:
		c = a[0] < a[1];
		break;
	case JSON_OPERATION_EQ:
		c = a[0] == a[1];
		break;
	}
	return (c);
}

//! Extract JSON value.
/*! Scan through the provided JSON stream looking for the supplied
	Namespace name. If it is found, return its value as a character
	string.
	\param jstring A string of JSON data
	\param token The Namespace name to be extracted.
	\return A character string representing the extracted value, otherwise NULL.
*/
char *json_extract_namedobject(char *jstring, char *token)
{
	char *ptr, *bptr, *eptr;
	uint32_t count;

	if ((ptr=strstr(jstring,token)) == NULL) return (NULL);

	while (ptr[0] != 0 && ptr[0] != ':') ptr++;

	if ((ptr)[0] == 0) return (NULL);

	ptr++;
	while (ptr[0] != 0 && isspace(ptr[0])) ptr++;

	if (ptr[0] == 0) return (NULL);

	//	ptr++;
	bptr = ptr;
	switch (ptr[0])
	{
	case '{':
		count = 1;
		do
		{
			++ptr;
			switch (ptr[0])
			{
			case '}':
				--count;
				break;
			case '{':
				++count;
				break;
			}
		} while (count);
		eptr = ptr;
		break;
	case '[':
		count = 1;
		do
		{
			++ptr;
			switch (ptr[0])
			{
			case ']':
				--count;
				break;
			case '[':
				++count;
				break;
			}
		} while (count);
		eptr = ptr;
		break;
	case '"':
		do
		{
			++ptr;
		} while (ptr[0] != '"');
		eptr = ptr;
		break;
	default:
		do
		{
			++ptr;
		} while ((ptr[0]>='0'&&ptr[0]<='9')||ptr[0]=='e'||ptr[0]=='E'||ptr[0]=='.'||ptr[0]=='-');
		eptr = ptr-1;
		break;
	}

	strncpy(tbuf,bptr,eptr-bptr+1);
	tbuf[eptr-bptr+1] = 0;

	return (tbuf);
}

//! Parse JSON using Name Space.
/*! Scan through the provided JSON stream, matching names to the ::json_name_list. For
 * each match that is found, load the associated data item with the accompanying data.
 * This function supports complex data types.
	\param jstring A string of JSON data
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return Zero or negative error.
*/
int32_t json_parse(const char *jstring, cosmosstruc *cdata)
{
	const char *cpoint;
	int32_t length, iretn;
	uint32_t count = 0;

	length = strlen(jstring);
	cpoint = &jstring[0];
	while (*cpoint != 0 && *cpoint != '{')
		cpoint++;
	do
	{
		// is this the only reference to endlines?
		if (*cpoint != 0)// && *cpoint != '\r' && *cpoint != '\n')
			//if (*cpoint != 0 && *cpoint != '\r' && *cpoint != '\n')
		{
			if ((iretn = json_parse_namedobject(&cpoint,cdata)) < 0)
			{
				if (iretn != JSON_ERROR_EOS && iretn != JSON_ERROR_NOJMAP)
					iretn = 0;
			}
			else
			{
				++count;
			}
		}
		else
			iretn = JSON_ERROR_EOS;
	} while (iretn != JSON_ERROR_EOS && iretn != JSON_ERROR_NOJMAP && *cpoint != 0 && (cpoint-&jstring[0]) <= length);

	if (!iretn) iretn = count;
	return (iretn);
}

//! Parse next JSON Named Pair
/*! Extract the next Named Pair from the provided JSON stream. Return a pointer to the next
 * Object in the string, and an error flag.
	\param pointer Pointer to a pointer to a JSON stream.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return Zero, or a negative error.
*/
int32_t json_parse_namedobject(const char** pointer, cosmosstruc *cdata)
{
	uint32_t hash, n;
	int32_t iretn=0;
	char string[JSON_MAX_DATA+1];

	if (!(cdata->jmapped))
	{
		return (JSON_ERROR_NOJMAP);
	}

	if ((*pointer)[0] != '{')
	{
		if ((iretn = json_skip_value(pointer)) < 0)
			return (iretn);
		else
			return (JSON_ERROR_SCAN);
	}

	(*pointer)++;

	// Extract string that should hold name of this object.
	if ((iretn = json_parse_string(pointer, string)) < 0)
	{
		if (iretn != JSON_ERROR_EOS)
		{
			if ((iretn = json_skip_value(pointer)) < 0)
				return (iretn);
			else
				return (JSON_ERROR_SCAN);
		}
		else
			return (iretn);
	}

	// Calculate hash
	hash = json_hash(string);

	// See if there is a match in the ::jsonmap.
	for (n=0; n<cdata->jmap[hash].size(); ++n)	{
		if (!strcmp(string,cdata->jmap[hash][n].name))
		{
			break;
		}
	}

	if (n == cdata->jmap[hash].size())	{
		if ((iretn = json_skip_value(pointer)) < 0 && iretn != JSON_ERROR_EOS)
		{
			return (iretn);
		}
		else
			return (JSON_ERROR_NOENTRY);
	}
	else
	{
		if ((iretn = json_skip_white(pointer)) < 0)
		{
			if (iretn != JSON_ERROR_EOS)
			{
				if ((iretn = json_skip_value(pointer)) < 0)
					return (iretn);
				else
					return (JSON_ERROR_SCAN);
			}
			else
				return (iretn);
		}
		if ((iretn = json_parse_character(pointer,':')) < 0)
		{
			if (iretn != JSON_ERROR_EOS)
			{
				if ((iretn = json_skip_value(pointer)) < 0)
					return (iretn);
				else
					return (JSON_ERROR_SCAN);
			}
			else
				return (iretn);
		}
		if ((iretn = json_parse_value(pointer,cdata->jmap[hash][n].type,cdata->jmap[hash][n].offset,cdata->jmap[hash][n].group,cdata)) < 0)
		{
			if (iretn != JSON_ERROR_EOS)
			{
				if ((iretn = json_skip_value(pointer)) < 0)
					return (iretn);
				else
					return (JSON_ERROR_SCAN);
			}
			else
			{
				return (iretn);
			}
		}
	}

	(*pointer)++;
	json_skip_white(pointer);
	return (iretn);
}

//! Parse a specific character out of a JSON stream
/*! Look for the specified character in the provided JSON stream and
 * flag an error if it's not there. Otherwise, increment the pointer
 * to the next byte in the string.
 \param pointer Pointer to a pointer to a JSON stream.
 \param character The character to look for.
 \return Zero or a negative error.
*/
int32_t json_parse_character(const char **pointer, const char character)
{
	int32_t iretn = 0;

	if ((*pointer)[0] == 0)
		return (JSON_ERROR_EOS);

	if ((iretn = json_skip_white(pointer)) < 0)
	{
		return (iretn);
	}
	if ((*pointer)[0] != character)
		return (JSON_ERROR_SCAN);
	(*pointer)++;

	//whitespace after character
	if ((iretn = json_skip_white(pointer)) < 0)
	{
		return (iretn);
	}

	return (iretn);
}

//! Parse the next variable name out of a JSON stream.
/*! Look for a valid JSON string, followed by a ':' and copy that
 * name to the provided buffer, otherwise flag an error.
 \param pointer Pointer to a pointer to a JSON stream.
 \param string Pointer to a location to copy the string.
 \return Zero, otherwise negative error.
*/
int32_t json_parse_name(const char **pointer, char *string)
{
	int32_t iretn = 0;

	if ((*pointer)[0] == 0)
		return (JSON_ERROR_EOS);

	//Skip whitespace before name
	if ((iretn = json_skip_white(pointer)) < 0)
	{
		return (iretn);
	}

	//Parse name
	if ((iretn = json_parse_string(pointer, string)) < 0)
		return (iretn);

	//Skip whitespace after name
	if ((iretn = json_skip_white(pointer)) < 0)
	{
		return (iretn);
	}

	if ((iretn = json_parse_character(pointer, ':')) < 0)
		return (iretn);

	return (iretn);
}

//! Parse the next JSON equation out of a JSON stream.
/*! Look for a valid JSON equation in the provided JSON stream and copy
 * it to the provided buffer. Otherwise flag an error.
 \param pointer Pointer to a pointer to a JSON stream.
 \param string Pointer to a location to copy the equation.
 \return Zero, otherwise negative error.
*/
int32_t json_parse_equation(const char **pointer, char *equation)
{
	int32_t iretn = 0;
	register uint16_t i2;
	uint16_t index, ilen, depth=1;
	char iequation[JSON_MAX_DATA+1];

	if ((*pointer)[0] == 0)
		return (JSON_ERROR_EOS);

	// Skip white space before '('
	if ((iretn=json_skip_white(pointer)) < 0)
	{
		return(JSON_ERROR_SCAN);
	}

	// Check if it's an equation. All equations must begin with '(' and end with ')'
	if ((*pointer)[0] != '(')
	{
		return(JSON_ERROR_SCAN);
	}

	ilen = strlen(*pointer);

	// Start of object, get equation
	index = 0;
	iequation[index++] = (*pointer)[0];
	for (i2=1; i2<ilen; i2++)
	{
		if (index == JSON_MAX_DATA-1)
			break;
		switch((*pointer)[i2])
		{
		case ' ':
		case '\t':
		case '\f':
		case '\n':
		case '\r':
		case '\v':
			break;
		case ')':
			--depth;
			iequation[index++] = (*pointer)[i2];
			break;
		case '(':
			++depth;
			iequation[index++] = (*pointer)[i2];
			break;
		default:
			iequation[index++] = (*pointer)[i2];
			break;
		}
		if (!depth)
			break;
	}

	if (i2 >= ilen)
	{
		*pointer = &(*pointer)[ilen-1];
		return(JSON_ERROR_SCAN);
	}

	if (equation != NULL)
	{
		strncpy(equation,iequation,index);
		equation[index] = 0;
	}

	*pointer = &(*pointer)[i2+1];

	return (iretn);
}

//! Parse the next JSON equation operand out of a JSON stream.
/*! Look for a valid JSON equation operand in the provided JSON stream and
 * copy it to a ::jsonoperand.
 \param pointer Pointer to a pointer to a JSON stream.
 \param operand Pointer to a ::jsonoperand.
 \return Zero, otherwise negative error.
*/
int32_t json_parse_operand(const char **pointer, jsonoperand *operand, cosmosstruc *cdata)
{
	char string[JSON_MAX_DATA+1];
	int32_t iretn;

	switch((*pointer)[0])
	{
	case '(':
		// It's an equation
		if ((iretn=json_parse_equation(pointer,string)) < 0)
		{
			return (JSON_ERROR_SCAN);
		}
		if ((iretn=json_equation_map(string, cdata, &operand->data)) < 0)
		{
			return (JSON_ERROR_SCAN);
		}
		operand->type = JSON_OPERAND_EQUATION;
		break;
	case '"':
		if ((iretn=json_parse_string(pointer,string)) < 0)
		{
			return (JSON_ERROR_SCAN);
		}
		if ((iretn=json_name_map(string, cdata, &operand->data)) < 0)
		{
			return (JSON_ERROR_SCAN);
		}
		operand->type = JSON_OPERAND_NAME;
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '+':
	case '-':
	case '.':
		operand->type = JSON_OPERAND_CONSTANT;
		if ((iretn=json_parse_number(pointer,&operand->value)) < 0)
			return (JSON_ERROR_SCAN);
		break;
	default:
		operand->type = JSON_OPERAND_NULL;
		break;
	}

	return (0);
}

//! Parse the next JSON string out of a JSON stream.
/*! Look for a valid JSON string in the provided JSON stream and copy
 * it to the provided buffer. Otherwise flag an error.
 \param pointer Pointer to a pointer to a JSON stream.
 \param string Pointer to a location to copy the string.
 \return Zero, otherwise negative error.
*/
int32_t json_parse_string(const char **pointer, char *string)
{
	int32_t iretn = 0;
	register uint16_t i2;
	uint16_t index, ilen;
	char istring[JSON_MAX_DATA+1];

	if ((*pointer)[0] == 0)
		return (JSON_ERROR_EOS);

	//Skip whitespace before string
	if ((iretn = json_skip_white(pointer)) < 0)
	{
		return (iretn);
	}

	ilen = strlen(*pointer);

	// Start of object, get string
	index = 0;
	for (i2=1; i2<ilen; i2++)
	{
		if ((*pointer)[i2] == '"')
			break;
		if ((*pointer)[i2] == '\\')
		{
			switch ((*pointer)[i2+1])
			{
			case '"':
			case '\\':
			case '/':
				istring[index] = (*pointer)[i2+1];
				break;
			case 'b':
				istring[index] = '\b';
				break;
			case 'f':
				istring[index] = '\f';
				break;
			case 'n':
				istring[index] = '\n';
				break;
			case 'r':
				istring[index] = '\r';
				break;
			case 't':
				istring[index] = '\t';
				break;
			default:
				i2 += 3;
			}
			i2++;
		}
		else
		{
			istring[index] = (*pointer)[i2];
		}
		index++;
	}

	if (i2 >= ilen)
	{
		*pointer = &(*pointer)[ilen-1];
		return(JSON_ERROR_SCAN);
	}

	// i2 is last character in string +1, index is length of extracted string
	if (index-1 > JSON_MAX_DATA)
		index = JSON_MAX_DATA - 2;

	if (string != NULL)
	{
		strncpy(string,istring,index);
		string[index] = 0;
	}

	*pointer = &(*pointer)[i2+1];

	return (iretn);
}

//! Parse the next number out of a JSON stream.
/*! Extract a valid number out of the provided JSON stream, otherwise
 * flag an error.
 \param pointer Pointer to a pointer to a JSON stream.
 \param number Extracted number in double precission.
 \return Zero or negative error.
*/
int32_t json_parse_number(const char **pointer, double *number)
{
	int32_t iretn = 0;
	uint32_t i1, ilen;

	if ((*pointer)[0] == 0)
		return (JSON_ERROR_EOS);

	//Skip whitespace before number
	if ((iretn = json_skip_white(pointer)) < 0)
	{
		return (iretn);
	}

	ilen = strlen(*pointer);


	// First, check for integer: series of digits
	i1 = 0;
	if ((*pointer)[i1] == '-')
		++i1;
	while (i1 < ilen && (*pointer)[i1] >= '0' && (*pointer)[i1] <= '9')
	{
		++i1;
	}

	if (i1 == ilen)
	{
		return (JSON_ERROR_EOS);
	}

	// Second, check for fraction: . followed by series of digits
	if ((*pointer)[i1] == '.')
	{
		++i1;
		while (i1 < ilen && (*pointer)[i1] >= '0' && (*pointer)[i1] <= '9')
		{
			++i1;
		}

		if (i1 == ilen)
		{
			return (JSON_ERROR_EOS);
		}
	}


	// Third, check for exponent: e or E followed by optional - and series of digits
	if ((*pointer)[i1] == 'e' || (*pointer)[i1] == 'E')
	{
		++i1;
		if ((*pointer)[i1] == '-')
			++i1;
		while (i1 < ilen && (*pointer)[i1] >= '0' && (*pointer)[i1] <= '9')
		{
			++i1;
		}

		if (i1 == ilen)
		{
			return (JSON_ERROR_EOS);
		}
	}

	// Finally, scan resulting string and move pointer to new location: i1 equals first position after number
	sscanf(*pointer,"%lf",number);
	*pointer = &(*pointer)[i1];
	return (iretn);
}

//! Skip white space in JSON string.
/*! Skip over any white space characters, leaving pointer at next non white
 * space.
	\param pointer Double pointer to the JSON string
	\return Zero, or negative error.
*/
int32_t json_skip_white(const char **pointer)
{
	if ((*pointer)[0] == 0)
		return (JSON_ERROR_EOS);

	while ((*pointer)[0] != 0 && isspace((*pointer)[0])) (*pointer)++;

	if ((*pointer)[0] == 0)
		return (JSON_ERROR_EOS);
	else
		return (0);
}

//! Skip next value in JSON string
/*! Skip over characters until you reach the next value in a JSON string.
	\param pointer Double pointer to the JSON string
	\return Zero, or negative error.
*/
int32_t json_skip_value(const char **pointer)
{

	if ((*pointer)[0] == 0)
		return (JSON_ERROR_EOS);

	while ((*pointer)[0] != 0 && (*pointer)[0] != '{')
		(*pointer)++;

	if ((*pointer)[0] == 0 || (*pointer)[1] == 0)
		return (JSON_ERROR_EOS);
	else
		return (0);
}

int32_t json_parse_value(const char **pointer, uint8_t type, ptrdiff_t offset, uint16_t group, cosmosstruc *cdata)
{
	char input[JSON_MAX_DATA+1];
	int32_t iretn = 0;
	double val;
	cartpos *rp;
	geoidpos *gp;
	spherpos *sp;
	dcmatt *ra;
	qatt *qa;
	uint8_t *data;

	if ((*pointer)[0] == 0)
		return (JSON_ERROR_EOS);

	data = json_ptr_of_offset(offset,group,cdata);

	//Skip whitespace before value
	if ((iretn = json_skip_white(pointer)) < 0)
	{
		return (iretn);
	}

	switch (type)
	{
	case JSON_TYPE_UINT16:
		if (isnan(val=json_equation(pointer,cdata)))
		{
			if ((iretn = json_parse_number(pointer,&val)) < 0)
				return (iretn);
		}
		*(uint16_t *)data = (uint16_t)val;
		break;
	case JSON_TYPE_UINT32:
		if (isnan(val=json_equation(pointer,cdata)))
		{
			if ((iretn = json_parse_number(pointer,&val)) < 0)
				return (iretn);
		}
		*(uint32_t *)data = (uint32_t)val;
		break;
	case JSON_TYPE_INT16:
		if (isnan(val=json_equation(pointer,cdata)))
		{
			if ((iretn = json_parse_number(pointer,&val)) < 0)
				return (iretn);
		}
		*(int16_t *)data = (int16_t)val;
		break;
	case JSON_TYPE_INT32:
		if (isnan(val=json_equation(pointer,cdata)))
		{
			if ((iretn = json_parse_number(pointer,&val)) < 0)
				return (iretn);
		}
		*(int32_t *)data = (int32_t)val;
		break;
	case JSON_TYPE_FLOAT:
		if (isnan(val=json_equation(pointer,cdata)))
		{
			if ((iretn = json_parse_number(pointer,&val)) < 0)
				return (iretn);
		}
		*(float *)data = (float)val;
		break;
	case JSON_TYPE_TIMESTAMP:
		if (isnan(val=json_equation(pointer,cdata)))
		{
			if ((iretn = json_parse_number(pointer,&val)) < 0)
				return (iretn);
		}
		*(double *)data = (double)val;
		break;
	case JSON_TYPE_DOUBLE:
		if (isnan(val=json_equation(pointer,cdata)))
		{
			if ((iretn = json_parse_number(pointer,&val)) < 0)
				return (iretn);
		}
		*(double *)data = (double)val;
		break;
	case JSON_TYPE_STRING:
	case JSON_TYPE_NAME:
		if ((iretn = json_parse_string(pointer,(char *)input)) < 0)
			return (iretn);
		if (type == JSON_TYPE_NAME)
		{
			if (strlen(input) > COSMOS_MAX_NAME) input[COSMOS_MAX_NAME-1] = 0;
			strcpy((char *)data,input);
		}
		else
		{
			if (strlen(input) > COSMOS_MAX_DATA) input[COSMOS_MAX_DATA-1] = 0;
			strcpy((char *)data,input);
		}
		break;
	case JSON_TYPE_GVECTOR:
		if ((iretn = json_parse_character(pointer,'{')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(gvector,lat),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(gvector,lon),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(gvector,h),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,'}')) < 0)
			return (iretn);
		break;
	case JSON_TYPE_CVECTOR:
		if ((iretn = json_parse_character(pointer,'{')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(cvector,x),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(cvector,y),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(cvector,z),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,'}')) < 0)
			return (iretn);
		break;
	case JSON_TYPE_RVECTOR:
	case JSON_TYPE_TVECTOR:
		if ((iretn = json_parse_character(pointer,'[')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(rvector,col[0]),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(rvector,col[1]),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(rvector,col[2]),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,']')) < 0)
			return (iretn);
		break;
	case JSON_TYPE_QUATERNION:
		if ((iretn = json_parse_character(pointer,'{')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_CVECTOR,offset+(ptrdiff_t)offsetof(quaternion,d),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(quaternion,w),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,'}')) < 0)
			return (iretn);
		break;
	case JSON_TYPE_RMATRIX:
		if ((iretn = json_parse_character(pointer,'[')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(rmatrix,row[0]),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(rmatrix,row[1]),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(rmatrix,row[2]),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(rmatrix,row[3]),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,']')) < 0)
			return (iretn);
		break;
	case JSON_TYPE_DCM:
		if ((iretn = json_parse_character(pointer,'[')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(rmatrix,row[0]),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(rmatrix,row[1]),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(rmatrix,row[2]),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,']')) < 0)
			return (iretn);
		break;
	case JSON_TYPE_POS_SELG:
	case JSON_TYPE_POS_GEOD:
		if ((iretn = json_parse_character(pointer,'{')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,input)) < 0)
			return (iretn);

		gp = (geoidpos *)(data+(ptrdiff_t)offsetof(geoidpos,utc));
		gp->utc = 0.;
		if (!strcmp(input,"utc"))
		{
			if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(cartpos,utc),group,cdata)) < 0)
				return (iretn);
			if ((iretn = json_parse_character(pointer,',')) < 0)
				return (iretn);
			if ((iretn = json_parse_name(pointer,NULL)) < 0)
				return (iretn);
			//			json_set_double_name(gp->utc,(char *)"node_loc_utc",cdata);
		}
		if (gp->utc == 0.)
		{
			gp->utc = currentmjd(cdata->physics.utcoffset);
		}
		
		if ((iretn = json_parse_value(pointer,JSON_TYPE_GVECTOR,offset+(ptrdiff_t)offsetof(geoidpos,s),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_GVECTOR,offset+(ptrdiff_t)offsetof(geoidpos,v),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_GVECTOR,offset+(ptrdiff_t)offsetof(geoidpos,a),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,'}')) < 0)
			return (iretn);
		gp->pass++;
		//		pos_geod((locstruc *)json_ptrto((char *)"node_loc",cdata));
		break;
	case JSON_TYPE_POS_GEOS:
		if ((iretn = json_parse_character(pointer,'{')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,input)) < 0)
			return (iretn);

		sp = (spherpos *)(data+(ptrdiff_t)offsetof(spherpos,utc));
		sp->utc = 0.;
		if (!strcmp(input,"utc"))
		{
			if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(cartpos,utc),group,cdata)) < 0)
				return (iretn);
			if ((iretn = json_parse_character(pointer,',')) < 0)
				return (iretn);
			if ((iretn = json_parse_name(pointer,NULL)) < 0)
				return (iretn);
			//			json_set_double_name(sp->utc,(char *)"node_loc_utc",cdata);
		}
		if (sp->utc == 0.)
		{
			sp->utc = currentmjd(cdata->physics.utcoffset);
		}
		
		if ((iretn = json_parse_value(pointer,JSON_TYPE_SVECTOR,offset+(ptrdiff_t)offsetof(spherpos,s),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_SVECTOR,offset+(ptrdiff_t)offsetof(spherpos,v),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_SVECTOR,offset+(ptrdiff_t)offsetof(spherpos,a),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,'}')) < 0)
			return (iretn);
		sp->pass++;
		//		pos_geos((locstruc *)json_ptrto((char *)"node_loc",cdata));
		break;
	case JSON_TYPE_CARTPOS:
	case JSON_TYPE_POS_GEOC:
	case JSON_TYPE_POS_SELC:
	case JSON_TYPE_POS_ECI:
	case JSON_TYPE_POS_SCI:
	case JSON_TYPE_POS_BARYC:
		if ((iretn = json_parse_character(pointer,'{')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,input)) < 0)
			return (iretn);

		rp = (cartpos *)(data+(ptrdiff_t)offsetof(cartpos,utc));
		rp->utc = 0.;
		if (!strcmp(input,"utc"))
		{
			if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(cartpos,utc),group,cdata)) < 0)
				return (iretn);
			if ((iretn = json_parse_character(pointer,',')) < 0)
				return (iretn);
			if ((iretn = json_parse_name(pointer,NULL)) < 0)
				return (iretn);
			//			json_set_double_name(rp->utc,(char *)"node_loc_utc",cdata);
		}
		if (rp->utc == 0.)
		{
			rp->utc = currentmjd(cdata->physics.utcoffset);
		}
		
		if ((iretn = json_parse_value(pointer,JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(cartpos,s),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(cartpos,v),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(cartpos,a),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,'}')) < 0)
			return (iretn);
		rp->pass++;
		break;
	case JSON_TYPE_DCMATT:
		if ((iretn = json_parse_character(pointer,'{')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_DCM,offset+(ptrdiff_t)offsetof(dcmatt,s),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(dcmatt,v),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(dcmatt,a),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,'}')) < 0)
			return (iretn);
		ra = (dcmatt *)(data+(ptrdiff_t)offsetof(dcmatt,utc));
		ra->utc = *(double *)json_ptrto((char *)"node_utc",cdata);
		break;
	case JSON_TYPE_QATT:
	case JSON_TYPE_QATT_TOPO:
	case JSON_TYPE_QATT_GEOC:
	case JSON_TYPE_QATT_ICRF:
	case JSON_TYPE_QATT_SELC:
	case JSON_TYPE_QATT_LVLH:
		if ((iretn = json_parse_character(pointer,'{')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,input)) < 0)
			return (iretn);

		qa = (qatt *)(data+(ptrdiff_t)offsetof(qatt,utc));
		qa->utc = 0.;
		if (!strcmp(input,"utc"))
		{
			if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(qatt,utc),group,cdata)) < 0)
				return (iretn);
			if ((iretn = json_parse_character(pointer,',')) < 0)
				return (iretn);
			if ((iretn = json_parse_name(pointer,NULL)) < 0)
				return (iretn);
			//			json_set_double_name(qa->utc,(char *)"node_loc_utc",cdata);
		}
		if (qa->utc == 0.)
		{
			qa->utc = currentmjd(cdata->physics.utcoffset);
		}
		
		if ((iretn = json_parse_value(pointer,JSON_TYPE_QUATERNION,offset+(ptrdiff_t)offsetof(qatt,s),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(qatt,v),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_name(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(qatt,a),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,'}')) < 0)
			return (iretn);
		qa->pass++;
		break;
	case JSON_TYPE_HBEAT:
		if ((iretn = json_parse_character(pointer,'{')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(beatstruc,utc),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_STRING,offset+(ptrdiff_t)offsetof(beatstruc,node),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_STRING,offset+(ptrdiff_t)offsetof(beatstruc,proc),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_INT32,offset+(ptrdiff_t)offsetof(beatstruc,ntype),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_STRING,offset+(ptrdiff_t)offsetof(beatstruc,addr),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_INT32,offset+(ptrdiff_t)offsetof(beatstruc,port),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_INT32,offset+(ptrdiff_t)offsetof(beatstruc,bsz),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(beatstruc,bprd),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,'}')) < 0)
			return (iretn);
		break;
	case JSON_TYPE_LOC_POS:
		if ((iretn = json_parse_character(pointer,'{')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(posstruc,utc),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_CARTPOS,offset+(ptrdiff_t)offsetof(posstruc,baryc),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_CARTPOS,offset+(ptrdiff_t)offsetof(posstruc,eci),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_CARTPOS,offset+(ptrdiff_t)offsetof(posstruc,sci),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_CARTPOS,offset+(ptrdiff_t)offsetof(posstruc,geoc),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_CARTPOS,offset+(ptrdiff_t)offsetof(posstruc,selc),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_POS_GEOD,offset+(ptrdiff_t)offsetof(posstruc,geod),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_POS_GEOD,offset+(ptrdiff_t)offsetof(posstruc,selg),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_POS_GEOS,offset+(ptrdiff_t)offsetof(posstruc,geos),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,'}')) < 0)
			return (iretn);
		break;
	case JSON_TYPE_LOC:
		if ((iretn = json_parse_character(pointer,'{')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_DOUBLE,offset+(ptrdiff_t)offsetof(locstruc,utc),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_POSSTRUC,offset+(ptrdiff_t)offsetof(locstruc,pos),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_ATTSTRUC,offset+(ptrdiff_t)offsetof(locstruc,att),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,',')) < 0)
			return (iretn);
		if ((iretn = json_parse_string(pointer,NULL)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,':')) < 0)
			return (iretn);
		if ((iretn = json_parse_value(pointer,JSON_TYPE_RVECTOR,offset+(ptrdiff_t)offsetof(locstruc,bearth),group,cdata)) < 0)
			return (iretn);
		if ((iretn = json_parse_character(pointer,'}')) < 0)
			return (iretn);
		break;
	}

	//Skip whitespace after value
	if ((iretn = json_skip_white(pointer)) < 0)
	{
		return (iretn);
	}

	return (iretn);
}

//! Clear global data structure
/*! Zero out elements of the ::cosmosstruc. The entire structure can be cleared, or the
 * clearing can be confined to either the Dynamic or Static piece. This allows you to remove
 * the effects of previous calls to ::json_parse.
	\param type One of JSON_MAP_ALL, JSON_MAP_AGENT, JSON_MAP_BEACON, JSON_MAP_NODESTATIC, or
	JSON_MAP_NODEDYNAMIC
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return 0, or a negative \ref error.
*/
int32_t json_clear_cosmosstruc(int32_t type, cosmosstruc *cdata)
{
	switch (type)
	{
	case JSON_GROUP_NODE:
		memset(&(cdata->node),0,sizeof(nodestruc));
		break;
	case JSON_GROUP_EVENT:
		memset(cdata->event.data(),0,sizeof(eventstruc));
		break;
	case JSON_GROUP_PHYSICS:
		//		memset(cdata->physics.data(),0,sizeof(physicsstruc));
		memset(&(cdata->physics),0,sizeof(physicsstruc));
		break;
	case JSON_GROUP_AGENT:
		memset(cdata->agent.data(),0,sizeof(agentstruc));
		break;
	case JSON_GROUP_USER:
		memset(cdata->user.data(),0,sizeof(userstruc));
		break;
	case JSON_GROUP_PORT:
		memset(cdata->port.data(),0,sizeof(portstruc));
		break;
	case JSON_GROUP_TARGET:
		memset(cdata->target.data(),0,cdata->target.size()*sizeof(targetstruc));
		break;
	}
	return (0);
}

//! Map Name Space to global data structure components and pieces
/*! Create an entry in the JSON mapping tables between each name in the Name Space and the
 * \ref cosmosstruc.
 *	\param buffer Character buffer containing contents of node.ini.
 *	\param cdata Pointer to cdata ::cosmosstruc.
	\return 0, or a negative ::error
*/
int32_t json_setup_node(char *buffer, cosmosstruc *cdata)
{
	uint32_t i;
	int16_t iretn;

	if (!cdata || !cdata->jmapped)
		return (JSON_ERROR_NOJMAP);

	// First pass: parse data for summary information
	if ((iretn = json_parse(buffer,cdata)) < 0 && iretn != JSON_ERROR_EOS)
		return (iretn);

	// Resize, then add entries to map for pieces
	cdata->piece.resize(cdata->node.piece_cnt);
	if (cdata->piece.size() != cdata->node.piece_cnt)
		return (AGENT_ERROR_MEMORY);
	for (i=0; i<cdata->node.piece_cnt; i++)
	{
		json_addpieceentry(i, cdata);
	}

	// Second pass: parse data for piece information
	if ((iretn = json_parse(buffer,cdata)) < 0 && iretn != JSON_ERROR_EOS)
		return (iretn);

	// Resize, then add entries to map for devices
	cdata->device.resize(cdata->node.device_cnt);
	if (cdata->device.size() != cdata->node.device_cnt)
		return (AGENT_ERROR_MEMORY);

	// Add entries to map for Devices and set pointers in nodestruc for comp
	for (i=0; i< cdata->node.device_cnt; i++)
	{
		json_addcompentry(i, cdata);
	}

	// Third pass: parse data for device information
	if ((iretn = json_parse(buffer,cdata)) < 0 && iretn != JSON_ERROR_EOS)
		return (iretn);

	// Add entries to map for Devices specific information
	for (i=0; i< cdata->node.device_cnt; i++)
	{
		json_adddeviceentry(i, cdata);
	}

	// Resize, then add names for ports
	cdata->port.resize(cdata->node.port_cnt);
	if (cdata->port.size() != cdata->node.port_cnt)
		return (AGENT_ERROR_MEMORY);

	for (i=0; i<cdata->node.port_cnt; i++)
	{
		json_addentry((char *)"port_name",i,-1,(ptrdiff_t)offsetof(portstruc,name)+i*sizeof(portstruc),JSON_TYPE_NAME,JSON_GROUP_PORT,cdata);
		json_addentry((char *)"port_type",i,-1,(ptrdiff_t)offsetof(portstruc,type)+i*sizeof(portstruc),JSON_TYPE_UINT16,JSON_GROUP_PORT,cdata);
	}

	// Final pass: parse data again to get all remaining info
	iretn = json_parse(buffer, cdata);

	return (0);
}

//! Add base entries to JMAP
/*! Add all of the base entries to the Namespace map.
 \param cdata Pointer to cdata ::cosmosstruc.
	\return The current number of entries, if successful, 0 if the entry could not be
 */
uint16_t json_addbaseentry(cosmosstruc *cdata)
{
	// User structure
	json_addentry((char *)"user_node",-1,-1,offsetof(userstruc,node),JSON_TYPE_NAME,JSON_GROUP_USER,cdata);
	json_addentry((char *)"user_name",-1,-1,offsetof(userstruc,name),JSON_TYPE_NAME,JSON_GROUP_USER,cdata);
	json_addentry((char *)"user_tool",-1,-1,offsetof(userstruc,tool),JSON_TYPE_NAME,JSON_GROUP_USER,cdata);
	json_addentry((char *)"user_cpu",-1,-1,offsetof(userstruc,cpu),JSON_TYPE_NAME,JSON_GROUP_USER,cdata);


	// Agent structure
	json_addentry((char *)"agent_pid",-1,-1,offsetof(agentstruc,pid),JSON_TYPE_INT32,JSON_GROUP_AGENT,cdata);
	json_addentry((char *)"agent_aprd",-1,-1,offsetof(agentstruc,aprd),JSON_TYPE_DOUBLE,JSON_GROUP_AGENT,cdata);
	json_addentry((char *)"agent_stateflag",-1,-1,offsetof(agentstruc,stateflag),JSON_TYPE_UINT16,JSON_GROUP_AGENT,cdata);
	json_addentry((char *)"agent_sohstring",-1,-1,offsetof(agentstruc,sohstring),JSON_TYPE_STRING,JSON_GROUP_AGENT,cdata);
	json_addentry((char *)"agent_beat",-1,-1,offsetof(agentstruc,beat),JSON_TYPE_HBEAT,JSON_GROUP_AGENT,cdata);
	json_addentry((char *)"agent_utc",-1,-1,offsetof(agentstruc,beat)+offsetof(beatstruc,utc),JSON_TYPE_DOUBLE,JSON_GROUP_AGENT,cdata);
	json_addentry((char *)"agent_node",-1,-1,offsetof(agentstruc,beat)+offsetof(beatstruc,node),JSON_TYPE_NAME,JSON_GROUP_AGENT,cdata);
	json_addentry((char *)"agent_proc",-1,-1,offsetof(agentstruc,beat)+offsetof(beatstruc,proc),JSON_TYPE_NAME,JSON_GROUP_AGENT,cdata);
	json_addentry((char *)"agent_ntype",-1,-1,offsetof(agentstruc,beat)+offsetof(beatstruc,ntype),JSON_TYPE_UINT16,JSON_GROUP_AGENT,cdata);
	json_addentry((char *)"agent_addr",-1,-1,offsetof(agentstruc,beat)+offsetof(beatstruc,addr),JSON_TYPE_NAME,JSON_GROUP_AGENT,cdata);
	json_addentry((char *)"agent_port",-1,-1,offsetof(agentstruc,beat)+offsetof(beatstruc,port),JSON_TYPE_UINT16,JSON_GROUP_AGENT,cdata);
	json_addentry((char *)"agent_bsz",-1,-1,offsetof(agentstruc,beat)+offsetof(beatstruc,bsz),JSON_TYPE_UINT16,JSON_GROUP_AGENT,cdata);
	json_addentry((char *)"agent_bprd",-1,-1,offsetof(agentstruc,beat)+offsetof(beatstruc,bprd),JSON_TYPE_DOUBLE,JSON_GROUP_AGENT,cdata);
	json_addentry((char *)"agent_user",-1,-1,offsetof(agentstruc,beat)+offsetof(beatstruc,user),JSON_TYPE_NAME,JSON_GROUP_AGENT,cdata);

	// Event structure
	json_addentry((char *)"event_utc",-1,-1,offsetof(longeventstruc,utc),JSON_TYPE_DOUBLE,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_utcexec",-1,-1,offsetof(longeventstruc,utcexec),JSON_TYPE_DOUBLE,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_node",-1,-1,offsetof(longeventstruc,node),JSON_TYPE_NAME,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_name",-1,-1,offsetof(longeventstruc,name),JSON_TYPE_NAME,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_user",-1,-1,offsetof(longeventstruc,user),JSON_TYPE_NAME,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_flag",-1,-1,offsetof(longeventstruc,flag),JSON_TYPE_UINT32,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_type",-1,-1,offsetof(longeventstruc,type),JSON_TYPE_UINT32,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_data",-1,-1,offsetof(longeventstruc,data),JSON_TYPE_STRING,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_condition",-1,-1,offsetof(longeventstruc,condition),JSON_TYPE_STRING,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_value",-1,-1,offsetof(longeventstruc,value),JSON_TYPE_DOUBLE,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_dtime",-1,-1,offsetof(longeventstruc,dtime),JSON_TYPE_DOUBLE,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_ctime",-1,-1,offsetof(longeventstruc,ctime),JSON_TYPE_DOUBLE,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_denergy",-1,-1,offsetof(longeventstruc,denergy),JSON_TYPE_FLOAT,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_cenergy",-1,-1,offsetof(longeventstruc,cenergy),JSON_TYPE_FLOAT,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_dmass",-1,-1,offsetof(longeventstruc,dmass),JSON_TYPE_FLOAT,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_cmass",-1,-1,offsetof(longeventstruc,cmass),JSON_TYPE_FLOAT,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_dbytes",-1,-1,offsetof(longeventstruc,dbytes),JSON_TYPE_FLOAT,JSON_GROUP_EVENT,cdata);
	json_addentry((char *)"event_cbytes",-1,-1,offsetof(longeventstruc,cbytes),JSON_TYPE_FLOAT,JSON_GROUP_EVENT,cdata);

	// Physics structure
	json_addentry((char *)"node_utcoffset",-1,-1,offsetof(physicsstruc,utcoffset),JSON_TYPE_DOUBLE,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_utcoffset",-1,-1,offsetof(physicsstruc,utcoffset),JSON_TYPE_DOUBLE,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_dt",-1,-1,offsetof(physicsstruc,dt),JSON_TYPE_DOUBLE,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_dtj",-1,-1,offsetof(physicsstruc,dtj),JSON_TYPE_DOUBLE,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_mjdbase",-1,-1,offsetof(physicsstruc,mjdbase),JSON_TYPE_DOUBLE,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_mjdaccel",-1,-1,offsetof(physicsstruc,mjdaccel),JSON_TYPE_DOUBLE,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_mjddiff",-1,-1,offsetof(physicsstruc,mjddiff),JSON_TYPE_DOUBLE,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_mode",-1,-1,offsetof(physicsstruc,mode),JSON_TYPE_INT32,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_thrust",-1,-1,offsetof(physicsstruc,thrust),JSON_TYPE_RVECTOR,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_adrag",-1,-1,offsetof(physicsstruc,adrag),JSON_TYPE_RVECTOR,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_rdrag",-1,-1,offsetof(physicsstruc,rdrag),JSON_TYPE_RVECTOR,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_atorque",-1,-1,offsetof(physicsstruc,atorque),JSON_TYPE_RVECTOR,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_rtorque",-1,-1,offsetof(physicsstruc,rtorque),JSON_TYPE_RVECTOR,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_gtorque",-1,-1,offsetof(physicsstruc,gtorque),JSON_TYPE_RVECTOR,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_htorque",-1,-1,offsetof(physicsstruc,htorque),JSON_TYPE_RVECTOR,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_ctorque",-1,-1,offsetof(physicsstruc,ctorque),JSON_TYPE_RVECTOR,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_ftorque",-1,-1,offsetof(physicsstruc,ftorque),JSON_TYPE_RVECTOR,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_ftorque_x",-1,-1,offsetof(physicsstruc,ftorque.col[0]),JSON_TYPE_DOUBLE,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_ftorque_y",-1,-1,offsetof(physicsstruc,ftorque.col[1]),JSON_TYPE_DOUBLE,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_ftorque_z",-1,-1,offsetof(physicsstruc,ftorque.col[2]),JSON_TYPE_DOUBLE,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_hcap",-1,-1,offsetof(physicsstruc,hcap),JSON_TYPE_FLOAT,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_area",-1,-1,offsetof(physicsstruc,area),JSON_TYPE_FLOAT,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_moi",-1,-1,offsetof(physicsstruc,moi),JSON_TYPE_RVECTOR,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_com",-1,-1,offsetof(physicsstruc,com),JSON_TYPE_RVECTOR,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_mass",-1,-1,offsetof(physicsstruc,mass),JSON_TYPE_FLOAT,JSON_GROUP_PHYSICS,cdata);
	json_addentry((char *)"physics_heat",-1,-1,offsetof(physicsstruc,heat),JSON_TYPE_FLOAT,JSON_GROUP_PHYSICS,cdata);

	// Node Structure
	json_addentry((char *)"node_name",-1,-1,offsetof(nodestruc,name),JSON_TYPE_NAME,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_type",-1,-1,offsetof(nodestruc,type),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_state",-1,-1,offsetof(nodestruc,state),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_hcap",-1,-1,offsetof(nodestruc,hcap),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_mass",-1,-1,offsetof(nodestruc,mass),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_area",-1,-1,offsetof(nodestruc,area),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_battcap",-1,-1,offsetof(nodestruc,battcap),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_charging",-1,-1,offsetof(nodestruc,charging),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_powgen",-1,-1,offsetof(nodestruc,powgen),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_powuse",-1,-1,offsetof(nodestruc,powuse),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_battlev",-1,-1,offsetof(nodestruc,battlev),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc",-1,-1,offsetof(nodestruc,loc),JSON_TYPE_LOC,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_utc",-1,-1,offsetof(nodestruc,loc.utc),JSON_TYPE_DOUBLE,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_utc",-1,-1,offsetof(nodestruc,loc.utc),JSON_TYPE_DOUBLE,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos",-1,-1,offsetof(nodestruc,loc.pos),JSON_TYPE_POSSTRUC,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_geod",-1,-1,offsetof(nodestruc,loc.pos.geod),JSON_TYPE_POS_GEOD,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_geod_v_lat",-1,-1,offsetof(nodestruc,loc.pos.geod.s.lat),JSON_TYPE_DOUBLE,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_geod_v_lon",-1,-1,offsetof(nodestruc,loc.pos.geod.s.lon),JSON_TYPE_DOUBLE,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_geod_v_h",-1,-1,offsetof(nodestruc,loc.pos.geod.s.h),JSON_TYPE_DOUBLE,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_geod_s_lat",-1,-1,offsetof(nodestruc,loc.pos.geod.s.lat),JSON_TYPE_DOUBLE,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_geod_s_lon",-1,-1,offsetof(nodestruc,loc.pos.geod.s.lon),JSON_TYPE_DOUBLE,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_geod_s_h",-1,-1,offsetof(nodestruc,loc.pos.geod.s.h),JSON_TYPE_DOUBLE,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_geoc",-1,-1,offsetof(nodestruc,loc.pos.geoc),JSON_TYPE_POS_GEOC,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_geos",-1,-1,offsetof(nodestruc,loc.pos.geos),JSON_TYPE_POS_GEOS,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_eci",-1,-1,offsetof(nodestruc,loc.pos.eci),JSON_TYPE_POS_ECI,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_sci",-1,-1,offsetof(nodestruc,loc.pos.sci),JSON_TYPE_POS_SCI,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_selc",-1,-1,offsetof(nodestruc,loc.pos.selc),JSON_TYPE_POS_SELC,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_selg",-1,-1,offsetof(nodestruc,loc.pos.selg),JSON_TYPE_POS_SELG,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_baryc",-1,-1,offsetof(nodestruc,loc.pos.baryc),JSON_TYPE_POS_BARYC,JSON_GROUP_NODE,cdata);
	//	json_addentry((char *)"node_loc_pos_extra",-1,-1,offsetof(nodestruc,loc.pos.extra),JSON_TYPE_POS_EXTRA,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_sunsize",-1,-1,offsetof(nodestruc,loc.pos.sunsize),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_sunradiance",-1,-1,offsetof(nodestruc,loc.pos.sunradiance),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_earthsep",-1,-1,offsetof(nodestruc,loc.pos.earthsep),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_pos_moonsep",-1,-1,offsetof(nodestruc,loc.pos.moonsep),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att",-1,-1,offsetof(nodestruc,loc.att),JSON_TYPE_ATTSTRUC,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_icrf",-1,-1,offsetof(nodestruc,loc.att.icrf),JSON_TYPE_QATT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_icrf_s",-1,-1,offsetof(nodestruc,loc.att.icrf.s),JSON_TYPE_QUATERNION,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_icrf_v",-1,-1,offsetof(nodestruc,loc.att.icrf.v),JSON_TYPE_RVECTOR,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_icrf_a",-1,-1,offsetof(nodestruc,loc.att.icrf.a),JSON_TYPE_RVECTOR,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_topo",-1,-1,offsetof(nodestruc,loc.att.topo),JSON_TYPE_QATT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_topo_s",-1,-1,offsetof(nodestruc,loc.att.topo.s),JSON_TYPE_QUATERNION,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_topo_v",-1,-1,offsetof(nodestruc,loc.att.topo.v),JSON_TYPE_RVECTOR,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_topo_a",-1,-1,offsetof(nodestruc,loc.att.topo.a),JSON_TYPE_RVECTOR,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_geoc",-1,-1,offsetof(nodestruc,loc.att.geoc),JSON_TYPE_QATT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_geoc_s",-1,-1,offsetof(nodestruc,loc.att.geoc.s),JSON_TYPE_QUATERNION,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_geoc_v",-1,-1,offsetof(nodestruc,loc.att.geoc.v),JSON_TYPE_RVECTOR,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_geoc_a",-1,-1,offsetof(nodestruc,loc.att.geoc.a),JSON_TYPE_RVECTOR,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_lvlh",-1,-1,offsetof(nodestruc,loc.att.lvlh),JSON_TYPE_QATT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_lvlh_s",-1,-1,offsetof(nodestruc,loc.att.lvlh.s),JSON_TYPE_QUATERNION,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_lvlh_v",-1,-1,offsetof(nodestruc,loc.att.lvlh.v),JSON_TYPE_RVECTOR,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_lvlh_a",-1,-1,offsetof(nodestruc,loc.att.lvlh.a),JSON_TYPE_RVECTOR,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_selc",-1,-1,offsetof(nodestruc,loc.att.selc),JSON_TYPE_QATT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_selc_s",-1,-1,offsetof(nodestruc,loc.att.selc.s),JSON_TYPE_QUATERNION,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_selc_v",-1,-1,offsetof(nodestruc,loc.att.selc.v),JSON_TYPE_RVECTOR,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_att_selc_a",-1,-1,offsetof(nodestruc,loc.att.selc.a),JSON_TYPE_RVECTOR,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_loc_bearth",-1,-1,offsetof(nodestruc,loc.bearth),JSON_TYPE_RVECTOR,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_azfrom",-1,-1,offsetof(nodestruc,azfrom),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_azto",-1,-1,offsetof(nodestruc,azto),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_elfrom",-1,-1,offsetof(nodestruc,elfrom),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_elto",-1,-1,offsetof(nodestruc,elto),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_range",-1,-1,offsetof(nodestruc,range),JSON_TYPE_FLOAT,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"comp_cnt",-1,-1,offsetof(nodestruc,device_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"piece_cnt",-1,-1,offsetof(nodestruc,piece_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"port_cnt",-1,-1,offsetof(nodestruc,port_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"target_cnt",-1,-1,offsetof(nodestruc,target_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"node_glossary_cnt",-1,-1,offsetof(nodestruc,glossary_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_ant_cnt",-1,-1,offsetof(nodestruc,ant_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_batt_cnt",-1,-1,offsetof(nodestruc,batt_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_bus_cnt",-1,-1,offsetof(nodestruc,bus_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_cpu_cnt",-1,-1,offsetof(nodestruc,cpu_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_gps_cnt",-1,-1,offsetof(nodestruc,gps_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_htr_cnt",-1,-1,offsetof(nodestruc,htr_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_imu_cnt",-1,-1,offsetof(nodestruc,imu_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_mcc_cnt",-1,-1,offsetof(nodestruc,mcc_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_motr_cnt",-1,-1,offsetof(nodestruc,motr_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_mtr_cnt",-1,-1,offsetof(nodestruc,mtr_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_pload_cnt",-1,-1,offsetof(nodestruc,pload_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_prop_cnt",-1,-1,offsetof(nodestruc,prop_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_psen_cnt",-1,-1,offsetof(nodestruc,psen_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_rot_cnt",-1,-1,offsetof(nodestruc,rot_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_rw_cnt",-1,-1,offsetof(nodestruc,rw_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_rxr_cnt",-1,-1,offsetof(nodestruc,rxr_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_ssen_cnt",-1,-1,offsetof(nodestruc,ssen_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_strg_cnt",-1,-1,offsetof(nodestruc,strg_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_stt_cnt",-1,-1,offsetof(nodestruc,stt_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_suchi_cnt",-1,-1,offsetof(nodestruc,suchi_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_swch_cnt",-1,-1,offsetof(nodestruc,swch_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_tcu_cnt",-1,-1,offsetof(nodestruc,tcu_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_tcv_cnt",-1,-1,offsetof(nodestruc,tcv_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_thst_cnt",-1,-1,offsetof(nodestruc,thst_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_tsen_cnt",-1,-1,offsetof(nodestruc,tsen_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);
	json_addentry((char *)"device_txr_cnt",-1,-1,offsetof(nodestruc,txr_cnt),JSON_TYPE_UINT16,JSON_GROUP_NODE,cdata);

	return (cdata->jmapped);
}

//! Add piece entry.
/*! Add an entry for piece number i to the JSON Namespace map.
 \param i Piece number.
 \param cdata Pointer to cdata ::cosmosstruc.
	\return The current number of entries, if successful, 0 if the entry could not be
 */
uint16_t json_addpieceentry(uint16_t i, cosmosstruc *cdata)
{
	json_addentry((char *)"piece_name",i,-1,(ptrdiff_t)offsetof(piecestruc,name)+i*sizeof(piecestruc),JSON_TYPE_NAME,JSON_GROUP_PIECE,cdata);
	json_addentry((char *)"piece_type",i,-1,(ptrdiff_t)offsetof(piecestruc,type)+i*sizeof(piecestruc),JSON_TYPE_UINT16,JSON_GROUP_PIECE,cdata);
	json_addentry((char *)"piece_cidx",i,-1,(ptrdiff_t)offsetof(piecestruc,cidx)+i*sizeof(piecestruc),JSON_TYPE_UINT16,JSON_GROUP_PIECE,cdata);
	json_addentry((char *)"piece_mass",i,-1,(ptrdiff_t)offsetof(piecestruc,mass)+i*sizeof(piecestruc),JSON_TYPE_FLOAT,JSON_GROUP_PIECE,cdata);
	json_addentry((char *)"piece_emi",i,-1,(ptrdiff_t)offsetof(piecestruc,emi)+i*sizeof(piecestruc),JSON_TYPE_FLOAT,JSON_GROUP_PIECE,cdata);
	json_addentry((char *)"piece_abs",i,-1,(ptrdiff_t)offsetof(piecestruc,abs)+i*sizeof(piecestruc),JSON_TYPE_FLOAT,JSON_GROUP_PIECE,cdata);
	json_addentry((char *)"piece_dim",i,-1,(ptrdiff_t)offsetof(piecestruc,dim)+i*sizeof(piecestruc),JSON_TYPE_FLOAT,JSON_GROUP_PIECE,cdata);
	json_addentry((char *)"piece_hcap",i,-1,(ptrdiff_t)offsetof(piecestruc,hcap)+i*sizeof(piecestruc),JSON_TYPE_FLOAT,JSON_GROUP_PIECE,cdata);
	json_addentry((char *)"piece_hcon",i,-1,(ptrdiff_t)offsetof(piecestruc,hcon)+i*sizeof(piecestruc),JSON_TYPE_FLOAT,JSON_GROUP_PIECE,cdata);
	json_addentry((char *)"piece_area",i,-1,(ptrdiff_t)offsetof(piecestruc,area)+i*sizeof(piecestruc),JSON_TYPE_FLOAT,JSON_GROUP_PIECE,cdata);
	json_addentry((char *)"piece_com",i,-1,(ptrdiff_t)offsetof(piecestruc,centroid)+i*sizeof(piecestruc),JSON_TYPE_RVECTOR,JSON_GROUP_PIECE,cdata);
	json_addentry((char *)"piece_pnt_cnt",i,-1,(ptrdiff_t)offsetof(piecestruc,pnt_cnt)+i*sizeof(piecestruc),JSON_TYPE_UINT16,JSON_GROUP_PIECE,cdata);
	for (uint16_t j=0; j<MAXPNT; ++j)
	{
		json_addentry((char *)"piece_pnt",i,j,(ptrdiff_t)offsetof(piecestruc,points)+i*sizeof(piecestruc)+j*sizeof(rvector),JSON_TYPE_RVECTOR,JSON_GROUP_PIECE,cdata);
	}

	return (cdata->jmapped);
}

//! Add component entry.
/*! Add an entry for component number i to the JSON Namespace map.
 \param i Component number.
 \param cdata Pointer to cdata ::cosmosstruc.
	\return The current number of entries, if successful, 0 if the entry could not be
 */
uint16_t json_addcompentry(uint16_t i, cosmosstruc *cdata)
{
	json_addentry((char *)"comp_type",i,-1,(ptrdiff_t)offsetof(compstruc,type)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
	json_addentry((char *)"comp_model",i,-1,(ptrdiff_t)offsetof(compstruc,model)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
	json_addentry((char *)"comp_flag",i,-1,(ptrdiff_t)offsetof(compstruc,flag)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
	json_addentry((char *)"comp_cidx",i,-1,(ptrdiff_t)offsetof(compstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
	json_addentry((char *)"comp_didx",i,-1,(ptrdiff_t)offsetof(compstruc,didx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
	json_addentry((char *)"comp_pidx",i,-1,(ptrdiff_t)offsetof(compstruc,pidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
	json_addentry((char *)"comp_bidx",i,-1,(ptrdiff_t)offsetof(compstruc,bidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
	json_addentry((char *)"comp_namp",i,-1,(ptrdiff_t)offsetof(compstruc,namp)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
	json_addentry((char *)"comp_nvolt",i,-1,(ptrdiff_t)offsetof(compstruc,volt)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
	json_addentry((char *)"comp_amp",i,-1,(ptrdiff_t)offsetof(compstruc,amp)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
	json_addentry((char *)"comp_volt",i,-1,(ptrdiff_t)offsetof(compstruc,volt)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
	json_addentry((char *)"comp_temp",i,-1,(ptrdiff_t)offsetof(compstruc,temp)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
	json_addentry((char *)"comp_utc",i,-1,(ptrdiff_t)offsetof(compstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
	cdata->node.comp.push_back((compstruc *)&cdata->device[i].comp);

	return (cdata->jmapped);
}

//! Add device entry.
/*! Add entries specific to device number i to the JSON Namespace map.
 \param i Device number.
 \param cdata Pointer to ::cosmosstruc.
	\return The current number of entries, if successful, 0 if the entry could not be
 */
uint16_t json_adddeviceentry(uint16_t i, cosmosstruc *cdata)
{
	switch (cdata->device[i].comp.type)
	{
	case DEVICE_TYPE_PLOAD:
		json_addentry((char *)"device_pload_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(ploadstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_pload_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(ploadstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_pload_portidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(ploadstruc,portidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_pload_bps",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(ploadstruc,bps)+i*sizeof(devicestruc),JSON_TYPE_UINT32,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_pload_key_cnt",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(ploadstruc,key_cnt)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		for (uint16_t j=0; j<MAXPLOADKEYCNT; j++)
		{
			json_addentry((char *)"device_pload_keyidx",cdata->device[i].comp.didx,j,(ptrdiff_t)offsetof(ploadstruc,keyidx)+i*sizeof(devicestruc)+j*sizeof(uint16_t),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
			json_addentry((char *)"device_pload_keyval",cdata->device[i].comp.didx,j,(ptrdiff_t)offsetof(ploadstruc,keyval)+i*sizeof(devicestruc)+j*sizeof(float),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		}
		cdata->node.pload.push_back((ploadstruc *)&cdata->device[i].pload);
		cdata->node.pload_cnt = cdata->node.pload.size();
		break;
		//! Elevation and Azimuth Sun Sensor
	case DEVICE_TYPE_SSEN:
		json_addentry((char *)"device_ssen_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(ssenstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_ssen_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(ssenstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_ssen_align",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(ssenstruc,align)+i*sizeof(devicestruc),JSON_TYPE_QUATERNION,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_ssen_qva",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(ssenstruc,qva)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_ssen_qvb",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(ssenstruc,qvb)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_ssen_qvc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(ssenstruc,qvc)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_ssen_qvd",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(ssenstruc,qvd)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_ssen_azimuth",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(ssenstruc,azimuth)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_ssen_elevation",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(ssenstruc,elevation)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		cdata->node.ssen.push_back((ssenstruc *)&cdata->device[i].ssen);
		cdata->node.ssen_cnt = cdata->node.ssen.size();
		break;
		//! Inertial Measurement Unit
	case DEVICE_TYPE_IMU:
		json_addentry((char *)"device_imu_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(imustruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_imu_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(imustruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_imu_portidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(imustruc,portidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_imu_align",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(imustruc,align)+i*sizeof(devicestruc),JSON_TYPE_QUATERNION,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_imu_accel",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(imustruc,accel)+i*sizeof(devicestruc),JSON_TYPE_RVECTOR,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_imu_accel_x",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(imustruc,accel)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_imu_accel_y",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(imustruc,accel)+sizeof(double)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_imu_accel_z",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(imustruc,accel)+2*sizeof(double)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_imu_omega",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(imustruc,omega)+i*sizeof(devicestruc),JSON_TYPE_RVECTOR,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_imu_mag",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(imustruc,mag)+i*sizeof(devicestruc),JSON_TYPE_RVECTOR,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_imu_mag_x",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(imustruc,mag)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_imu_mag_y",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(imustruc,mag)+sizeof(double)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_imu_mag_z",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(imustruc,mag)+2*sizeof(double)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_imu_bdot",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(imustruc,bdot)+i*sizeof(devicestruc),JSON_TYPE_RVECTOR,JSON_GROUP_DEVICE,cdata);
		cdata->node.imu.push_back((imustruc *)&cdata->device[i].imu);
		cdata->node.imu_cnt = cdata->node.imu.size();
		break;
		//! Reaction Wheel
	case DEVICE_TYPE_RW:
		json_addentry((char *)"device_rw_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rwstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_rw_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rwstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_rw_portidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rwstruc,portidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_rw_align",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rwstruc,align)+i*sizeof(devicestruc),JSON_TYPE_QUATERNION,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_rw_mom",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rwstruc,mom)+i*sizeof(devicestruc),JSON_TYPE_RVECTOR,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_rw_mxomg",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rwstruc,mxomg)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_rw_mxalp",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rwstruc,mxalp)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_rw_tc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rwstruc,tc)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_rw_omg",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rwstruc,omg)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_rw_alp",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rwstruc,alp)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_rw_romg",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rwstruc,romg)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_rw_ralp",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rwstruc,ralp)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		cdata->node.rw.push_back((rwstruc *)&cdata->device[i].rw);
		cdata->node.rw_cnt = cdata->node.rw.size();
		break;
		//! Magnetic Torque Rod
	case DEVICE_TYPE_MTR:
		json_addentry((char *)"device_mtr_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(mtrstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_mtr_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(mtrstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_mtr_mxfld",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(mtrstruc,mxfld)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_mtr_tc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(mtrstruc,tc)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_mtr_align",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(mtrstruc,align)+i*sizeof(devicestruc),JSON_TYPE_QUATERNION,JSON_GROUP_DEVICE,cdata);
		for (uint16_t j=0; j<7; j++)
		{
			json_addentry((char *)"device_mtr_npoly",cdata->device[i].comp.didx,j,(ptrdiff_t)offsetof(mtrstruc,npoly)+i*sizeof(devicestruc)+j*sizeof(float),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
			json_addentry((char *)"device_mtr_ppoly",cdata->device[i].comp.didx,j,(ptrdiff_t)offsetof(mtrstruc,ppoly)+i*sizeof(devicestruc)+j*sizeof(float),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		}
		json_addentry((char *)"device_mtr_fld",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(mtrstruc,fld)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_mtr_rfld",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(mtrstruc,rfld)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_mtr_portidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(mtrstruc,portidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		cdata->node.mtr.push_back((mtrstruc *)&cdata->device[i].mtr);
		cdata->node.mtr_cnt = cdata->node.mtr.size();
		break;
		//! Processing Unit
	case DEVICE_TYPE_CPU:
		json_addentry((char *)"device_cpu_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(cpustruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_cpu_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(cpustruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_cpu_portidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(cpustruc,portidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_cpu_mxdsk",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(cpustruc,mxdsk)+i*sizeof(devicestruc),JSON_TYPE_UINT32,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_cpu_mxmem",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(cpustruc,mxmem)+i*sizeof(devicestruc),JSON_TYPE_UINT32,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_cpu_mxlod",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(cpustruc,mxlod)+i*sizeof(devicestruc),JSON_TYPE_UINT32,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_cpu_load",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(cpustruc,load)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_cpu_mem",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(cpustruc,mem)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_cpu_dsk",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(cpustruc,dsk)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		cdata->node.cpu.push_back((cpustruc *)&cdata->device[i].cpu);
		cdata->node.cpu_cnt = cdata->node.cpu.size();
		break;
		//! GPS Unit
	case DEVICE_TYPE_GPS:
		json_addentry((char *)"device_gps_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_portidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,portidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_pos",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,position)+i*sizeof(devicestruc),JSON_TYPE_RVECTOR,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_pos_x",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,position.col[0])+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_pos_y",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,position.col[1])+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_pos_z",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,position.col[2])+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_vel",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,velocity)+i*sizeof(devicestruc),JSON_TYPE_RVECTOR,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_vel_x",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,velocity.col[0])+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_vel_y",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,velocity.col[1])+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_vel_z",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,velocity.col[2])+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_pos_sd",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,position_sd)+i*sizeof(devicestruc),JSON_TYPE_RVECTOR,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_pos_sd_x",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,position_sd.col[0])+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_pos_sd_y",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,position_sd.col[1])+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_pos_sd_z",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,position_sd.col[2])+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_vel_sd",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,velocity_sd)+i*sizeof(devicestruc),JSON_TYPE_RVECTOR,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_vel_sd_x",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,velocity_sd.col[0])+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_vel_sd_y",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,velocity_sd.col[1])+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_gps_vel_sd_z",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(gpsstruc,velocity_sd.col[2])+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		cdata->node.gps.push_back((gpsstruc *)&cdata->device[i].gps);
		cdata->node.gps_cnt = cdata->node.gps.size();
		break;
		//! Antenna
	case DEVICE_TYPE_ANT:
		json_addentry((char *)"device_ant_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(antstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_ant_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(antstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_ant_align",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(antstruc,align)+i*sizeof(devicestruc),JSON_TYPE_QUATERNION,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_ant_minelev",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(antstruc,minelev)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		cdata->node.ant.push_back((antstruc *)&cdata->device[i].ant);
		cdata->node.ant_cnt = cdata->node.ant.size();
		break;
		//! Radio Receiver
	case DEVICE_TYPE_RXR:
		json_addentry((char *)"device_rxr_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rxrstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_rxr_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rxrstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_rxr_portidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rxrstruc,portidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		cdata->node.rxr.push_back((rxrstruc *)&cdata->device[i].rxr);
		cdata->node.rxr_cnt = cdata->node.rxr.size();
		break;
		//! Radio Transmitter
	case DEVICE_TYPE_TXR:
		json_addentry((char *)"device_txr_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(txrstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_txr_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(txrstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_txr_portidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(txrstruc,portidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		cdata->node.txr.push_back((txrstruc *)&cdata->device[i].txr);
		cdata->node.txr_cnt = cdata->node.txr.size();
		break;
		//! Radio Transceiver
	case DEVICE_TYPE_TCV:
		json_addentry((char *)"device_tcv_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(tcvstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_tcv_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(tcvstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_tcv_portidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(tcvstruc,portidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		cdata->node.tcv.push_back((tcvstruc *)&cdata->device[i].tcv);
		cdata->node.tcv_cnt = cdata->node.tcv.size();
		break;
		//! Photo Voltaic String
	case DEVICE_TYPE_STRG:
		json_addentry((char *)"device_strg_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(strgstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_strg_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(strgstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_strg_efi",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(strgstruc,effbase)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_strg_efs",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(strgstruc,effslope)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_strg_max",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(strgstruc,maxpower)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_strg_pow",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(strgstruc,power)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		cdata->node.strg.push_back((strgstruc *)&cdata->device[i].strg);
		cdata->node.strg_cnt = cdata->node.strg.size();
		break;
		//! Battery
	case DEVICE_TYPE_BATT:
		json_addentry((char *)"device_batt_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(battstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_batt_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(battstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_batt_cap",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(battstruc,capacity)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_batt_eff",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(battstruc,efficiency)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_batt_lev",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(battstruc,charge)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		cdata->node.batt.push_back((battstruc *)&cdata->device[i].batt);
		cdata->node.batt_cnt = cdata->node.batt.size();
		break;
		//! Heater
	case DEVICE_TYPE_HTR:
		json_addentry((char *)"device_htr_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(htrstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_htr_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(htrstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		cdata->node.htr.push_back((htrstruc *)&cdata->device[i].htr);
		cdata->node.htr_cnt = cdata->node.htr.size();
		break;
		//! Motor
	case DEVICE_TYPE_MOTR:
		json_addentry((char *)"device_motr_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(motrstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_motr_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(motrstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_motr_rat",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(motrstruc,rat)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_motr_max",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(motrstruc,max)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_motr_spd",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(motrstruc,spd)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		cdata->node.motr.push_back((motrstruc *)&cdata->device[i].motr);
		cdata->node.motr_cnt = cdata->node.motr.size();
		break;
		//! Pressure Sensor
	case DEVICE_TYPE_PSEN:
		json_addentry((char *)"device_psen_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(psenstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_psen_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(psenstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_psen_press",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(psenstruc,press)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		cdata->node.psen.push_back((psenstruc *)&cdata->device[i].psen);
		cdata->node.psen_cnt = cdata->node.psen.size();
		break;
		//! Temperature Sensor
	case DEVICE_TYPE_TSEN:
		json_addentry((char *)"device_tsen_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(tsenstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_tsen_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(tsenstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_tsen_temp",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(tsenstruc,temp)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		cdata->node.tsen.push_back((tsenstruc *)&cdata->device[i].tsen);
		cdata->node.tsen_cnt = cdata->node.tsen.size();
		break;
		//! Thruster
	case DEVICE_TYPE_THST:
		json_addentry((char *)"device_thst_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(thststruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_thst_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(thststruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_thst_isp",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(thststruc,isp)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_thst_align",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(thststruc,align)+i*sizeof(devicestruc),JSON_TYPE_QUATERNION,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_thst_flw",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(thststruc,flw)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		cdata->node.thst.push_back((thststruc *)&cdata->device[i].thst);
		cdata->node.thst_cnt = cdata->node.thst.size();
		break;
		//! Propellant Tank
	case DEVICE_TYPE_PROP:
		json_addentry((char *)"device_prop_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(propstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_prop_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(propstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_prop_cap",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(propstruc,cap)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_prop_lev",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(propstruc,lev)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		cdata->node.prop.push_back((propstruc *)&cdata->device[i].prop);
		cdata->node.prop_cnt = cdata->node.prop.size();
		break;
		//! Switch
	case DEVICE_TYPE_SWCH:
		json_addentry((char *)"device_swch_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(swchstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_swch_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(swchstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		cdata->node.swch.push_back((swchstruc *)&cdata->device[i].swch);
		cdata->node.swch_cnt = cdata->node.swch.size();
		break;
		//! Rotor
	case DEVICE_TYPE_ROT:
		json_addentry((char *)"device_rot_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rotstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_rot_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(rotstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		cdata->node.rot.push_back((rotstruc *)&cdata->device[i].rot);
		cdata->node.rot_cnt = cdata->node.rot.size();
		break;
		//! Star Tracker
	case DEVICE_TYPE_STT:
		json_addentry((char *)"device_stt_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(sttstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_stt_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(sttstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_stt_portidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(sttstruc,portidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_stt_align",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(sttstruc,align)+i*sizeof(devicestruc),JSON_TYPE_QUATERNION,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_stt_att",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(sttstruc,att)+i*sizeof(devicestruc),JSON_TYPE_QUATERNION,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_stt_omega",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(sttstruc,omega)+i*sizeof(devicestruc),JSON_TYPE_RVECTOR,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_stt_retcode",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(sttstruc,retcode)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_stt_status",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(sttstruc,status)+i*sizeof(devicestruc),JSON_TYPE_UINT32,JSON_GROUP_DEVICE,cdata);
		cdata->node.stt.push_back((sttstruc *)&cdata->device[i].stt);
		cdata->node.stt_cnt = cdata->node.stt.size();
		break;
		//! Star Tracker
	case DEVICE_TYPE_SUCHI:
		{
			json_addentry((char *)"device_suchi_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(suchistruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
			json_addentry((char *)"device_suchi_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(suchistruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
			json_addentry((char *)"device_suchi_portidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(suchistruc,portidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
			json_addentry((char *)"device_suchi_align",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(suchistruc,align)+i*sizeof(devicestruc),JSON_TYPE_QUATERNION,JSON_GROUP_DEVICE,cdata);
			json_addentry((char *)"device_suchi_press",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(suchistruc,press)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
			for (uint16_t j=0; j<8; j++)
			{
				json_addentry((char *)"device_suchi_temps",cdata->device[i].comp.didx,j,(ptrdiff_t)offsetof(suchistruc,temps)+i*sizeof(devicestruc)+j*sizeof(float),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
			}
			suchistruc* ptr = &cdata->device[i].suchi;
			cdata->node.suchi.push_back(ptr);
			cdata->node.suchi_cnt = cdata->node.suchi.size();
			break;
		}
	case DEVICE_TYPE_MCC:
		json_addentry((char *)"device_mcc_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(mccstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_mcc_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(mccstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_mcc_q",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(mccstruc,q)+i*sizeof(devicestruc),JSON_TYPE_QUATERNION,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_mcc_align",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(mccstruc,align)+i*sizeof(devicestruc),JSON_TYPE_QUATERNION,JSON_GROUP_DEVICE,cdata);
		cdata->node.mcc.push_back((mccstruc *)&cdata->device[i].mcc);
		cdata->node.mcc_cnt = cdata->node.mcc.size();
		break;
	case DEVICE_TYPE_TCU:
		json_addentry((char *)"device_tcu_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(tcustruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_tcu_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(tcustruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_tcu_portidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(tcustruc,portidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_tcu_mcnt",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(tcustruc,mcnt)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		for (uint16_t j=0; j<3; j++)
		{
			json_addentry((char *)"device_tcu_mcidx",cdata->device[i].comp.didx,j,(ptrdiff_t)offsetof(tcustruc,mcidx)+i*sizeof(devicestruc)+j*sizeof(int16_t),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		}
		cdata->node.tcu.push_back((tcustruc *)&cdata->device[i].tcu);
		cdata->node.tcu_cnt = cdata->node.tcu.size();
		break;
	case DEVICE_TYPE_BUS:
		json_addentry((char *)"device_bus_utc",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(busstruc,utc)+i*sizeof(devicestruc),JSON_TYPE_DOUBLE,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_bus_cidx",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(busstruc,cidx)+i*sizeof(devicestruc),JSON_TYPE_UINT16,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_bus_amp",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(busstruc,amp)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_bus_volt",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(busstruc,volt)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		json_addentry((char *)"device_bus_energy",cdata->device[i].comp.didx,-1,(ptrdiff_t)offsetof(busstruc,energy)+i*sizeof(devicestruc),JSON_TYPE_FLOAT,JSON_GROUP_DEVICE,cdata);
		cdata->node.bus.push_back((busstruc *)&cdata->device[i].bus);
		cdata->node.bus_cnt = cdata->node.bus.size();
		break;
	}

	return (cdata->jmapped);
}

//! Create JSON stream from wildcard
/*! Generate a JSON stream based on a character string representing
 * all the ::namespace names you wish to match.
	\param jstring Pointer to a string large enough to hold the end result.
	\param wildcard Character string representing a regular expression to be matched to all names in the ::namespace.
	\param cdata Pointer to cdata ::cosmosstruc to be used.
	\return Pointer to the string if successful, otherwise NULL.
*/
char *json_of_wildcard(jstring *jstring, char *wildcard, cosmosstruc *cdata)
{
	int32_t iretn;

	iretn = json_startout(jstring);
	if (iretn < 0)
		return (NULL);

	json_out_wildcard(jstring, wildcard, cdata);
	json_stopout(jstring);

	return (jstring->string);
}

//! Create JSON stream from list
/*! Generate a JSON stream based on a comma separated list of ::namespace names.
	\param jstring Pointer to a string large enough to hold the end result.
	\param cdata Pointer to cdata ::cosmosstruc to be used.
	\return Pointer to the string if successful, otherwise NULL.
*/
char *json_of_list(jstring *jstring, char *list, cosmosstruc *cdata)
{
	int32_t iretn;

	iretn = json_startout(jstring);
	if (iretn < 0)
		return (NULL);

	iretn = json_out_list(jstring,list,cdata);
	if (iretn < 0 && iretn != JSON_ERROR_EOS)
		return (NULL);

	json_stopout(jstring);

	return (jstring->string);
}

//! Create JSON Track string
/*! Generate a JSON stream showing the variables stored in an ::nodestruc.
	\param jstring Pointer to a string large enough to hold the end result.
 \param cdata->stat A pointer to the beginning of the ::cosmosstruc_s to use.
 \param cdata->dyn A pointer to the beginning of the ::cosmosstruc_d to use.
	\return Pointer to the string if successful, otherwise NULL.
*/
char *json_of_target(jstring *jstring, cosmosstruc *cdata, int32_t num)
{
	int32_t iretn;

	iretn = json_startout(jstring);
	if (iretn < 0)
		return (NULL);

	iretn = json_out_1d(jstring,(char *)"target_utc",num,cdata);
	iretn = json_out_1d(jstring,(char *)"target_name",num,cdata);
	iretn = json_out_1d(jstring,(char *)"target_type",num,cdata);
	iretn = json_out_1d(jstring,(char *)"target_azfrom",num,cdata);
	iretn = json_out_1d(jstring,(char *)"target_elfrom",num,cdata);
	iretn = json_out_1d(jstring,(char *)"target_azto",num,cdata);
	iretn = json_out_1d(jstring,(char *)"target_elto",num,cdata);
	iretn = json_out_1d(jstring,(char *)"target_min",num,cdata);
	iretn = json_out_1d(jstring,(char *)"target_range",num,cdata);
	iretn = json_out_1d(jstring,(char *)"target_close",num,cdata);
	iretn = json_out_1d(jstring,(char *)"target_loc_pos_geod",num,cdata);

	json_stopout(jstring);

	return (jstring->string);
}

//! Create JSON Info string
/*! Generate a JSON stream showing the variables stored in an ::infostruc.
	\param jstring Pointer to a string large enough to hold the end result.
 \param cdata->stat A pointer to the beginning of the ::cosmosstruc_s to use.
 \param cdata->dyn A pointer to the beginning of the ::cosmosstruc_d to use.
	\return Pointer to the string if successful, otherwise NULL.
*/
char *json_of_node(jstring *jstring, cosmosstruc *cdata)
{
	int32_t iretn;

	iretn = json_startout(jstring);
	if (iretn < 0)
		return (NULL);

	iretn = json_out(jstring,(char *)"node_utc",cdata);
	iretn = json_out(jstring,(char *)"node_name",cdata);
	iretn = json_out(jstring,(char *)"node_type",cdata);
	iretn = json_out(jstring,(char *)"node_mass",cdata);
	iretn = json_out(jstring,(char *)"node_moi",cdata);
	iretn = json_out(jstring,(char *)"node_battcap",cdata);
	iretn = json_out(jstring,(char *)"node_battlev",cdata);
	iretn = json_out(jstring,(char *)"node_powchg",cdata);
	iretn = json_out(jstring,(char *)"node_powgen",cdata);
	iretn = json_out(jstring,(char *)"node_powuse",cdata);
	iretn = json_out(jstring,(char *)"node_baryc",cdata);
	iretn = json_out(jstring,(char *)"node_icrf",cdata);

	json_stopout(jstring);

	return (jstring->string);
}

//! Create JSON Time string
/*! Generate a JSON stream showing the various internal times stored
 * in the global COSMOS data structure.
	\param jstring Pointer to a string large enough to hold the end
	result.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return Pointer to the string if successful, otherwise NULL.
*/
char *json_of_time(jstring *jstring, cosmosstruc *cdata)
{
	int32_t iretn;

	iretn = json_startout(jstring);
	if (iretn < 0)
		return (NULL);

	iretn = json_out(jstring,(char *)"node_utc",cdata);
	if (iretn < 0)
		return (NULL);
	iretn = json_out(jstring,(char *)"node_utcoffset",cdata);
	if (iretn < 0)
		return (NULL);
	json_stopout(jstring);

	return (jstring->string);
}

//! Create JSON Heart Beat string
/*! Create a complete JSON formatted Heartbeat string using the data currently in the global COSMOS
 * structure.
	\param jstring Pointer to a ::jstring structure to be used to build out the JSON string.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return Pointer to the string if successful, otherwise NULL.
*/
char *json_of_beat(jstring *jstring, cosmosstruc *cdata)
{
	int32_t iretn;

	iretn = json_startout(jstring);
	if (iretn < 0)
		return (NULL);
	iretn = json_out(jstring,(char *)"beat",cdata);
	if (iretn < 0)
		return (NULL);
	json_stopout(jstring);

	return (jstring->string);
}

//! Create JSON Beacon string
/*! Create a complete JSON formatted Beacon string using the data currently in the global COSMOS structure.
	\param jstring Pointer to a ::jstring structure to be used to build out the JSON string.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return Pointer to the string if successful, otherwise NULL.
*/
char *json_of_beacon(jstring *jstring, cosmosstruc *cdata)
{
	int32_t iretn;

	iretn = json_startout(jstring);
	if (iretn < 0)
		return (NULL);
	json_out(jstring,(char *)"node_name",cdata);
	json_out(jstring,(char *)"node_type",cdata);
	json_out(jstring,(char *)"node_utc",cdata);
	json_out(jstring,(char *)"node_utcoffset",cdata);
	json_out(jstring,(char *)"node_loc_pos_eci",cdata);
	json_out(jstring,(char *)"node_loc_att_icrf",cdata);
	json_out(jstring,(char *)"node_powgen",cdata);
	json_out(jstring,(char *)"node_powuse",cdata);
	json_out(jstring,(char *)"node_powchg",cdata);
	json_out(jstring,(char *)"node_battlev",cdata);
	json_stopout(jstring);

	return (jstring->string);
}

//! Create JSON IMU string
/*! Create a complete JSON formatted IMU string for the indicated IMU using the data currently in the global COSMOS structure.
	\param jstring Pointer to a ::jstring structure to be used to build out the JSON string.
	\param num Number of the IMU.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return Pointer to the string if successful, otherwise NULL.
*/
char *json_of_imu(jstring *jstring, uint16_t num, cosmosstruc *cdata)
{
	int32_t iretn;

	iretn = json_startout(jstring);
	if (iretn < 0)
		return (NULL);
	json_out_1d(jstring,(char *)"device_imu_att",num,cdata);
	json_out_1d(jstring,(char *)"device_imu_align",num,cdata);
	json_out_1d(jstring,(char *)"device_imu_cidx",num,cdata);
	json_out_1d(jstring,(char *)"device_imu_portidx",num,cdata);
	json_out_1d(jstring,(char *)"device_imu_cnt",num,cdata);
	json_out_1d(jstring,(char *)"device_imu_mag",num,cdata);
	json_out_1d(jstring,(char *)"device_imu_bdot",num,cdata);
	json_out_1d(jstring,(char *)"device_imu_pos",num,cdata);
	json_stopout(jstring);

	return (jstring->string);
}

char *json_of_ephemeris(jstring *jstring, cosmosstruc *cdata)
{

	json_startout(jstring);

	// Location
	//json_out(jstring,(char *)"timestamp",cdata);
	json_out(jstring,(char *)"node_utc",cdata);
	json_out(jstring,(char *)"node_utcoffset",cdata);
	json_out(jstring,(char *)"node_name",cdata);
	json_out(jstring,(char *)"node_type",cdata);
	json_out(jstring,(char *)"node_loc_pos_eci",cdata);

	json_stopout(jstring);
	return (jstring->string);
}

char *json_of_utc(jstring *jstring, cosmosstruc *cdata)
{
	json_startout(jstring);

	// Time
	json_out(jstring,(char *)"node_utc",cdata);
	json_out(jstring,(char *)"node_utcoffset",cdata);
	json_stopout(jstring);
	return (jstring->string);
}

char *json_of_soh(jstring *jstring, cosmosstruc *cdata)
{
	//	int16_t i, j;

	json_startout(jstring);

	// Time
	json_out(jstring,(char *)"physics_utcoffset",cdata);

	// Satellite Information
	json_out(jstring,(char *)"node_name",cdata);
	json_out(jstring,(char *)"node_type",cdata);
	json_out(jstring,(char *)"node_powgen",cdata);
	json_out(jstring,(char *)"node_powuse",cdata);
	json_out(jstring,(char *)"node_battlev",cdata);
	json_out(jstring,(char *)"node_battcap",cdata);
	json_out(jstring,(char *)"node_powchg",cdata);
	json_out(jstring,(char *)"node_thst",cdata);

	// Earth magnetic field
	json_out(jstring,(char *)"node_loc_bearth",cdata);

	// Position
	json_out(jstring,(char *)"node_loc_pos_eci",cdata);

	// Attitude
	json_out(jstring,(char *)"node_loc_att_icrf",cdata);

	json_out_wildcard(jstring, (char *)"device_*", cdata);

	json_stopout(jstring);
	return (jstring->string);
}

//! Create JSON for an event
/*! Generate a JSON stream that represents the current state of the
 * ::eventstruc in ::cosmosstruc.
	\param jstring User provided ::jstring for creating the JSON stream
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
	\return Pointer to the string created.
*/
char *json_of_event(jstring *jstring, cosmosstruc *cdata)
{
	json_startout(jstring);

	json_out(jstring,(char *)"event_utc",cdata);
	json_out(jstring,(char *)"event_utcexec",cdata);
	json_out(jstring,(char *)"event_node",cdata);
	json_out(jstring,(char *)"event_name",cdata);
	json_out(jstring,(char *)"event_user",cdata);
	json_out(jstring,(char *)"event_type",cdata);
	json_out(jstring,(char *)"event_flag",cdata);
	json_out(jstring,(char *)"event_data",cdata);
	json_out(jstring,(char *)"event_condition",cdata);
	if (json_get_double_name((char *)"event_dtime",cdata) != 0.) json_out(jstring,(char *)"event_dtime",cdata);
	if (json_get_double_name((char *)"event_ctime",cdata) != 0.) json_out(jstring,(char *)"event_ctime",cdata);
	if (json_get_double_name((char *)"event_denergy",cdata) != 0.) json_out(jstring,(char *)"event_denergy",cdata);
	if (json_get_double_name((char *)"event_cenergy",cdata) != 0.) json_out(jstring,(char *)"event_cenergy",cdata);
	if (json_get_double_name((char *)"event_dmass",cdata) != 0.) json_out(jstring,(char *)"event_dmass",cdata);
	if (json_get_double_name((char *)"event_cmass",cdata) != 0.) json_out(jstring,(char *)"event_cmass",cdata);
	if (json_get_double_name((char *)"event_dbytes",cdata) != 0.) json_out(jstring,(char *)"event_dbytes",cdata);
	if (json_get_double_name((char *)"event_cbytes",cdata) != 0.) json_out(jstring,(char *)"event_cbytes",cdata);

	json_stopout(jstring);
	return (jstring->string);
}

char *json_of_groundcontact(jstring *jstring, cosmosstruc *cdata)
{
	int16_t i;

	json_startout(jstring);

	//json_out(jstring,(char *)"timestamp",cdata);
	json_out(jstring,(char *)"node_utc",cdata);
	json_out(jstring,(char *)"node_utcoffset",cdata);
	json_out(jstring,(char *)"gs_cnt",cdata);
	for (i=0; i<*(int16_t *)json_ptrto((char *)"gs_cnt",cdata); i++)
	{
		json_out_1d(jstring,(char *)"gs_az",i,cdata);
		json_out_1d(jstring,(char *)"gs_el",i,cdata);
	}

	json_stopout(jstring);
	return (jstring->string);
}

char *json_of_mtr(jstring *jstring,uint16_t index, cosmosstruc *cdata)
{
	json_startout(jstring);
	//json_out(jstring,(char *)"timestamp",cdata);
	json_out(jstring,(char *)"node_utc",cdata);
	json_out(jstring,(char *)"node_utcoffset",cdata);

	json_out_1d(jstring,(char *)"device_mtr_cidx",index,cdata);
	json_out_1d(jstring,(char *)"device_mtr_mom",index,cdata);
	json_out_1d(jstring,(char *)"device_mtr_fld",index,cdata);
	json_out_1d(jstring,(char *)"device_mtr_rfld",index,cdata);

	json_stopout(jstring);
	return (jstring->string);
}


char *json_of_rw(jstring *jstring,uint16_t index, cosmosstruc *cdata)
{
	json_startout(jstring);
	//json_out(jstring,(char *)"timestamp",cdata);
	json_out(jstring,(char *)"node_utc",cdata);
	json_out(jstring,(char *)"node_utcoffset",cdata);

	json_out_1d(jstring,(char *)"device_rw_utc",index,cdata);
	json_out_1d(jstring,(char *)"device_rw_cidx",index,cdata);
	json_out_1d(jstring,(char *)"device_rw_portidx",index,cdata);
	json_out_1d(jstring,(char *)"device_rw_mom_x",index,cdata);
	json_out_1d(jstring,(char *)"device_rw_mom_y",index,cdata);
	json_out_1d(jstring,(char *)"device_rw_mom_z",index,cdata);
	json_out_1d(jstring,(char *)"device_rw_omg",index,cdata);
	json_out_1d(jstring,(char *)"device_rw_alp",index,cdata);
	json_out_1d(jstring,(char *)"device_rw_romg",index,cdata);
	json_out_1d(jstring,(char *)"device_rw_ralp",index,cdata);

	json_stopout(jstring);
	return (jstring->string);
}

char *json_of_statevec(jstring *jstring, cosmosstruc *cdata)
{
	json_startout(jstring);
	//json_out(jstring,(char *)"timestamp",cdata);
	json_out(jstring,(char *)"node_utc",cdata);
	json_out(jstring,(char *)"node_utcoffset",cdata);
	json_out(jstring,(char *)"node_loc_att_icrf",cdata);
	json_out(jstring,(char *)"node_loc_pos_eci",cdata);
	json_stopout(jstring);
	return (jstring->string);
}

//! Dump Static satellite data
/*! Create a JSON stream for all the static elements of the satellite. Does not include any
 * derivative data (eg. area).
 \param jstring Pointer to a ::jstring to build the JSON stream in.
 \param cdata A pointer to the beginning of the ::cosmosstruc to use.
 \return Pointer to the created JSON stream.
*/
char *json_node(jstring *jstring, cosmosstruc *cdata)
{
	uint16_t cnt;
	char tstring[COSMOS_MAX_NAME];

	json_startout(jstring);

	json_out(jstring,(char *)"node_name",cdata);
	json_out_character(jstring, '\n');
	json_out(jstring,(char *)"node_type",cdata);
	json_out_character(jstring, '\n');

	// Dump structures
	json_out(jstring,(char *)"piece_cnt",cdata);
	json_out_character(jstring, '\n');
	for (uint16_t i=0; i<*(int16_t *)json_ptrto((char *)"piece_cnt",cdata); i++)
	{
		json_out_1d(jstring,(char *)"piece_name",i,cdata);
		json_out_character(jstring, '\n');
		json_out_1d(jstring,(char *)"piece_type",i,cdata);
		json_out_character(jstring, '\n');
		json_out_1d(jstring,(char *)"piece_cidx",i,cdata);
		json_out_character(jstring, '\n');
		json_out_1d(jstring,(char *)"piece_mass",i,cdata);
		json_out_character(jstring, '\n');
		json_out_1d(jstring,(char *)"piece_emi",i,cdata);
		json_out_character(jstring, '\n');
		json_out_1d(jstring,(char *)"piece_abs",i,cdata);
		json_out_character(jstring, '\n');
		json_out_1d(jstring,(char *)"piece_hcap",i,cdata);
		json_out_character(jstring, '\n');
		json_out_1d(jstring,(char *)"piece_hcon",i,cdata);
		json_out_character(jstring, '\n');
		json_out_1d(jstring,(char *)"piece_dim",i,cdata);
		json_out_character(jstring, '\n');
		json_out_1d(jstring,(char *)"piece_pnt_cnt",i,cdata);
		json_out_character(jstring, '\n');
		cnt = json_get_int_name_1d((char *)"piece_pnt_cnt",i,cdata);
		for (uint16_t j=0; j<cnt; j++)
		{
			json_out_2d(jstring,(char *)"piece_pnt",i,j,cdata);
			json_out_character(jstring, '\n');
		}
	}

	// Dump components
	json_out(jstring,(char *)"comp_cnt",cdata);
	json_out_character(jstring, '\n');
	for (uint16_t i=0; i<*(int16_t *)json_ptrto((char *)"comp_cnt",cdata); i++)
	{
		json_out_1d(jstring,(char *)"comp_type",i,cdata);
		json_out_character(jstring, '\n');
		//		json_out_1d(jstring,(char *)"comp_model",i,cdata);
		//		json_out_character(jstring, '\n');
		json_out_1d(jstring,(char *)"comp_didx",i,cdata);
		json_out_character(jstring, '\n');
		json_out_1d(jstring,(char *)"comp_pidx",i,cdata);
		json_out_character(jstring, '\n');
		json_out_1d(jstring,(char *)"comp_bidx",i,cdata);
		json_out_character(jstring, '\n');
		json_out_1d(jstring,(char *)"comp_nvolt",i,cdata);
		json_out_character(jstring, '\n');
		json_out_1d(jstring,(char *)"comp_namp",i,cdata);
		json_out_character(jstring, '\n');
	}

	// Dump device specific info
	for (uint16_t i=0; i<DEVICE_TYPE_COUNT; ++i)
	{
		sprintf(tstring,"device_%s_cnt",comp_type_string[i]);
		if ((cnt=*(uint16_t *)json_ptrto(tstring,cdata)))
		{
			json_out(jstring,tstring,cdata);
			json_out_character(jstring, '\n');
			for (uint16_t j=0; j<cnt; ++j)
			{
				sprintf(tstring,"device_%s_cidx",comp_type_string[i]);
				json_out_1d(jstring,tstring,j,cdata);
				json_out_character(jstring, '\n');

				// Dump ploads
				if (!strcmp(comp_type_string[i],"pload"))
				{
					json_out_1d(jstring,(char *)"device_pload_portidx",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_pload_bps",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_pload_key_cnt",j,cdata);
					json_out_character(jstring, '\n');
					for (uint16_t k=0; k<json_get_int_name_1d((char *)"device_pload_key_cnt",j,cdata); ++k)
					{
						json_out_2d(jstring,(char *)"device_pload_key_name",j,k,cdata);
						json_out_character(jstring, '\n');
					}
				}

				// Dump Sun sensors
				if (!strcmp(comp_type_string[i],"ssen"))
				{
					json_out_1d(jstring,(char *)"device_ssen_align",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump IMU's
				if (!strcmp(comp_type_string[i],"imu"))
				{
					json_out_1d(jstring,(char *)"device_imu_portidx",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_imu_align",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump Reaction Wheel
				if (!strcmp(comp_type_string[i],"rw"))
				{
					json_out_1d(jstring,(char *)"device_rw_portidx",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_rw_align",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_rw_mom",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_rw_mxalp",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_rw_mxomg",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_rw_tc",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump Magtorque Rods
				if (!strcmp(comp_type_string[i],"mtr"))
				{
					json_out_1d(jstring,(char *)"device_mtr_mxfld",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_mtr_align",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_mtr_npoly",j,cdata);
					json_out_1d(jstring,(char *)"device_mtr_ppoly",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_mtr_tc",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump Computer systems
				if (!strcmp(comp_type_string[i],"cpu"))
				{
					json_out_1d(jstring,(char *)"device_cpu_portidx",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_cpu_mxdsk",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_cpu_mxmem",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_cpu_mxlod",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump GPS's
				if (!strcmp(comp_type_string[i],"gps"))
				{
					json_out_1d(jstring,(char *)"device_gps_portidx",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump Antennas
				if (!strcmp(comp_type_string[i],"ant"))
				{
					json_out_1d(jstring,(char *)"device_ant_align",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_ant_minelev",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump Receivers
				if (!strcmp(comp_type_string[i],"rxr"))
				{
					json_out_1d(jstring,(char *)"device_rxr_portidx",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump Transmitters
				if (!strcmp(comp_type_string[i],"txr"))
				{
					json_out_1d(jstring,(char *)"device_txr_portidx",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump Transceivers
				if (!strcmp(comp_type_string[i],"tcv"))
				{
					json_out_1d(jstring,(char *)"device_tcv_portidx",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump solar strings
				if (!strcmp(comp_type_string[i],"strg"))
				{
					json_out_1d(jstring,(char *)"device_strg_efi",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_strg_efs",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_strg_max",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump batteries
				if (!strcmp(comp_type_string[i],"batt"))
				{
					json_out_1d(jstring,(char *)"device_batt_cap",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_batt_eff",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump Heaters
				if (!strcmp(comp_type_string[i],"htr"))
				{
				}

				// Dump motors
				if (!strcmp(comp_type_string[i],"motr"))
				{
					json_out_1d(jstring,(char *)"device_motr_rat",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_motr_max",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump temperature sensor
				if (!strcmp(comp_type_string[i],"tsen"))
				{
				}

				// Dump thsters
				if (!strcmp(comp_type_string[i],"thst"))
				{
					json_out_1d(jstring,(char *)"device_thst_isp",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_thst_align",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump propellant tanks
				if (!strcmp(comp_type_string[i],"prop"))
				{
					json_out_1d(jstring,(char *)"device_prop_cap",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump Switch info
				if (!strcmp(comp_type_string[i],"swch"))
				{
				}

				// Dump Rotor info
				if (!strcmp(comp_type_string[i],"rot"))
				{
				}

				// Dump STT's
				if (!strcmp(comp_type_string[i],"stt"))
				{
					json_out_1d(jstring,(char *)"device_stt_portidx",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_stt_align",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump Motion Capture info
				if (!strcmp(comp_type_string[i],"mcc"))
				{
					json_out_1d(jstring,(char *)"device_mcc_portidx",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_mcc_align",j,cdata);
					json_out_character(jstring, '\n');
				}

				// Dump Torque Rod Control Unit info
				if (!strcmp(comp_type_string[i],"tcu"))
				{
					json_out_1d(jstring,(char *)"device_tcu_portidx",j,cdata);
					json_out_character(jstring, '\n');
					json_out_1d(jstring,(char *)"device_tcu_mcnt",j,cdata);
					json_out_character(jstring, '\n');
					for (uint16_t k=0; k<json_get_int_name_1d((char *)"device_tcu_mcnt",j,cdata); ++k)
					{
						json_out_2d(jstring,(char *)"device_tcu_mcidx",j,k,cdata);
						json_out_character(jstring, '\n');
					}
				}

				// Dump power bus
				if (!strcmp(comp_type_string[i],"bus"))
				{

				}

				// Dump pressure sensor
				if (!strcmp(comp_type_string[i],"psen"))
				{
				}

				// Dump SUCHI
				if (!strcmp(comp_type_string[i],"suchi"))
				{
					json_out_1d(jstring,(char *)"device_suchi_portidx",j,cdata);
					json_out_character(jstring, '\n');
				}
			}
		}
	}

	// Dump Port table
	json_out(jstring,(char *)"port_cnt",cdata);
	json_out_character(jstring, '\n');
	for (uint16_t i=0; i<*(int16_t *)json_ptrto((char *)"port_cnt",cdata); i++)
	{
		json_out_1d(jstring,(char *)"port_name",i,cdata);
		json_out_character(jstring, '\n');
		json_out_1d(jstring,(char *)"port_type",i,cdata);
		json_out_character(jstring, '\n');
	}

	json_stopout(jstring);
	return (jstring->string);
}

void json_test(cosmosstruc *cdata)
{
	uint16_t i, j;
	long hash;

	int32_t hashcount[1480]= {0};

	for (i=0; i<cdata->jmap.size(); ++i)
	{
		for (j=0; j<cdata->jmap[i].size(); ++j)
		{
			hash = json_hash(cdata->jmap[i][j].name);

			printf("%s %d %d %ld\n",cdata->jmap[i][j].name,i,j,hash);

			hashcount[hash]++;
		}

	}
}

//! Get hash and index in JSON Namespace map
/*! Using the provided name, find it's location in the provided Namespace map and set the
 * values for the hash and index.
	\param name Namespace name.
	\param cdata ::cosmosstruc containing map.
	\param handle Pointer to ::jsonhandle of name.
	\return Zero, or negative error number.
*/
int32_t json_name_map(char *name, cosmosstruc *cdata, jsonhandle *handle)
{

	if (!cdata || !cdata->jmapped)
		return (JSON_ERROR_NOJMAP);

	if (cdata->jmap.size() == 0)
		return (JSON_ERROR_NOJMAP);

	handle->hash = json_hash(name);

	for (handle->index=0; handle->index<cdata->jmap[handle->hash].size(); ++handle->index)
		if (!strcmp(name,cdata->jmap[handle->hash][handle->index].name))
			return (0);

	return (JSON_ERROR_NOENTRY);
}

//! Get hash and index in JSON Equation map
/*! Using the provided text, find it's location in the provided Equation map and set the
 * values for the hash and index. If the equation is not already in the table, add it.
	\param equation Equation text.
	\param cdata Pointer to ::cosmosstruc containing map.
	\param handle Pointer to ::jsonhandle of name.
	\return Zero, or negative error number.
*/
int32_t json_equation_map(char *equation, cosmosstruc *cdata, jsonhandle *handle)
{
	const char *pointer;
	jsonequation tequation;
	char ops[] = "+-*/&|><=!~";
	int32_t iretn;
	int32_t textlen;

	if (!cdata || !cdata->jmapped)
		return (JSON_ERROR_NOJMAP);

	if (cdata->emap.size() == 0)
		return (JSON_ERROR_NOJMAP);

	handle->hash = json_hash(equation);

	for (handle->index=0; handle->index<cdata->emap[handle->hash].size(); ++handle->index)
		if (!strcmp(equation,cdata->emap[handle->hash][handle->index].text))
			return (0);

	// Not found. We will have to add it to the map.
	pointer = &equation[0];

	if ((pointer)[0] != '(')
	{
		return (JSON_ERROR_SCAN);
	}

	(pointer)++;

	// Extract first operand
	if ((iretn=json_parse_operand(&pointer, &tequation.operand[0], cdata)) < 0)
	{
		return (JSON_ERROR_SCAN);
	}

	// Extract operation
	for (tequation.operation=0; tequation.operation<(int)strlen(ops); tequation.operation++)
	{
		if ((pointer)[0] == ops[tequation.operation])
			break;
	}
	if (tequation.operation == (int)strlen(ops))
		return (JSON_ERROR_SCAN);
	(pointer)++;

	// Extract second argument
	if ((iretn=json_parse_operand(&pointer, &tequation.operand[1], cdata)) < 0)
	{
		return (JSON_ERROR_SCAN);
	}

	// Remove trailing )
	while ((pointer)[0] != 0 && (pointer)[0] != ')')
		(pointer)++;
	if ((pointer)[0] != 0)
		(pointer)++;

	textlen = strlen(equation)+1;
	if ((tequation.text = (char *)calloc(1,textlen)) == NULL)
	{
		return (JSON_ERROR_SCAN);
	}

	// Populate the equation
	strcpy(tequation.text,equation);
	
	handle->index = cdata->emap[handle->hash].size();
	cdata->emap[handle->hash].push_back(tequation);
	if (cdata->emap[handle->hash].size() != handle->index+1)
	{
		free(tequation.text);
		return (JSON_ERROR_SCAN);
	}

	return (0);

}

//! Get cosmosstruc clone
/*! Get a pointer to a clone of the internally stored main ::cosmosstruc.
	\param clone Pointer to clone ::cosmosstruc.
	\return Zero, or negative error.
*/
int32_t json_clone(cosmosstruc *cdata)
{
	if (!cdata || !cdata->jmapped) return (JSON_ERROR_NOJMAP);

	cdata[1] = cdata[0];
	for (uint16_t i=0; i<cdata[1].node.device_cnt; ++i)
	{
		switch(cdata[1].device[i].comp.type)
		{
		case DEVICE_TYPE_PLOAD:
			cdata[1].node.pload[cdata[1].device[i].comp.didx] = &cdata[1].device[i].pload;
			break;
		case DEVICE_TYPE_SSEN:
			cdata[1].node.ssen[cdata[1].device[i].comp.didx] = &cdata[1].device[i].ssen;
			break;
		case DEVICE_TYPE_IMU:
			cdata[1].node.imu[cdata[1].device[i].comp.didx] = &cdata[1].device[i].imu;
			break;
		case DEVICE_TYPE_RW:
			cdata[1].node.rw[cdata[1].device[i].comp.didx] = &cdata[1].device[i].rw;
			break;
		case DEVICE_TYPE_MTR:
			cdata[1].node.mtr[cdata[1].device[i].comp.didx] = &cdata[1].device[i].mtr;
			break;
		case DEVICE_TYPE_CPU:
			cdata[1].node.cpu[cdata[1].device[i].comp.didx] = &cdata[1].device[i].cpu;
			break;
		case DEVICE_TYPE_GPS:
			cdata[1].node.gps[cdata[1].device[i].comp.didx] = &cdata[1].device[i].gps;
			break;
		case DEVICE_TYPE_ANT:
			cdata[1].node.ant[cdata[1].device[i].comp.didx] = &cdata[1].device[i].ant;
			break;
		case DEVICE_TYPE_RXR:
			cdata[1].node.rxr[cdata[1].device[i].comp.didx] = &cdata[1].device[i].rxr;
			break;
		case DEVICE_TYPE_TXR:
			cdata[1].node.txr[cdata[1].device[i].comp.didx] = &cdata[1].device[i].txr;
			break;
		case DEVICE_TYPE_TCV:
			cdata[1].node.tcv[cdata[1].device[i].comp.didx] = &cdata[1].device[i].tcv;
			break;
		case DEVICE_TYPE_STRG:
			cdata[1].node.strg[cdata[1].device[i].comp.didx] = &cdata[1].device[i].strg;
			break;
		case DEVICE_TYPE_BATT:
			cdata[1].node.batt[cdata[1].device[i].comp.didx] = &cdata[1].device[i].batt;
			break;
		case DEVICE_TYPE_HTR:
			cdata[1].node.htr[cdata[1].device[i].comp.didx] = &cdata[1].device[i].htr;
			break;
		case DEVICE_TYPE_MOTR:
			cdata[1].node.motr[cdata[1].device[i].comp.didx] = &cdata[1].device[i].motr;
			break;
		case DEVICE_TYPE_PSEN:
			cdata[1].node.psen[cdata[1].device[i].comp.didx] = &cdata[1].device[i].psen;
			break;
		case DEVICE_TYPE_TSEN:
			cdata[1].node.tsen[cdata[1].device[i].comp.didx] = &cdata[1].device[i].tsen;
			break;
		case DEVICE_TYPE_THST:
			cdata[1].node.thst[cdata[1].device[i].comp.didx] = &cdata[1].device[i].thst;
			break;
		case DEVICE_TYPE_PROP:
			cdata[1].node.prop[cdata[1].device[i].comp.didx] = &cdata[1].device[i].prop;
			break;
		case DEVICE_TYPE_SWCH:
			cdata[1].node.swch[cdata[1].device[i].comp.didx] = &cdata[1].device[i].swch;
			break;
		case DEVICE_TYPE_ROT:
			cdata[1].node.rot[cdata[1].device[i].comp.didx] = &cdata[1].device[i].rot;
			break;
		case DEVICE_TYPE_STT:
			cdata[1].node.stt[cdata[1].device[i].comp.didx] = &cdata[1].device[i].stt;
			break;
		case DEVICE_TYPE_MCC:
			cdata[1].node.mcc[cdata[1].device[i].comp.didx] = &cdata[1].device[i].mcc;
			break;
		case DEVICE_TYPE_TCU:
			cdata[1].node.tcu[cdata[1].device[i].comp.didx] = &cdata[1].device[i].tcu;
			break;
		case DEVICE_TYPE_BUS:
			cdata[1].node.bus[cdata[1].device[i].comp.didx] = &cdata[1].device[i].bus;
			break;
		case DEVICE_TYPE_SUCHI:
			cdata[1].node.suchi[cdata[1].device[i].comp.didx] = &cdata[1].device[i].suchi;
			break;
		}
	}


	return (0);
}

uint32_t json_get_name_list_count(cosmosstruc *cdata)
{
	if (cdata->jmapped == false) return 0;

	uint32_t count = 0;
	for (uint32_t i = 0; i < cdata->jmap.size(); i++)
	{
		for (uint32_t j = 0; j < cdata->jmap[i].size(); j++)
		{
			++count;
		}
	}
	return count;
}

//! @}

