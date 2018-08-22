/*! \file datalib.cpp
	\brief Data support functions
*/

#include "datalib.h"
#include "agentlib.h"
#include "nodelib.h"
#include "jsonlib.h"

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>


//static cosmosstruc cdata;
//char sname[100], ename[100];

//! \defgroup common_statics Common static variables
//! @{

//! @}

//! \ingroup data
//! \defgroup data.stat->cs Static variables for Data functions.
//! @{

//static vector<cosmosstruc> nodes;

//! Path to COSMOS Root directory
char rootdir[MAXDIR] = {0};
//! Path to COSMOS Resource directory
char resdir[MAXDIR] = {0};
//! Path to COSMOS Nodes directory
char nodedir[MAXDIR] = {0};
//! Path to current COSMOS Node directory
char cnodedir[MAXDIR] = {0};

//! @}


//! \ingroup data
//! \defgroup data_functions Data Management support functions
//! @{

void log_write(char *node, int type, double utc, char *record)
{
	FILE *fout;

	if (utc == 0.)
		return;

	switch (type)
	{
	case DATA_LOG_TYPE_SOH:
		fout = data_open(data_path(node,(char *)"temp", (char *)"soh", utc,(char *)"telemetry") ,(char *)"a+");
		break;
	case DATA_LOG_TYPE_EVENT:
		fout = data_open(data_path(node,(char *)"temp", (char *)"soh", utc,(char *)"event") ,(char *)"a+");
		break;
	default:
		fout = data_open(data_path(node,(char *)"temp", (char *)"soh", utc,(char *)"log") ,(char *)"a+");
		break;
	}

	fprintf(fout,"%s\n",record);
	fclose(fout);
}


int data_list_files(string node, string location, string agent, vector<filestruc>& files)
{
	string dtemp;
	DIR *jdp, *tdp;
	struct dirent *td;
	filestruc tf;

	tf.node = node;
	tf.agent = agent;
	dtemp = data_path(node, location, agent);
	if ((jdp=opendir(dtemp.c_str())) != NULL)
	{
		while ((td=readdir(jdp)) != NULL)
		{
			if (td->d_name[0] != '.')
			{
				tf.name = td->d_name;
				tf.path = dtemp + "/" + tf.name;
				if ((tdp=opendir(tf.path.c_str())) != NULL)
				{
					tf.type = "directory";
					closedir(tdp);
				}
				else
				{
					for (uint16_t i=strlen(td->d_name)-1; i<strlen(td->d_name); --i)
					{
						if (td->d_name[i] == '.')
						{
							tf.type = &td->d_name[i+1];
							break;
						}
					}
				}
				files.push_back(tf);
			}
		}
		closedir(jdp);
	}

	return (files.size());
}

int data_list_nodes(vector<cosmosstruc> &node)
{
	DIR *jdp;
	char dtemp[1024];
	char *rootd;
	struct dirent *td;
	cosmosstruc *tnode;

	if ((rootd=get_rootdir()) == NULL) return (NODE_ERROR_ROOTDIR);

	if ((tnode=json_create()) == NULL) return (NODE_ERROR_NODE);

	sprintf(dtemp,"%s/nodes",rootd);
	if ((jdp=opendir(dtemp)) != NULL)
	{
		while ((td=readdir(jdp)) != NULL)
		{
			if (td->d_name[0] != '.')
			{
				if (!node_init(td->d_name,tnode))
				{
					node.push_back(*tnode);
				}
			}
		}
		closedir(jdp);
	}
	return (0);
}

//! Create data file name
/*! Builds a filename up from the date of creation and its type. Format is:
*	yyyyjjjsssss.type, where yyyy is the Year, jjj is the Julian Day, sssss is
*	the Seconds, and type is any accepted COSMOS file type (eg. log, event,
*	telemetry, message, command.)
*	\param mjd UTC of creation date in Modified Julian Day
*	\param type Any valid extension type
*	\return Filename string, otherwise NULL
*/
string data_name(string node, double mjd, string extra, string type)
{
	string name;
	char ntemp[100];

	int year, month, seconds;
	double jday, day;

	mjd2ymd(mjd,&year,&month,&day,&jday);
	seconds = (int)(86400.*(jday-(int)jday));
	sprintf(ntemp,"_%04d%03d%05d",year,(int32_t)jday,seconds);
	if (extra.empty())
	{
		name = node + ntemp + "." + type;
	}
	else
	{
		name = node + ntemp + "_" + extra + "." + type;
	}
	return (name);
}

char *data_name(char *node, double mjd, char *type)
{
	static char dtemp[100];

	int year, month, seconds;
	double jday, day;

	mjd2ymd(mjd,&year,&month,&day,&jday);
	seconds = (int)(86400.*(jday-(int)jday));
	sprintf(dtemp,"%s_%04d%03d%05d.%s",node,year,(int32_t)jday,seconds,type);
	return (dtemp);
}

string data_path(string node, string type, string sub, string filename)
{

	string filepath = get_cnodedir((char*)node.c_str());
	if (COSMOS_MKDIR(filepath.c_str()) < 0 && errno != EEXIST)
	{
		return NULL;
	}

	filepath += "/" + type;
	if (COSMOS_MKDIR(filepath.c_str()) < 0 && errno != EEXIST)
	{
		return NULL;
	}

	filepath += "/" + sub;
	if (COSMOS_MKDIR(filepath.c_str()) < 0 && errno != EEXIST)
	{
		return NULL;
	}

	filepath += "/" + filename;
	return (filepath);

}

string data_path(string node, string location, string agent)
{
	string cnode;
	string path;
	static char dtemp[1024]="";

//	cnode = get_cnodedir((char*)node.c_str());
	cnode = get_rootdir();
	cnode += "/nodes/" + node;

	if (COSMOS_MKDIR(cnode.c_str()) < 0 && errno != EEXIST)
	{
		return NULL;
	}

	sprintf(dtemp,"%s/%s", cnode.c_str(), location.c_str());
	if (COSMOS_MKDIR(dtemp) < 0 && errno != EEXIST)
	{
		return NULL;
	}
	if (agent.empty())
	{
		sprintf(dtemp,"%s/%s",cnode.c_str(),location.c_str());
	}
	else
	{
		sprintf(dtemp,"%s/%s/%s", cnode.c_str(), location.c_str(), agent.c_str());
		if (COSMOS_MKDIR(dtemp) < 0 && errno != EEXIST)
		{
			return NULL;
		}
	}

	path = dtemp;
	return (path);

}

//! Create data file path
/*! Build a path to a data file using its filename and the current Node
 * directory.
 * \param node Node directory in ::cosmosroot.
 * \param location Subfolder in Node directory (outgoing, incoming, data, temp).
 * \param agent Task specific subfolder of location, if relevant
 * \param mjd UTC of creation date in Modified Julian Day
 * \param name Extra text to add to the full name.
 * \param type Any valid extension type
 * \return File path string, otherwise NULL
*/
string data_path(string node, string location, string agent, double mjd, string extra, string type)
{
	string dtemp;
	char ntemp[COSMOS_MAX_NAME];
	string cnode;

//	cnode = get_cnodedir(node.c_str());
	cnode = get_rootdir();
	cnode += "/nodes/" + node;

	if (COSMOS_MKDIR(cnode.c_str()) < 0 && errno != EEXIST)
	{
		return NULL;
	}

	switch (location[0])
	{
	case 'd':
		// data
		{
			dtemp = cnode + "/data";
			if (COSMOS_MKDIR(dtemp.c_str()) < 0 && errno != EEXIST)
			{
				return NULL;
			}
			int year, month;
			double jday, day;
			mjd2ymd(mjd,&year,&month,&day,&jday);
			sprintf(ntemp, "/%04d", year);
			dtemp += ntemp;
			if (COSMOS_MKDIR(dtemp.c_str()) < 0 && errno != EEXIST)
			{
				return NULL;
			}
			sprintf(ntemp, "/%03d", (int32_t)jday);
			dtemp += ntemp;
			if (COSMOS_MKDIR(dtemp.c_str()) < 0 && errno != EEXIST)
			{
				return NULL;
			}
			dtemp += "/" + data_name(node, mjd, extra, type);
			break;
		}
	default:
		{
			dtemp = cnode + "/" + location;
			if (COSMOS_MKDIR(dtemp.c_str()) < 0 && errno != EEXIST)
			{
				return NULL;
			}

			if (agent.empty())
			{
				dtemp += data_name(node, mjd, extra, type);
			}
			else
			{
				dtemp += "/" + agent;
				if (COSMOS_MKDIR(dtemp.c_str()) < 0 && errno != EEXIST)
				{
					return NULL;
				}
				dtemp += "/" + data_name(node, mjd, extra, type);
			}
			break;
		}
	}

	return (dtemp);
}

string data_path(string node, string location, string agent, double mjd, string type)
{
	string path;

	path = data_path(node.c_str(), location.c_str(), agent.c_str(), mjd, type.c_str());

	return (path);
}

//! Create data file path
/*! Build a path to a data file using its filename and the current Node
 * directory.
 * \param node Node directory in ::cosmosroot.
 * \param location Subfolder in Node directory (outgoing, incoming, data, temp).
 * \param agent Task specific subfolder of location, if relevant
*	\param mjd UTC of creation date in Modified Julian Day
*	\param type Any valid extension type
*	\return File path string, otherwise NULL
*/
char *data_path(char *node, char *location, char *agent, double mjd, char *type)
{
	static char dtemp[1024]="";
	string cnode;
	
	cnode = get_cnodedir(node);

	if (COSMOS_MKDIR(cnode.c_str()) < 0 && errno != EEXIST)
	{
		return NULL;
	}

	switch (location[0])
	{
	case 'd':
		// data
		{
			sprintf(dtemp,"%s/data", cnode.c_str());
			if (COSMOS_MKDIR(dtemp) < 0 && errno != EEXIST)
			{
				return NULL;
			}
			int year, month;
			double jday, day;
			mjd2ymd(mjd,&year,&month,&day,&jday);
			sprintf(dtemp,"%s/data/%04d",cnode.c_str(),year);
			if (COSMOS_MKDIR(dtemp) < 0 && errno != EEXIST)
			{
				return NULL;
			}
			sprintf(dtemp,"%s/data/%04d/%03d",cnode.c_str(),year,(int32_t)jday);
			if (COSMOS_MKDIR(dtemp) < 0 && errno != EEXIST)
			{
				return NULL;
			}
			sprintf(dtemp,"%s/data/%04d/%03d/%s",cnode.c_str(),year,(int32_t)jday,data_name(node,mjd,type));
			break;
		}
	default:
		{
			sprintf(dtemp,"%s/%s", cnode.c_str(), location);
			if (COSMOS_MKDIR(dtemp) < 0 && errno != EEXIST)
			{
				return NULL;
			}
			if (agent == NULL)
			{
				sprintf(dtemp,"%s/%s/%s",cnode.c_str(),location, data_name(node,mjd,type));
			}
			else
			{
				sprintf(dtemp,"%s/%s/%s", cnode.c_str(), location, agent);
				if (COSMOS_MKDIR(dtemp) < 0 && errno != EEXIST)
				{
					return NULL;
				}
				sprintf(dtemp,"%s/%s/%s/%s",cnode.c_str(),location, agent, data_name(node,mjd,type));
			}
			break;
		}
	}

	return (dtemp);
}

//! Open file from path.
/*! Attempt to open a file with an optional path. If a path is included, each
 * directory element will be created if it is missing. The final file will be
 * opened with the requested mode.
 * \param path Full path to file, absolute or relative.
 * \mode fopen style mode
 * \return fopen style file handle, or NULL if either a directory element can not be
 * created, or the file can not be opened.
 */

FILE *data_open(char *path, char *mode)
{
	char dtemp[1024];
	uint32_t index, dindex, length;
	FILE *tfd;

	length = strlen(path);
	for (dindex=length-1; dindex<length; --dindex)
	{
		if (path[dindex] == '/')
			break;
	}

	if (dindex < length)
	{
		for (index=0; index<=dindex; ++index)
		{
			if (path[index] == '/')
			{
				strncpy(dtemp, path, index+1);
				dtemp[index+1] = 0;
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
				if (mkdir(dtemp,00777))
#else
				if (mkdir(dtemp))
#endif
				{
					if (errno != EEXIST)
						return (NULL);
				}
			}
		}
	}

	if ((tfd = fopen(path,mode)) != NULL)
	{
		return (tfd);
	}

	return (NULL);
}

//! Set Root Directory
/*! Set the internal variable that points to where all COSMOS files
 * are stored.
	\param name Absolute or relative pathname of directory.
*/
void set_rootdir(char *name)
{
	strncpy(rootdir,name,MAXDIR);
}

//! Get Root Directory
/*! Get the internal variable that points to where all COSMOS files are
 * stored.
 * \return Pointer to character string containing path to Root, otherwise NULL.
*/
char *get_rootdir()
{
	char *troot;
	char dir1[50];
	int i;
	struct stat sbuf;

	if (!rootdir[0])
	{
		if ((troot=getenv("COSMOS_ROOTDIR")) != NULL)
		{
			strncpy(rootdir,troot,MAXDIR);
		}
		else
		{
			strcpy(rootdir,"../cosmosroot");
			for (i=0; i<4; i++)
			{
				sprintf(dir1,"%s/nodes",rootdir);
				if (stat(dir1,&sbuf) == 0)
				{
					break;
				}
				strcpy(dir1,rootdir);
				sprintf(rootdir,"../%s",dir1);
			}
		}

		sprintf(resdir,"%s/resources",get_rootdir());
		sprintf(nodedir,"%s/nodes",get_rootdir());
	}
	return ((char *)rootdir);
}

//! Set Resource Directory
/*! Set the internal variable that points to where resource files are stored for the various modeling
 * functions.
	\param name Absolute or relative pathname of directory.
*/
void set_resdir(char *name)
{
	strncpy(resdir,name,MAXDIR);
}

//! Get Resource Directory
/*! Get the internal variable that points to where resource files are
 * stored for the various modeling functions.
 * \return Pointer to character string containing path to Resources, otherwise NULL.
*/
char *get_resdir()
{

	if (!resdir[0])
		get_rootdir();

	return ((char *)resdir);
}

//! Get Node Directory
/*! Get the internal variable that points to where node files are
 * stored for the various modeling functions.
 * \return Pointer to character string containing path to Nodes, otherwise NULL.
*/
char *get_nodedir()
{
	if (!nodedir[0])
		get_rootdir();

	return ((char *)nodedir);
}

//! Get Current Node Directory
/*! Get the internal variable that points to where node files are
 * stored for the current Node.
 * \return Pointer to character string containing path to Node, otherwise NULL.
*/
char *get_cnodedir(const char *name)
{
	if (!nodedir[0])
		get_rootdir();

	return ((char *)set_cnodedir(name));
}

//! Set Current Node Directory
/*! Set the internal variable that points to where node files are
 * stored for the current Node.
 * \param node Name of current Node
 * \return Pointer to character string containing path to Node, otherwise NULL.
*/
char *set_cnodedir(const char *node)
{
	struct stat sbuf;

	if (!nodedir[0])
		get_rootdir();

	sprintf(cnodedir,"%s/%s",nodedir,node);
	if (stat(cnodedir,&sbuf) != 0)
	{
		cnodedir[0] = 0;
		return (NULL);
	}
	else
		return ((char *)cnodedir);
}


int data_load_archive(double mjd, vector<string> &telem, vector<string> &event, cosmosstruc *cdata)
{
	DIR *jdp;
	struct dirent *td;
	int year, month;
	double day, jday;
	int len, dlen;
	char dtemp[356];
	ifstream tfd;
	string tstring;

	telem.clear();
	event.clear();

	mjd = (int)mjd;
	mjd2ymd(mjd,&year,&month,&day,&jday);

	dlen = strlen(get_cnodedir(cdata->node.name)) + 29 + strlen(cdata->node.name);
	sprintf(dtemp,"%s/data/%4d/%03d",get_cnodedir(cdata->node.name),year,(int32_t)jday);
	if ((jdp=opendir(dtemp))!=NULL)
	{
		while ((td=readdir(jdp))!=NULL)
		{
			if (td->d_name[0] != '.')
			{
				sprintf(dtemp,"%s/data/%4d/%03d/%s",get_cnodedir(cdata->node.name),year,(int32_t)jday,td->d_name);
				if (((len=strlen(dtemp)) > dlen))
					tfd.open(dtemp);
				if (tfd.is_open())
				{
					while (getline(tfd,tstring))
					{
						switch (dtemp[dlen])
						{
						//! Telemetry file
						case 't':
							if (!strcmp(&dtemp[dlen],"telemetry"))
							{
								telem.push_back(tstring);
							}
							break;
							//! Event file
						case 'e':
							//! Log file
						case 'l':
							//! Command file
						case 'c':
							//! Message file
						case 'm':
							if (!strcmp(&dtemp[dlen],"event") || !strcmp(&dtemp[dlen],"log") || !strcmp(&dtemp[dlen],"command") || !strcmp(&dtemp[dlen],"message"))
							{
								event.push_back(tstring);
							}
							break;
						}
					}
					tfd.close();
				}
			}
		}
		closedir(jdp);

		return (0);
	}
	return (DATA_ERROR_ARCHIVE);
}

//! Find last day in archive
/*! Searches through data archives for this Node to find most recent
 * day for which data is available. This is then stored in lastday.
 \return MJD of last day in archive.
*/
double findlastday(char *name)
{
	DIR *jdp;
	struct dirent *td;
	int year, jday;
	char dtemp[356];
	struct tm mytm;
	time_t mytime;

	year = jday = 0;
	sprintf(dtemp,"%s/data",get_cnodedir(name));
	if ((jdp=opendir(dtemp))!=NULL)
	{
		while ((td=readdir(jdp))!=NULL)
		{
			if (td->d_name[0] != '.')
			{
				if (atol(td->d_name) > year)
					year = atol(td->d_name);
			}
		}
		closedir(jdp);
		sprintf(dtemp,"%s/data/%04d",get_cnodedir(name),year);
		if ((jdp=opendir(dtemp))!=NULL)
		{
			while ((td=readdir(jdp))!=NULL)
			{
				if (td->d_name[0] != '.')
				{
					if (atol(td->d_name) > jday)
						jday = atol(td->d_name);
				}
			}
			closedir(jdp);
		}
	}

	if (year == 0. || jday == 0.)
	{
		return (0.);
	}

	mytm.tm_year = year - 1900;
	mytm.tm_hour = mytm.tm_min = mytm.tm_sec = 0;
	mytm.tm_mon = mytm.tm_mday = mytm.tm_wday = 0;
	mytm.tm_mday = 1;
	mytm.tm_mon = 0;
	mytm.tm_isdst = 0;
	mytime = mktime(&mytm);
	mytime += (int)((jday-1) * 86400.);
#ifdef COSMOS_WIN_OS
	struct tm *temptm;
	temptm = localtime(&mytime);
	if(temptm!=NULL) {
		mytm = *temptm;
	}
#else
	localtime_r(&mytime,&mytm);
#endif
	return (cal2mjd2(year,mytm.tm_mon+1,mytm.tm_mday));
}

//! Find first day in archive
/*! Searches through data archives for this Node to find oldest
 * day for which data is available. This is then stored in firstday.
 \return MJD of last day in archive.
*/
double findfirstday(char *name)
{
	DIR *jdp;
	struct dirent *td;
	int year, jday;
	char dtemp[356];
	struct tm mytm;
	time_t mytime;

	year = jday = 9000;
	sprintf(dtemp,"%s/data",get_cnodedir(name));
	if ((jdp=opendir(dtemp))!=NULL)
	{
		while ((td=readdir(jdp))!=NULL)
		{
			if (td->d_name[0] != '.')
			{
				if (atol(td->d_name) < year)
					year = atol(td->d_name);
			}
		}
		closedir(jdp);
		sprintf(dtemp,"%s/data/%04d",get_cnodedir(name),year);
		if ((jdp=opendir(dtemp))!=NULL)
		{
			while ((td=readdir(jdp))!=NULL)
			{
				if (td->d_name[0] != '.')
				{
					if (atol(td->d_name) < jday)
						jday = atol(td->d_name);
				}
			}
			closedir(jdp);
		}
	}

	if (year == 9000. || jday == 9000.)
	{
		return (0.);
	}

	mytm.tm_year = year - 1900;
	mytm.tm_hour = mytm.tm_min = mytm.tm_sec = 0;
	mytm.tm_mon = mytm.tm_mday = mytm.tm_wday = 0;
	mytm.tm_mday = 1;
	mytm.tm_mon = 0;
	mytm.tm_isdst = 0;
	mytime = mktime(&mytm);
	mytime += (int)((jday-1) * 86400.);
#ifdef COSMOS_WIN_OS
	struct tm *temptm;
	temptm = localtime(&mytime);
	mytm = *temptm;
#else
	localtime_r(&mytime,&mytm);
#endif

	return (cal2mjd2(year,mytm.tm_mon+1,mytm.tm_mday));
}

//! Add to KML path
/*! Write a KML file to keep track of the path the node is following. Create the file if it doesn't alreay exist.
 * Append to it if it already exists.
 \param cdata COSMOS data structure.
 \param name Name of file to write.
 \return 0, otherwise negative error.
*/
int32_t kml_write(cosmosstruc *cdata)
{
	char buf[500];
	FILE *fin, *fout;
	double utc;

	utc = floor(cdata->node.loc.utc);

	fin = data_open(data_path(cdata->node.name, (char *)"outgoing", (char *)"google", utc,(char *)"points"), (char *)"a+");
	fprintf(fin,"%.5f,%.5f,%.5f\n",DEGOF(cdata->node.loc.pos.geod.s.lon),DEGOF(cdata->node.loc.pos.geod.s.lat),cdata->node.loc.pos.geod.s.h);

	fout = data_open(data_path(cdata->node.name,(char *)"outgoing",(char *)"google",  utc,(char *)"kml"), (char *)"w");
	fprintf(fout,"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");
	fprintf(fout,"<Document>\n");
	fprintf(fout,"<name>%s JD%5.0f</name>\n",cdata->node.name,utc);
	fprintf(fout,"<description>Track of node.</description>\n");
	fprintf(fout,"<Style id=\"yellowLineGreenPoly\">\n<LineStyle>\n<color>7f00ffff</color>\n<width>4</width>\n</LineStyle>\n");
	fprintf(fout,"<PolyStyle>\n<color>7f00ff00</color>\n</PolyStyle>\n</Style>\n");
	fprintf(fout,"<Placemark>\n<name>Node Path</name>\n<description>%s JD%5.0f</description>\n",cdata->node.name,utc);
	fprintf(fout,"<styleUrl>#yellowLineGreenPoly</styleUrl>\n<LineString>\n<extrude>1</extrude>\n<tessellate>1</tessellate>\n<altitudeMode>absolute</altitudeMode>\n");
	fprintf(fout,"<coordinates>\n");

	rewind (fin);
	while (fgets(buf, 500, fin) != NULL)
	{
		fputs(buf, fout);
	}
	fclose(fin);
	
	fprintf(fout,"</coordinates>\n</LineString>\n</Placemark>\n</Document>\n</kml>\n");
	fclose(fout);

	return (0);
}
//! @}
