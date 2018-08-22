#ifndef _NODELIB_H
#define _NODELIB_H 1

#include "configCosmos.h"

#include "cosmos-defs.h"
#include "nodedef.h"
#include "jsondef.h"
#include "datalib.h"

/*! \file nodelib.h
*	\brief Node Library header file
*/

/*! \defgroup nodelib Node Library
* @{
* Node support library.
*
* The Node is the highest level physical entity in the COSMOS system. Every COSMOS
* installation will consist of a number of Nodes, all interacting through their Agents
* (as described in the \ref agentlib). Each Node has a detailed description, stored both as
* structures in memory, and as elements of the \ref jsonlib_namespace. These descriptions
* are broadly separated into two categories: static and dynamic. The ::cosmosstruc_s
* structure contains information about physical parts, as well as general information,
* such as the Name of the Node. The ::cosmosstruc_d structure contains the active
* telemetry of the Node, along with a Timestamp.
*
* Every physical thing in COSMOS is part of a Node. All Nodal actions
* are implemented through Agents, as described in the \ref agentlib.
* Interactions between Nodes are always expressed as interactions
* between Node:Agent pairings. Thus, Messages and Requests will be
* addressed to names expressed as Node:Agent (though wildcards are in
* same case supportedl eg. Node:* or *:Agent).
* @}
*/


//! \ingroup nodelib
//! \defgroup nodelib_functions Node Library functions
//! @{

int16_t node_init(const char *name, cosmosstruc *data);
int16_t node_calc(cosmosstruc *data);
void create_databases(cosmosstruc *root);
void load_databases(char *name, uint16_t type, cosmosstruc *root);
int load_dictionary(vector<shorteventstruc> &dict, cosmosstruc *root, char *file);
int load_target(cosmosstruc *root);
int update_target(cosmosstruc *root);
uint32_t calc_events(vector<shorteventstruc> &dictionary, cosmosstruc *root, vector<shorteventstruc> &events);

//! @}

#endif
