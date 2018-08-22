#ifndef _DATALIB_H
#define _DATALIB_H 1

#include "configCosmos.h"
#include "cosmos-errno.h"
#include "datadef.h"
#include "jsondef.h"

/*! \file datalib.h
*	\brief Data Management header file
*/

/*! \defgroup data Data Management support library
* @{
* Data Management support library.
*
* Data within COSMOS is managed in a heirarchical structure that
* mirrors the Node:Agent arrangement laid down in the \ref nodelib.
* Directories are defined for storage of general use information, as
* well as information specific to each Node. Separate directories are
* also defined for Incoming data, Outgoing data, and Archival data.
*
* At the highest level is a top level directory, "cosmosroot". This
* directory can be discovered by the software, if it is within 3
* levels above the current directory. It can also be set through use
* of the environment variable, "COSMOS_ROOTDIR". Within "cosmosroot"
* is a directory named "Resources", for any general use data, and a
* directory named "Nodes", for and Node specific data.
*
* "resources" contains directories of files for use in different
* aspects of COSMOS. The directory "general" contains files of
* coefficients used for the various models used in COSMOS simulations.
* The directory "logo" contains any special images used by COSMOS
* software. The directory "mapping" contains all the DEM's and
* ancillary files used by the \ref demlib. Finally. the directory "qt"
* contains files, such as UI forms, used by the various Qt based Tools
* withing COSMOS.
*
* "nodes" contains all data specific to each Node. There is a
* directory named after each Node. Within each Node directory is a
* file called "node.ini" which contains the static description of that
* Node, as well as a directory named after each Agent that is active
* (or has been active) for that Node. In its turn, each Agent
* directory has subdirectories for "incoming" files, "outgoing" files,
* and "archive" files. The "archive" directory is further subdivided by first
* year, then day.
*
* 
*/


//! @}

//! \ingroup datalib
//! \defgroup data_constants Data Management Support Library constants
//! @{

#define MAXDIR 256

//! @}

//! \ingroup datalib
//! \defgroup datalib_functions Data Management support library function declarations
//! @{

void log_reopen();
void log_write(char *node,int type,double utc, char *data);
int check_events(eventstruc *events, int max, cosmosstruc *data);
int data_list_nodes(vector<cosmosstruc> &data);
int data_list_files(string node, string location, string agent, vector<filestruc>& files);
FILE *data_open(char *path, char *mode);
string data_name(string node, double mjd, string extra, string type);
char *data_name(char *node, double mjd, char *type);
char *data_path(char *node, char *location, char *agent, double mjd, char *type);
string data_path(string node, string location, string agent, double mjd, string extra, string type);
string data_path(string node, string location, string agent, double mjd, string type);
string data_path(string node, string location, string agent);
string data_path(string node, string type, string sub, string filename);
char *get_resdir();
char *get_rootdir();
char *get_nodedir();
char *get_cnodedir(const char *node);
char *set_cnodedir(const char *node);
int data_load_archive(double mjd, vector<string> &telem, vector<string> &event, cosmosstruc *root);
double findlastday(char *node);
double findfirstday(char *node);
int32_t kml_write(cosmosstruc *cdata);

//! @}

#endif
