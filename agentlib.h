#ifndef _AGENTLIB_H
#define _AGENTLIB_H 1

#include "configCosmos.h"
#include "agentdef.h"

#include "cosmos-errno.h"
#include "stringlib.h"
//#include "timelib.h"
#include "jsondef.h"
#include "sliplib.h"

#ifdef _WIN32
//#define uInt32 unsigned int
#define int32_t int
#endif

/*! \file agentlib.h
*	\brief Agent Server and Client header file
*/

/*! \defgroup agentlib Agent Server and Client Library
* @{
* Agent Server and Client support library.
*
* Agents are persistent programs that provide the system framework for any
* COSMOS implementation. They are similar to UNIX Daemons or Windows Services in that
* they run continuously until commanded to stop, and serve a constant function, either
* automatically or on demand.
*
* All Agents provide at least the following:
*
* - A "Heartbeat", delivered to a system specified Multicast (::AGENTMCAST), or Broadcast address as a JSON packet,
* at some regular interval, that identifies the name of the Agent, the IP address and
* Port at which it can be reached, and the size of its communication buffer.
*
* - A UDP based "Request" channel, available on a predetermined IP Address and Port.
* Requests are received as plain text commands and arguments. They are processed and
* any response is sent back. The response, even if empty, always ends with [OK], if understood,
* or [NOK] if not. Requests and their responses must be less than the size of the communications
* buffer.
*
* - Support for three built in requests.
*	- "help" - list available requests for this Agent.
* 	- "shutdown" - causes the Agent to stop what it is doing and exit.
* 	- "status" - causes the Agent to dump any \ref jsonlib variables it is monitoring.
* 	- "getvalue {\"json_name_000\"[,\"json_name_001\"]}" - requests the Agent to return the values
* of the indicated JSON names.
*	- "setvalue {\"json_name_000\":value0[,\"json_name_001\":value1]}" - requests the Agent to set
* the values of the indicated JSON names.
* 	- "echo utc crc nbytes bytes" - requests the Agent to echo the local time the request was received,
* the CRC calculated for the bytes, and the bytes themselves.
*
* @}
*/

#ifdef COSMOS_WIN_OS
//#include <iostream.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <iphlpapi.h>
//#include "mmsystem.h"
#include <io.h>
#else
#include <sys/select.h>
//#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

//#include <errno.h>
//#include <pthread.h>
//#include <unistd.h>
#include <sys/time.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/types.h>
//#include <cstring>

//#include <string>

using namespace std;

//! \ingroup agentlib
//! \defgroup agentlib_functions Agent Server and Client Library functions
//! @{

int agent_add_request(cosmosstruc *cdata, const char *token, agent_request_function function);
int agent_add_request(cosmosstruc *cdata, const char *token, agent_request_function function, string description);
int agent_add_request(cosmosstruc *cdata, const char *token, agent_request_function function, string synopsis, string description);
int agent_start(cosmosstruc *cdata);
int32_t agent_send_request(cosmosstruc *cdata, beatstruc cbeat, char* request, char* output, uint32_t clen, float waitsec);
int agent_get_server(cosmosstruc *cdata, const char *node, const char *name, float waitsec, beatstruc *cbeat);
vector<beatstruc> agent_find_servers(cosmosstruc *cdata, float waitsec);
uint16_t agent_running(cosmosstruc *cdata);
int32_t agent_req_forward(char *request, char* response, void *root);
int32_t agent_req_echo(char *request, char* response, void *root);
int32_t agent_req_help(char *request, char* response, void *root);
int32_t agent_req_shutdown(char *request, char* response, void *root);
int32_t agent_req_status(char *request, char* response, void *root);
int32_t agent_req_getvalue(char *request, char* response, void *root);
int32_t agent_req_setvalue(char *request, char* response, void *root);
cosmosstruc *agent_setup_server(int ntype, const char *node, const char *name, double bprd, int32_t port, uint32_t bsize);
cosmosstruc *agent_setup_server(int ntype, const char *node, const char *name, double bprd, int32_t port, uint32_t bsize, bool multiflag);
cosmosstruc *agent_setup_client(int ntype, const char *node);
cosmosstruc *agent_setup_client(int ntype, const char *node, uint32_t usectimeo);
void agent_shutdown_server(cosmosstruc *cdata);
int agent_set_sohstring(cosmosstruc *cdata, char *list);
cosmosstruc *agent_get_cosmosstruc(cosmosstruc *cdata);
void agent_get_ip(cosmosstruc *cdata, char* buffer, size_t buflen);
void agent_get_ip_list(cosmosstruc *cdata, uint16_t port);
int agent_unpublish(cosmosstruc *cdata);
int agent_post(cosmosstruc *cdata, uint8_t type, char *message);
int agent_publish(cosmosstruc *cdata, uint16_t type, uint16_t port);
int agent_subscribe(cosmosstruc *cdata, uint16_t type, char *address, uint16_t port);
int agent_subscribe(cosmosstruc *cdata, uint16_t type, char *address, uint16_t port, uint32_t usectimeo);
int agent_unsubscribe(cosmosstruc *cdata);
int agent_poll(cosmosstruc *cdata, char *message, uint16_t bsize, uint8_t type, float waitsec);
timestruc agent_poll_time(cosmosstruc *cdata, float waitsec);
beatstruc agent_poll_beat(cosmosstruc *cdata, float waitsec);
locstruc agent_poll_location(cosmosstruc *cdata, float waitsec);
nodestruc agent_poll_info(cosmosstruc *cdata, float waitsec);
imustruc agent_poll_imu(cosmosstruc *cdata, float waitsec);
int json_map_agentstruc(cosmosstruc *cdata, agentstruc **agent);
int agent_open_socket(agent_channel *channel, uint16_t ntype, const char *address, uint16_t port, uint16_t direction, bool blocking, uint32_t usectimeo);
vector<agent_channel> agent_find_addresses(uint16_t ntype);

//! @}


#endif
