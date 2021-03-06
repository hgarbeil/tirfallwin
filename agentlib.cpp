/*! \file agentlib.cpp
	\brief Agent support functions
*/

#include "agentlib.h"
#include "jsonlib.h"
#include "nodelib.h"
#include <errno.h>
//#include <sys/types.h>
//#include <unistd.h>

#include "configCosmos.h"

#ifndef _SIZEOF_ADDR_IFREQ
#define _SIZEOF_ADDR_IFREQ sizeof
#endif

//! \ingroup agentlib
//! \defgroup agentlib_statics Agent Server and Client static variables
//! @{

jstring hbjstring={0,0,0};
static vector<beatstruc> slist;
agent_request_structure ireqs = {7,{
									 {"help",agent_req_help,"","list of available requests for this agent"},
									 {"shutdown",agent_req_shutdown,"","request to shutdown this agent"},
									 {"status",agent_req_status,"","request the status of this agent"},
									 {"getvalue",agent_req_getvalue,"{\"name1\",\"name2\",...}","get specified value(s) from agent"},
									 {"setvalue",agent_req_setvalue,"{\"name1\":value},{\"name2\":value},...}","set specified value(s) in agent"},
									 {"forward",agent_req_forward,"nbytes packet","Broadcast JSON packet to the default SEND port on local network"},
									 {"echo",agent_req_echo,"utc crc nbytes bytes","echo array of nbytes bytes, sent at time utc, with CRC crc."},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""},
									 {"",NULL,"",""}}};

#include<iostream>
using namespace std;

//! Handle for request thread
//static thread cthread;
//! Handle for heartbeat thread
//static thread hthread;
//! @}

void heartbeat_loop(cosmosstruc *cdata);
void request_loop(cosmosstruc *cdata);
char * agent_parse_request(char *input);

//! \ingroup agentlib
//! \defgroup agentlib_functions Agent Server and Client functions
//! @{

//! Add request to Agent request list
/*! Adds access to the indicated function by way of the given token. The provided
* function will be called with the request string as its argument. Any result will then
* be returned in the output pointer.
	\param cdata Pointer to ::cosmosstruc to use.
	\param token A string of maximum length ::COSMOS_MAX_NAME containing the token
	for the request. This should be the first word in the request.
	\param function The user supplied function to parse the specified request.
	\return Error, if any, otherwise zero.
*/
int agent_add_request(cosmosstruc *cdata, const char *token, agent_request_function function)
{
	if (cdata->agent[0].reqs.count > AGENTMAXREQUESTCOUNT)
		return (AGENT_ERROR_REQ_COUNT);

	strcpy(cdata->agent[0].reqs.request[cdata->agent[0].reqs.count].token,token);
	cdata->agent[0].reqs.request[cdata->agent[0].reqs.count].function = function;
	cdata->agent[0].reqs.request[cdata->agent[0].reqs.count].description="";
	cdata->agent[0].reqs.count++;
	return (0);
}

int agent_add_request(cosmosstruc *cdata, const char *token, agent_request_function function, string description)
{
	if (cdata->agent[0].reqs.count > AGENTMAXREQUESTCOUNT)
		return (AGENT_ERROR_REQ_COUNT);

	strcpy(cdata->agent[0].reqs.request[cdata->agent[0].reqs.count].token,token);
	cdata->agent[0].reqs.request[cdata->agent[0].reqs.count].function = function;
	cdata->agent[0].reqs.request[cdata->agent[0].reqs.count].description = description;
	cdata->agent[0].reqs.count++;
	return (0);
}

int agent_add_request(cosmosstruc *cdata, const char *token, agent_request_function function, string synopsis, string description)
{
	if (cdata->agent[0].reqs.count > AGENTMAXREQUESTCOUNT)
		return (AGENT_ERROR_REQ_COUNT);

	strcpy(cdata->agent[0].reqs.request[cdata->agent[0].reqs.count].token,token);
	cdata->agent[0].reqs.request[cdata->agent[0].reqs.count].function = function;
	cdata->agent[0].reqs.request[cdata->agent[0].reqs.count].synopsis = synopsis;
	cdata->agent[0].reqs.request[cdata->agent[0].reqs.count].description = description;
	cdata->agent[0].reqs.count++;
	return (0);
}

//! Start Agent Request and Heartbeat loops
/*!	Starts the request and heartbeat threads for an Agent server initialized with
 * ::agent_setup_server. The Agent will open its request and heartbeat channels using the supplied
 * address and port. The heartbeat will cycle with the requested period.
	\param cdata Pointer to ::cosmosstruc to use.
	\return value returned by request thread create
*/
int agent_start(cosmosstruc *cdata)
{

	// start heartbeat thread
    //hthread = thread(heartbeat_loop, cdata);
    //cthread = thread(request_loop, cdata);
	return 0;
}

//! Prepare Agent client
/*! This is the first function to call when setting up a program as an Agent client. It establishes the
 * Subscription channel for collecting messages and sets up the Name Space for the requested Node.
	\param ntype Network type.
	\param node Name of Node.
	\param usectimeo Blocking read timeout in micro seconds.
	\return Pointer to ::cosmosstruc to be used for all other calls, otherwise NULL.
*/
cosmosstruc *agent_setup_client(int ntype, const char *node, uint32_t usectimeo)
{
	int iretn;
	cosmosstruc *cdata;

	// Initialize COSMOS data space
	if ((cdata = json_create()) == NULL)
	{
		return(NULL);
	}

	//! Next, set up node.
	if (node != NULL && (iretn=node_init(node, cdata)))
	{
		json_destroy(cdata);
		return (NULL);
	}

	if (node == NULL) strcpy(cdata->node.name,"null");

	// Establish subscribe channel
	iretn = agent_subscribe(cdata, ntype,(char *)AGENTMCAST,AGENTSENDPORT, usectimeo);
	if (iretn)
	{
		json_destroy(cdata);
		return(NULL);
	}

	// Finally, make copies
	if ((iretn=json_clone(cdata)))
	{
		json_destroy(cdata);
		return(NULL);
	}

	cdata[0].agent[0].client = 1;
	cdata[1].agent[0].client = 1;
	return (cdata);
}

cosmosstruc *agent_setup_client(int ntype, const char *node)
{
	cosmosstruc *cdata;

	if ((cdata=agent_setup_client(ntype, node, 1000)) == NULL)
	{
		return (NULL);
	}

	return (cdata);
}

//! Prepare Agent server
/*! This is the first function to call when setting up an Agent. It first sets the Agent up as
 * a client, and checks that no other copies of the Agent are running. It then establishes the name
 * and connection information for the Agent, the frequency of the heartbeat, and the
 * size of the largest block that can be transferred. It initializes the built in
 * requests and makes it so the user can add their own requests.
	\param ntype The type of network medium.
	\param node The Node this Agent is associated with.
	\param name The name of the Agent.
	\param bprd The period of the heartbeat loop in sec.
	\param port The port number for contacting the Agent.
	\param bsize Size of transfer buffer.
	\param agent ::agentstruc in which to store all the Agent information.
	\return Pointer to ::cosmosstruc, otherwise NULL.
*/
cosmosstruc *agent_setup_server(int ntype, const char *node, const char *name, double bprd, int32_t port, uint32_t bsize)
{
	return (agent_setup_server(ntype, node, name, bprd, port, bsize, AGENT_SINGLE));
}

//! Prepare Agent server
/*! This is the first function to call when setting up an Agent. It establishes the name
 * and connection information for the Agent, the frequency of the heartbeat, and the
 * size of the largest block that can be transferred. It also initializes the buit in
 * requests and makes it so the user can add their own requests. It
 * also initializes a ::cosmosstruc, and the Data Space associated with
 * it. If the multiflag is set to false, then if another Agent of the same Node
 * and Name is discovered, the Agent will  not start and an error will be
 * returned. If multiflag is set to true, a 3 digit number of the form
 * "_ddd" will be appended to the name. The routine will then keep
 * trying until it finds no conflict.
	\param ntype The type of network medium.
	\param node The Node this Agent is associated with.
	\param name The name of the Agent.
	\param bprd The period of the heartbeat loop in sec.
	\param port The port number for contacting the Agent.
	\param bsize Size of transfer buffer.
	\param agent ::agentstruc in which to store all the Agent information.
	\param multiflag Boolean for whether to start multiple copies.
	\return Error number, otherwise zero.
*/
cosmosstruc *agent_setup_server(int ntype, const char *node, const char *name, double bprd, int32_t port, uint32_t bsize, bool multiflag)
{
	int iretn;
	char tname[COSMOS_MAX_NAME];
	cosmosstruc *cdata;

	//! First, see if we can become a Client, as all Servers are also Clients.
	if ((cdata = agent_setup_client(ntype, node, 1000)) == NULL)
	{
		return (NULL);
	}

	//! Next, check if this Agent is already running
	if (!multiflag)
	{
		if (strlen(cdata->node.name)>COSMOS_MAX_NAME || strlen(name)>COSMOS_MAX_NAME || agent_get_server(cdata, cdata->node.name, name, 4, (beatstruc *)NULL))
		{
			json_destroy(cdata);
			return (NULL);
		}
		strcpy(tname,name);
	}
	else
	{
		if (strlen(cdata->node.name)>COSMOS_MAX_NAME-4 || strlen(name)>COSMOS_MAX_NAME-4)
		{
			json_destroy(cdata);
			return (NULL);
		}

		uint32_t i=0;
		do
		{
			sprintf(tname,"%s_%03d",name,i);
			if (!agent_get_server(cdata, cdata->node.name, tname, 4, (beatstruc *)NULL))
			{
				break;
			}
		} while (++i<100);
	}

	// Initialize important server variables

	strncpy(cdata->agent[0].beat.node,cdata->node.name,COSMOS_MAX_NAME);
	strncpy(cdata->agent[0].beat.proc,tname,COSMOS_MAX_NAME);
	cdata->agent[0].beat.ntype = ntype;
	if (bprd >= .1)
		cdata->agent[0].beat.bprd = bprd;
	else
		cdata->agent[0].beat.bprd = .1;
	cdata->agent[0].stateflag = (uint16_t)AGENT_STATE_INIT;
	cdata->agent[0].beat.port = (uint16_t)port;
	cdata->agent[0].beat.bsz = (bsize<=AGENTMAXBUFFER-4?bsize:AGENTMAXBUFFER-4);
	cdata->agent[0].pid = getpid();
	cdata->agent[0].aprd = 1.;
	strncpy(cdata->agent[0].beat.user,"cosmos",COSMOS_MAX_NAME);
	cdata->agent[0].sohstring[0] = 0;

	// Establish publish channel
	iretn = agent_publish(cdata, ntype,AGENTSENDPORT);
	if (iretn)
	{
		agent_unsubscribe(cdata);
		json_destroy(cdata);
		return (NULL);
	}

	// Start the heartbeat and request threads running
	iretn = agent_start(cdata);
	if (iretn)
	{
		agent_unsubscribe(cdata);
		agent_unpublish(cdata);
		json_destroy(cdata);
		return (NULL);
	}

	//! Set up initial requests
	for (uint32_t i=0; i<ireqs.count; ++i)
	{
		cdata->agent[0].reqs.request[i] = ireqs.request[i];
	}
	cdata->agent[0].reqs.count = ireqs.count;

	cdata->agent[0].server = 1;
	return (cdata);
}

//! Shutdown server gracefully
/*! Waits for heartbeat and request loops to stop running before pulling the rug out from under them.
 * \param cdata Pointer to ::cosmosstruc to use.
 */
void agent_shutdown_server(cosmosstruc *cdata)
{
    //hthread.join();
    //cthread.join();
	agent_unsubscribe(cdata);
	agent_unpublish(cdata);
	json_destroy(cdata);
}

//! Check if we're supposed to be running
/*!	Returns the value of the internal variable that indicates that
 * the threads are running.
	\param cdata Pointer to ::cosmosstruc to use.
	\return True or False, depending on internal "run" variable
*/
uint16_t agent_running(cosmosstruc *cdata)
{
	return (cdata->agent[0].stateflag);
}

//! Send a request over AGENT
/*! Send a request string to the process at the provided address
	\param cdata Pointer to ::cosmosstruc to use.
	\param hbeat The agent ::beatstruc
	\param request the request and its arguments
	\param output any output returned by the request
	\param clen the size of the return buffer
	\param waitsec Maximum number of seconds to wait
	\return Either the number of bytes returned, or an error number.
*/
int32_t agent_send_request(cosmosstruc *, beatstruc hbeat, char* request, char* output, uint32_t clen, float waitsec)
{
	static agent_channel sendchan;
	int iretn;
	int32_t nbytes;
	struct timeval tv, ltv;

	if (hbeat.utc == 0. || hbeat.addr == 0 || hbeat.port == 0)
		return (AGENT_ERROR_SOCKET);

	gettimeofday(&ltv,NULL);

	ltv.tv_usec += 1000000 *(waitsec - (int)waitsec);
	ltv.tv_sec += (int)waitsec + (int)(ltv.tv_usec/1000000);
	ltv.tv_usec = ltv.tv_usec % 1000000;

	if ((iretn=agent_open_socket(&sendchan, AGENT_TYPE_UDP, hbeat.addr, hbeat.port, AGENT_TALK, AGENT_BLOCKING, AGENTRCVTIMEO)) < 0)
	{
		return (-errno);
	}

	if (strlen(request) > (uint16_t)hbeat.bsz)
		request[hbeat.bsz] = 0;
	nbytes = strlen(request);
	if ((nbytes=sendto(sendchan.cudp,request,nbytes,0,(struct sockaddr *)&sendchan.caddr,sizeof(struct sockaddr_in))) < 0)
	{
		CLOSE_SOCKET(sendchan.cudp);
#ifdef COSMOS_WIN_OS
		//		closesocket(sendchan.cudp);
		return(-WSAGetLastError());
#else
		//		close(sendchan.cudp);
		return (-errno);
#endif
	}


	do
	{

		nbytes = recvfrom(sendchan.cudp,output,clen,0,(struct sockaddr *)NULL,(socklen_t *)NULL);

		gettimeofday(&tv,NULL);

	} while (nbytes <= 0 && !(tv.tv_sec > ltv.tv_sec || (tv.tv_sec == ltv.tv_sec && tv.tv_usec > ltv.tv_usec)));
	output[nbytes] = 0;

	CLOSE_SOCKET(sendchan.cudp);




	return (nbytes);
}

//! Get specific server.
/*! Listen to the multicast/broadcast traffic for a set amount of time,
 * waiting for a specific named server to appear, then return its
 * heartbeat.
	\param cdata Pointer to ::cosmosstruc to use.
	\param node Node for the server.
	\param name Name of the server.
	\param waitsec Maximum number of seconds to wait
	\param rbeat pointer to a location to store the heartbeat
	\return 1 if found, otherwise 0, or an error number
*/
int agent_get_server(cosmosstruc *cdata, const char *node, const char *name, float waitsec, beatstruc *rbeat)
{
	beatstruc cbeat;
	struct timeval tv, ltv;

	//! 3. Loop for ::waitsec seconds, filling list with any discovered heartbeats.

	gettimeofday(&ltv,NULL);
	ltv.tv_usec += 1000000 *(waitsec - (int)waitsec);
	ltv.tv_sec += (int)waitsec + (int)(ltv.tv_usec/1000000);
	ltv.tv_usec = ltv.tv_usec % 1000000;
	do
	{
		cbeat = agent_poll_beat(cdata, 1);

		if (cbeat.utc != 0.)
		{

			if (!strcmp(cbeat.proc,name) && !strcmp(cbeat.node,node))
			{
				if (rbeat != NULL)
					*rbeat = cbeat;
				return (1);
			}
		}

		gettimeofday(&tv,NULL);

	} while (!(tv.tv_sec > ltv.tv_sec || (tv.tv_sec == ltv.tv_sec && tv.tv_usec > ltv.tv_usec)));

	return(0);
}


//! Generate a list of request servers.
/*! Listen to the multicast network for a set amount of time,
 * collecting heartbeats. Return a list of heartbeats collected.
	\param cdata Pointer to ::cosmosstruc to use.
	\param waitsec Maximum number of seconds to wait.
	\return A vector of ::beatstruc entries listing the unique servers found.
*/
vector<beatstruc> agent_find_servers(cosmosstruc *cdata, float waitsec)
{
	beatstruc cbeat, tbeat;
	uint32_t i, j;
	struct timeval tv, ltv;

	//! Loop for ::waitsec seconds, filling list with any discovered heartbeats.

	gettimeofday(&ltv,NULL);

	ltv.tv_usec += 1000000 *(waitsec - (int)waitsec);
	ltv.tv_sec += (int)waitsec + (int)(ltv.tv_usec/1000000);
	ltv.tv_usec = ltv.tv_usec % 1000000;

	do
	{
		cbeat = agent_poll_beat(cdata, 1);
		if (cbeat.utc != 0.)
		{
			for (i=0; i<slist.size(); i++)
			{
				if (!strcmp(cbeat.node,slist[i].node) && !strcmp(cbeat.proc,slist[i].proc))
					break;
			}
			if (i == slist.size())
			{
				slist.push_back(cbeat);
				for (j=i; j>0; j--)
				{
					if (slist[j].port > slist[j-1].port)
						break;
					tbeat = slist[j];
					slist[j] = slist[j-1];
					slist[j-1] = tbeat;
				}
			}
		}

		gettimeofday(&tv,NULL);

	} while (!(tv.tv_sec > ltv.tv_sec || (tv.tv_sec == ltv.tv_sec && tv.tv_usec > ltv.tv_usec)));

	return(slist);
}

//! Set SOH string
/*! Set the SOH string to a JSON list of \ref jsonlib_namespace names. A
 * proper JSON list will begin and end with matched curly braces, be comma separated,
 * and have all strings in double quotes.
	\param cdata Pointer to ::cosmosstruc to use.
	\param list Properly formatted list of JSON names.
	\return 0, otherwise a negative error.
*/
int agent_set_sohstring(cosmosstruc *cdata, char *list)
{
	strncpy(cdata->agent[0].sohstring,list,AGENTMAXBUFFER);
	return (0);
}

//! Return Agent ::cosmosstruc
/*! Return a pointer to the Agent's internal copy of the ::cosmosstruc.
	\param cdata Pointer to ::cosmosstruc to use.
	\return A pointer to the ::cosmosstruc, otherwise NULL.
*/
cosmosstruc *agent_get_cosmosstruc(cosmosstruc *cdata)
{
	return (cdata);
}

//! Heartbeat Loop
/*! This function is run as a thread to provide the Heartbeat for the Agent. The Heartbeat will
 * consist of the contents of ::AGENT_MESSAGE_BEAT in ::agent_poll, plus the contents of the
 * ::sohstring. It will come every ::bprd seconds.
 */
void heartbeat_loop(cosmosstruc *cdata)
{
	double cmjd, nmjd;
	unsigned long usec;
	struct timeval mytime;

	gettimeofday(&mytime, NULL);
	cmjd = (mytime.tv_sec - 1280000000) + mytime.tv_usec / 1e6;
	nmjd = cmjd + ((cosmosstruc *)cdata)->agent[0].beat.bprd;

	while (((cosmosstruc *)cdata)->agent[0].stateflag)
	{
		((cosmosstruc *)cdata)->agent[0].beat.utc = currentmjd(0.);
		if (((cosmosstruc *)cdata)->agent[0].sohstring[0] != 0)
		{
			agent_post(((cosmosstruc *)cdata), AGENT_MESSAGE_BEAT,json_of_list(&hbjstring,((cosmosstruc *)cdata)->agent[0].sohstring,((cosmosstruc *)cdata)));
		}
		else
		{
			agent_post(((cosmosstruc *)cdata), AGENT_MESSAGE_BEAT,(char *)"");
		}

		gettimeofday(&mytime, NULL);
		cmjd = (mytime.tv_sec - 1280000000) + mytime.tv_usec / 1e6;
		if (nmjd >= cmjd)
		{
			usec = (unsigned long)((nmjd-cmjd)*1e6+.5);
			COSMOS_USLEEP(usec);
		}

		if (((cosmosstruc *)cdata)->agent[0].beat.bprd < .1)
		{
			((cosmosstruc *)cdata)->agent[0].beat.bprd = .1;
		}
		nmjd += ((cosmosstruc *)cdata)->agent[0].beat.bprd;
	}
	agent_unpublish(((cosmosstruc *)cdata));
}

//! Request Loop
/*! This function is run as a thread to service requests to the Agent. It receives requests on
 * it assigned port number, matches the first word of the request against its set of requests,
 * and then either performs the matched function, or returns [NOK].
 */
void request_loop(cosmosstruc *cdata)
{
	char ebuffer[6]="[NOK]";
	int iretn, nbytes;
	char *bufferin, *bufferout;
	char request[AGENTMAXBUFFER+1];
	uint32_t i;

	if ((iretn=agent_open_socket(&((cosmosstruc *)cdata)->agent[0].req,AGENT_TYPE_UDP,(char *)"",((cosmosstruc *)cdata)->agent[0].beat.port,AGENT_LISTEN,AGENT_BLOCKING,2000000)) < 0)
	{
		return;
	}

	((cosmosstruc *)cdata)->agent[0].beat.port = ((cosmosstruc *)cdata)->agent[0].req.cport;

	if ((bufferin=(char *)calloc(1,((cosmosstruc *)cdata)->agent[0].beat.bsz)) == NULL)
	{
		iretn = -errno;
		return;
	}
	//	((cosmosstruc *)cdata)->agent[0].req.addrlen = sizeof(struct sockaddr_in);

	while (((cosmosstruc *)cdata)->agent[0].stateflag)
	{
		iretn = recvfrom(((cosmosstruc *)cdata)->agent[0].req.cudp,bufferin,((cosmosstruc *)cdata)->agent[0].beat.bsz,0,(struct sockaddr *)&((cosmosstruc *)cdata)->agent[0].req.caddr,(socklen_t *)&((cosmosstruc *)cdata)->agent[0].req.addrlen);

		if (iretn > 0)
		{
			bufferin[iretn] = 0;

			if (((cosmosstruc *)cdata)->agent[0].stateflag == AGENT_STATE_DEBUG)
			{
				printf("Request: [%d] %s ",iretn,bufferin);
			}

			for (i=0; i<COSMOS_MAX_NAME; i++)
			{
				if (bufferin[i] == ' ' || bufferin[i] == 0)
					break;
				request[i] = bufferin[i];
			}
			request[i] = 0;

			for (i=0; i<((cosmosstruc *)cdata)->agent[0].reqs.count; i++)
			{
				if (!strcmp(request,((cosmosstruc *)cdata)->agent[0].reqs.request[i].token))
					break;
			}

			if (i < ((cosmosstruc *)cdata)->agent[0].reqs.count)
			{
				iretn = ((cosmosstruc *)cdata)->agent[0].reqs.request[i].function(bufferin,request,((cosmosstruc *)cdata));
				if (iretn >= 0)
					bufferout = (char *)&request;
				else
					bufferout = NULL;
			}
			else
			{
				iretn = AGENT_ERROR_NULL;
				bufferout = NULL;
			}

			if (bufferout == NULL)
			{
				bufferout = ebuffer;
			}
			else
			{
				strcat(bufferout,"[OK]");
				bufferout[((cosmosstruc *)cdata)->agent[0].beat.bsz+3] = 0;
			}
			nbytes = sendto(((cosmosstruc *)cdata)->agent[0].req.cudp,bufferout,strlen(bufferout),0,(struct sockaddr *)&((cosmosstruc *)cdata)->agent[0].req.caddr,sizeof(struct sockaddr_in));
			if (((cosmosstruc *)cdata)->agent[0].stateflag == AGENT_STATE_DEBUG)
			{
				printf("[%d] %s\n",nbytes,bufferout);
			}
		}
	}
	free(bufferin);
	return;
}

//! Built-in Forward request
/*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 */
int32_t agent_req_forward(char* request, char* output, void *cdata)
{
	uint16_t count;
	int32_t iretn;

	sscanf(request,"%*s %hu",&count);
	for (uint16_t i=0; i<((cosmosstruc *)cdata)->agent[0].ifcnt; ++i)
	{
		iretn = sendto(((cosmosstruc *)cdata)->agent[0].pub[i].cudp,(const char *)&request[strlen(request)-count],count,0,(struct sockaddr *)&((cosmosstruc *)cdata)->agent[0].pub[i].caddr,sizeof(struct sockaddr_in));
	}
	sprintf(output,"%.17g %d ",currentmjd(0),iretn);
	return(0);
}

int32_t agent_req_echo(char* request, char* output, void *)
{
	double mjd;
	uint16_t crc, count;

	sscanf(request,"%*s %lf %hx %hu",&mjd,&crc,&count);
	sprintf(output,"%.17g %x %u ",currentmjd(0),slip_calc_crc((uint8_t *)&request[strlen(request)-count],count),count);
	strncpy(&output[strlen(output)],&request[strlen(request)-count],count+1);
	return(0);
}

int32_t agent_req_help(char*, char* output, void *cdata)
{
	string help_string;
	help_string += "\n";
	for(uint32_t i = 0; i < ((cosmosstruc *)cdata)->agent[0].reqs.count; ++i)
	{
		help_string += "        ";
		help_string += ((cosmosstruc *)cdata)->agent[0].reqs.request[i].token;
		help_string += " ";
		help_string += ((cosmosstruc *)cdata)->agent[0].reqs.request[i].synopsis;
		help_string += "\n";
		//size_t blanks = (20 - (signed int)strlen(((cosmosstruc *)cdata)->agent[0].reqs.request[i].token)) > 0 ? 20 - strlen(((cosmosstruc *)cdata)->agent[0].reqs.request[i].token) : 4;
		//string blank(blanks,' ');
		//help_string += blank;
		help_string += "                ";
		help_string += ((cosmosstruc *)cdata)->agent[0].reqs.request[i].description;
		help_string += "\n\n";
	}
	help_string += "\n";
	strcpy(output, (char*)help_string.c_str());
	return 0;
}

int32_t agent_req_shutdown(char*, char* output, void *cdata)
{
	((cosmosstruc *)cdata)->agent[0].stateflag = AGENT_STATE_SHUTDOWN;
	output[0] = 0;
	return(0);
}

int32_t agent_req_status(char*, char* output, void *cdata)
{
	int iretn;
	jsonhandle h;
	jstring jstring={0,0,0};

	json_startout(&jstring);
	for (h.hash=0; h.hash<JSON_MAX_HASH; ++h.hash)
	{
		for (h.index=0; h.index<(uint32_t)json_count_hash(h.hash,((cosmosstruc *)cdata)); ++h.index)
		{
			json_out_handle(&jstring, h,((cosmosstruc *)cdata));
		}
	}
	iretn = json_stopout(&jstring);
	if (!iretn)
	{
		if (jstring.index < AGENTMAXBUFFER+4)
		{
			strncpy(output,jstring.string,jstring.index);
			output[jstring.index-1] = 0;
		}
		else
		{
			strncpy(output,jstring.string,AGENTMAXBUFFER+3);
			output[AGENTMAXBUFFER+3] = 0;
		}
	}
	else
	{
		strcpy(output,"error");
		output[5] = 0;
	}
	return(iretn);
}

int32_t agent_req_getvalue(char *request, char* output, void *cdata)
{
	jstring jstring={0,0,0};

	if (json_of_list(&jstring,request,((cosmosstruc *)cdata)) != NULL)
	{
		strncpy(output,jstring.string,((cosmosstruc *)cdata)->agent[0].beat.bsz);
		output[((cosmosstruc *)cdata)->agent[0].beat.bsz-1] = 0;
		free(jstring.string);
		return (0);
	}
	else
		return (JSON_ERROR_EOS);
}

int32_t agent_req_setvalue(char *request, char* output, void *cdata)
{
	int iretn;
	iretn = json_parse(request,((cosmosstruc *)cdata));

	sprintf(output,"%d",iretn);

	return(iretn);
}

//! Open COSMOS output channel
/*! Establish a multicast socket for publishing COSMOS messages using the specified address and
 * port.
	\param type 0=Multicast, 1=Broadcast UDP, 2=Broadcast CSP.
	\param port Port number to publish on.
	\return 0, otherwise negative error.
*/
int agent_publish(cosmosstruc *cdata, uint16_t type, uint16_t port)
{
#ifdef COSMOS_WIN_OS
	struct sockaddr_storage ss;
	int sslen;
	INTERFACE_INFO ilist[20];
	unsigned long nbytes;
	uint32_t nif;
	uint32_t ip, net, bcast;
#else
	struct ifconf confa;
	struct ifreq *ifra;
	char data[512];
#endif // COSMOS_WIN_OS
	int iretn;
	int on = 1;
	//uint32_t ip, net, bcast;

	// Return immediately if we've already done this
	if (cdata->agent[0].pub[0].cport)
		return (0);

	switch (type)
	{
	case AGENT_TYPE_MULTICAST:
	case AGENT_TYPE_UDP:
		for (uint32_t i=0; i<AGENTMAXIF; i++)
			cdata->agent[0].pub[i].cudp = -1;

		if ((cdata->agent[0].pub[0].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
		{
			return (AGENT_ERROR_SOCKET);
		}

		// Use above socket to find available interfaces and establish
		// publication on each.
		cdata->agent[0].ifcnt = 0;
#ifdef COSMOS_WIN_OS
		if (WSAIoctl(cdata->agent[0].pub[0].cudp, SIO_GET_INTERFACE_LIST, 0, 0, &ilist,sizeof(ilist), &nbytes, 0, 0) == SOCKET_ERROR)
		{
			CLOSE_SOCKET(cdata->agent[0].pub[0].cudp);
			return (AGENT_ERROR_DISCOVERY);
		}

		nif = nbytes / sizeof(INTERFACE_INFO);
		for (uint32_t i=0; i<nif; i++)
		{
			strcpy(cdata->agent[0].pub[cdata->agent[0].ifcnt].address,inet_ntoa(((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr));
			if (!strcmp(cdata->agent[0].pub[cdata->agent[0].ifcnt].address,"127.0.0.1"))
			{
				if (cdata->agent[0].pub[cdata->agent[0].ifcnt].cudp >= 0)
				{
					CLOSE_SOCKET(cdata->agent[0].pub[cdata->agent[0].ifcnt].cudp);
					cdata->agent[0].pub[cdata->agent[0].ifcnt].cudp = -1;
				}
				continue;
			}
			// No need to open first socket again
			if (cdata->agent[0].pub[cdata->agent[0].ifcnt].cudp < 0)
			{
				if ((cdata->agent[0].pub[cdata->agent[0].ifcnt].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
				{
					continue;
				}
			}

			memset(&cdata->agent[0].pub[cdata->agent[0].ifcnt].caddr,0,sizeof(struct sockaddr_in));
			cdata->agent[0].pub[i].caddr.sin_family = AF_INET;
			if (type == AGENT_TYPE_MULTICAST)
			{
				sslen = sizeof(ss);
				WSAStringToAddressA((char *)AGENTMCAST,AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
				cdata->agent[0].pub[cdata->agent[0].ifcnt].caddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
			}
			else
			{
				if ((iretn = setsockopt(cdata->agent[0].pub[cdata->agent[0].ifcnt].cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
				{
					CLOSE_SOCKET(cdata->agent[0].pub[cdata->agent[0].ifcnt].cudp);
					continue;
				}
				ip = ((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr.S_un.S_addr;
				net = ((struct sockaddr_in*)&(ilist[i].iiNetmask))->sin_addr.S_un.S_addr;
				bcast = ip | (~net);

				cdata->agent[0].pub[cdata->agent[0].ifcnt].caddr.sin_addr = ((struct sockaddr_in *)&ilist[i].iiAddress)->sin_addr;
				cdata->agent[0].pub[cdata->agent[0].ifcnt].caddr.sin_addr.S_un.S_addr = bcast;
			}
			cdata->agent[0].pub[cdata->agent[0].ifcnt].caddr.sin_port = htons(port);
			cdata->agent[0].pub[cdata->agent[0].ifcnt].type = type;
			cdata->agent[0].pub[cdata->agent[0].ifcnt].cport = port;
			cdata->agent[0].ifcnt++;
		}
#else
		confa.ifc_len = sizeof(data);
		confa.ifc_buf = (caddr_t)data;
		if (ioctl(cdata->agent[0].pub[0].cudp,SIOCGIFCONF,&confa) < 0)
		{
			CLOSE_SOCKET(cdata->agent[0].pub[0].cudp);
			return (AGENT_ERROR_DISCOVERY);
		}
		// Use result to discover interfaces.
		ifra = confa.ifc_req;
		for (int32_t n=confa.ifc_len/sizeof(struct ifreq); --n >= 0; ifra++)
		{
			if (ifra->ifr_addr.sa_family != AF_INET) continue;
			inet_ntop(ifra->ifr_addr.sa_family,&((struct sockaddr_in*)&ifra->ifr_addr)->sin_addr,cdata->agent[0].pub[cdata->agent[0].ifcnt].address,sizeof(cdata->agent[0].pub[cdata->agent[0].ifcnt].address));

			if (ioctl(cdata->agent[0].pub[0].cudp,SIOCGIFFLAGS, (char *)ifra) < 0) continue;

			if ((ifra->ifr_flags & IFF_UP) == 0 || (ifra->ifr_flags & IFF_LOOPBACK) || (ifra->ifr_flags & (IFF_BROADCAST)) == 0) continue;

			// No need to open first socket again
			if (cdata->agent[0].pub[cdata->agent[0].ifcnt].cudp < 0)
			{
				if ((cdata->agent[0].pub[cdata->agent[0].ifcnt].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
				{
					continue;
				}
			}

			if (type == AGENT_TYPE_MULTICAST)
			{
				inet_pton(AF_INET,AGENTMCAST,&cdata->agent[0].pub[cdata->agent[0].ifcnt].caddr.sin_addr);
			}
			else
			{
				if ((iretn = setsockopt(cdata->agent[0].pub[cdata->agent[0].ifcnt].cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
				{
					CLOSE_SOCKET(cdata->agent[0].pub[cdata->agent[0].ifcnt].cudp);
					continue;
				}

				if (ioctl(cdata->agent[0].pub[0].cudp,SIOCGIFBRDADDR,(char *)ifra) < 0) continue;
				memcpy((char *)&cdata->agent[0].pub[cdata->agent[0].ifcnt].caddr, (char *)&ifra->ifr_broadaddr, sizeof(ifra->ifr_broadaddr));
			}
			cdata->agent[0].pub[cdata->agent[0].ifcnt].caddr.sin_port = htons(port);
			cdata->agent[0].pub[cdata->agent[0].ifcnt].type = type;
			cdata->agent[0].pub[cdata->agent[0].ifcnt].cport = port;
			cdata->agent[0].ifcnt++;
		}
#endif // COSMOS_WIN_OS
		break;
	case AGENT_TYPE_CSP:
		break;
	}

	return (0);
}

//! Discover interfaces
/*! Return a vector of ::agent_channel containing info on each valid interface. For IPV4 this
 *	will include the address and broadcast address, in both string sockaddr_in format.
	\param ntype Type of network (Multicast, Broadcast UDP, CSP)
	\return Vector of interfaces
	*/
vector<agent_channel> agent_find_addresses(uint16_t ntype)
{
	vector<agent_channel> iface;
	agent_channel tiface;

#ifdef COSMOS_WIN_OS
	struct sockaddr_storage ss;
	int sslen;
	INTERFACE_INFO ilist[20];
	unsigned long nbytes;
	uint32_t nif, ssize;
	uint32_t ip, net, bcast;
#else
	struct ifconf confa;
	struct ifreq *ifra;
	char data[512];
#endif // COSMOS_WIN_OS
	int iretn;
	int on = 1;
	int32_t cudp;

	switch (ntype)
	{
	case AGENT_TYPE_MULTICAST:
	case AGENT_TYPE_UDP:
		if ((cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
		{
			return (iface);
		}

		// Use above socket to find available interfaces and establish
		// publication on each.
#ifdef COSMOS_WIN_OS
		if (WSAIoctl(cudp, SIO_GET_INTERFACE_LIST, 0, 0, &ilist,sizeof(ilist), &nbytes, 0, 0) == SOCKET_ERROR)
		{
			CLOSE_SOCKET(cudp);
			return (iface);
		}

		nif = nbytes / sizeof(INTERFACE_INFO);
		PIP_ADAPTER_ADDRESSES pAddresses = NULL;
		PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
		pAddresses = (IP_ADAPTER_ADDRESSES *) calloc(sizeof(IP_ADAPTER_ADDRESSES), 2*nif);
		ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES) * 2 * nif;
		DWORD dwRetVal;
		if ((dwRetVal=GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen)) == ERROR_BUFFER_OVERFLOW)
		{
			free(pAddresses);
			return (iface);
		}

		for (uint32_t i=0; i<nif; i++)
		{
			strcpy(tiface.address,inet_ntoa(((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr));
			if (!strcmp(tiface.address,"127.0.0.1"))
			{
				continue;
			}

			pCurrAddresses = pAddresses;
			while (pAddresses)
			{
				if (((struct sockaddr_in *)(pCurrAddresses->FirstUnicastAddress->Address.lpSockaddr))->sin_addr.s_addr == ((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr.s_addr)
				{
					strcpy(tiface.name, pCurrAddresses->AdapterName);
					break;
				}
				pCurrAddresses = pCurrAddresses->Next;
			}
			memset(&tiface.caddr,0,sizeof(struct sockaddr_in));
			memset(&tiface.baddr,0,sizeof(struct sockaddr_in));
			tiface.caddr.sin_family = AF_INET;
			tiface.baddr.sin_family = AF_INET;
			if (ntype == AGENT_TYPE_MULTICAST)
			{
				sslen = sizeof(ss);
				WSAStringToAddressA((char *)AGENTMCAST,AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
				tiface.caddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
				tiface.baddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
			}
			else
			{
				if ((iretn = setsockopt(cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
				{
					continue;
				}
				ip = ((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr.S_un.S_addr;
				net = ((struct sockaddr_in*)&(ilist[i].iiNetmask))->sin_addr.S_un.S_addr;
				bcast = ip | (~net);

				tiface.caddr.sin_addr = ((struct sockaddr_in *)&ilist[i].iiAddress)->sin_addr;
				tiface.caddr.sin_addr.S_un.S_addr = ip;
				tiface.baddr.sin_addr = ((struct sockaddr_in *)&ilist[i].iiAddress)->sin_addr;
				tiface.baddr.sin_addr.S_un.S_addr = bcast;
			}
			((struct sockaddr_in *)&ss)->sin_addr = tiface.caddr.sin_addr;
			ssize = strlen(tiface.address);
			WSAAddressToStringA((struct sockaddr *)&tiface.caddr.sin_addr, sizeof(struct sockaddr_in), 0, tiface.address, (LPDWORD)&ssize);
			ssize = strlen(tiface.baddress);
			WSAAddressToStringA((struct sockaddr *)&tiface.baddr.sin_addr, sizeof(struct sockaddr_in), 0, tiface.baddress, (LPDWORD)&ssize);
			tiface.type = ntype;
			iface.push_back(tiface);
		}
#else
		confa.ifc_len = sizeof(data);
		confa.ifc_buf = (caddr_t)data;
		if (ioctl(cudp,SIOCGIFCONF,&confa) < 0)
		{
			CLOSE_SOCKET(cudp);
			return (iface);
		}
		// Use result to discover interfaces.
		ifra = confa.ifc_req;
		for (int32_t n=confa.ifc_len/sizeof(struct ifreq); --n >= 0; ifra++)
		{
			if (ifra->ifr_addr.sa_family != AF_INET) continue;
			inet_ntop(ifra->ifr_addr.sa_family,&((struct sockaddr_in*)&ifra->ifr_addr)->sin_addr,tiface.address,sizeof(tiface.address));

			if (ioctl(cudp,SIOCGIFFLAGS, (char *)ifra) < 0) continue;

			if ((ifra->ifr_flags & IFF_UP) == 0 || (ifra->ifr_flags & IFF_LOOPBACK) || (ifra->ifr_flags & (IFF_BROADCAST)) == 0) continue;

			if (ntype == AGENT_TYPE_MULTICAST)
			{
				inet_pton(AF_INET,AGENTMCAST,&tiface.caddr.sin_addr);\
				strcpy(tiface.baddress, AGENTMCAST);
				inet_pton(AF_INET,AGENTMCAST,&tiface.baddr.sin_addr);\
			}
			else
			{
				if ((iretn = setsockopt(cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
				{
					continue;
				}

				strncpy(tiface.name, ifra->ifr_name, COSMOS_MAX_NAME);
				if (ioctl(cudp,SIOCGIFBRDADDR,(char *)ifra) < 0) continue;
				memcpy((char *)&tiface.baddr, (char *)&ifra->ifr_broadaddr, sizeof(ifra->ifr_broadaddr));
				if (ioctl(cudp,SIOCGIFADDR,(char *)ifra) < 0) continue;
				memcpy((char *)&tiface.caddr, (char *)&ifra->ifr_addr, sizeof(ifra->ifr_addr));
				inet_ntop(tiface.baddr.sin_family,&tiface.baddr.sin_addr,tiface.baddress,sizeof(tiface.baddress));
			}
			tiface.type = ntype;
			iface.push_back(tiface);
		}

#endif // COSMOS_WIN_OS

		break;
	}

	return (iface);
}

//! Post a JSON message
/*! Post a JSON string on the previously opened publication channel.
	\param type A byte indicating the type of message.
	\param message A NULL terminated JSON text string to post.
	\return 0, otherwise negative error.
*/
int agent_post(cosmosstruc *cdata, uint8_t type, char *message)
{
	int nbytes, mbytes, i, iretn=0;
	char post[AGENTMAXBUFFER];
	//jstring jstring={0,0,0};

	if (message == NULL)
		mbytes = 0;
	else
		mbytes = strlen(message);

	cdata->agent[0].beat.utc = cdata->agent[0].beat.utc;
	post[0] = type;
	for (i=0; i<cdata->agent[0].ifcnt; i++)
	{
		sprintf(&post[1],"{\"agent_utc\":%.15g}{\"agent_node\":\"%s\"}{\"agent_proc\":\"%s\"}{\"agent_addr\":\"%s\"}{\"agent_port\":%u}{\"agent_bsz\":%u}",cdata->agent[0].beat.utc,cdata->agent[0].beat.node,cdata->agent[0].beat.proc,cdata->agent[0].pub[i].address,cdata->agent[0].beat.port,cdata->agent[0].beat.bsz);
		if (mbytes)
		{
			nbytes = strlen(post);
			if (nbytes+mbytes > AGENTMAXBUFFER)
				return (AGENT_ERROR_BUFLEN);
			strcat(post,message);
		}
		else
		{
			nbytes = strlen(post)+1;
		}
		iretn = sendto(cdata->agent[0].pub[i].cudp,(const char *)post,nbytes+mbytes,0,(struct sockaddr *)&cdata->agent[0].pub[i].caddr,sizeof(struct sockaddr_in));
	}
	if (iretn<0)
	{
#ifdef COSMOS_WIN_OS
		return(-WSAGetLastError());
#else
		return (-errno);
#endif
	}
	return (0);
}

//! Close COSMOS output channel
/*! Close a previously opened publication channel and recover any allocated resources.
	\param channel A previously opened publication channel.
	\return 0, otherwise negative error.
	*/
int agent_unpublish(cosmosstruc *cdata)
{
	int i;

	for (i=0; i<cdata->agent[0].ifcnt; i++)
		CLOSE_SOCKET(cdata->agent[0].pub[i].cudp);
	return (0);
}

//! Open COSMOS channel for polling
/*! This establishes a multicast channel for subscribing to COSMOS messages.
	\param cdata Pointer to ::cosmosstruc to use.
	\param type 0=Multicast, 1=Broadcast UDP, 2=Broadcast CSP.
	\param address The IP Multicast address of the channel.
	\param port The port to use for the channel.
	\param usectimeo Blocking read timeout in micro seconds.
	\return 0, otherwise negative error.
*/
int agent_subscribe(cosmosstruc *cdata, uint16_t type, char *address, uint16_t port, uint32_t usectimeo)
{
	int iretn = 0;

	if (cdata->agent[0].sub.cport)
		return (0);

	if ((iretn=agent_open_socket(&cdata->agent[0].sub,type,address,port,AGENT_LISTEN,AGENT_BLOCKING, usectimeo)) < 0)
	{
		return (iretn);
	}

	return (0);
}

int agent_subscribe(cosmosstruc *cdata, uint16_t type, char *address, uint16_t port)
{
	int iretn = 0;

	if ((iretn=agent_subscribe(cdata, type, address, port, 100)) < 0)
	{
		return (iretn);
	}

	return (0);
}
//! Close COSMOS subscription channel
/*! Close channel previously opened for polling for messages and recover resources.
	\return 0, otherwise negative error.
*/
int agent_unsubscribe(cosmosstruc *cdata)
{
	CLOSE_SOCKET(cdata->agent[0].sub.cudp);
	return (0);
}

//! Listen for message
/*! Poll the subscription channel for the requested amount of time. Return as soon as a single message
 * comes in, or the timer runs out.
	\param message Buffer for storing incoming message.
	\param bsize Size of message buffer, including terminating NULL.
	\param waitsec Number of seconds in timer.
	\return If a message comes in, return its type. If none comes in, return zero, otherwise negative error.
*/
int agent_poll(cosmosstruc *cdata, char *message, uint16_t bsize, uint8_t type, float waitsec)
{
	struct timeval tv, ltv;
	//	struct sockaddr_in raddr;
	//	int addrlen;
	int nbytes;
	char input[AGENTMAXBUFFER];

	if (!cdata->agent[0].sub.cport)
		return (AGENT_ERROR_CHANNEL);

	gettimeofday(&ltv,NULL);

	//	addrlen = sizeof(raddr);
	ltv.tv_usec += 1000000 *(waitsec - (int)waitsec);
	ltv.tv_sec += (int)waitsec + (int)(ltv.tv_usec/1000000);
	ltv.tv_usec = ltv.tv_usec % 1000000;

	do
	{
		nbytes = 0;
		switch (cdata->agent[0].sub.type)
		{
		case AGENT_TYPE_MULTICAST:
		case AGENT_TYPE_UDP:
			nbytes = recvfrom(cdata->agent[0].sub.cudp,input,AGENTMAXBUFFER,0,(struct sockaddr *)NULL,(socklen_t *)NULL);
			break;
		case AGENT_TYPE_CSP:
			break;
		}

		if (nbytes > 0)
		{
			if (type == AGENT_MESSAGE_ALL || type == input[0])
			{
				if (nbytes < bsize+2)
				{
					strncpy(message,&input[1],nbytes-1);
					message[nbytes-1] = 0;
					return ((int)input[0]);
				}
				else
				{
					return (AGENT_ERROR_MEMORY);
				}
			}
		}
		gettimeofday(&tv,NULL);
		if (tv.tv_sec > ltv.tv_sec || (tv.tv_sec == ltv.tv_sec && tv.tv_usec > ltv.tv_usec))
			nbytes = 0;
	} while (nbytes != 0);

	return (0);
}

//! Listen for heartbeat
/*! Poll the subscription channel until you receive a heartbeat message, or the timer runs out.
	\param waitsec Number of seconds to wait before timing out.
	\return ::beatstruc with acquired heartbeat. The UTC will be set to 0 if no heartbeat was
	acquired.
*/
beatstruc agent_poll_beat(cosmosstruc *cdata, float waitsec)
{
	int iretn;
	beatstruc beat;
	char input[AGENTMAXBUFFER];

	iretn = agent_poll(cdata, input,AGENTMAXBUFFER,AGENT_MESSAGE_BEAT,waitsec);

	beat.utc = 0.;
	if (iretn == AGENT_MESSAGE_BEAT)
	{
		iretn = json_parse(input,&cdata[1]);
		beat = cdata[1].agent[0].beat;
	}

	return (beat);
}

//! Listen for Time
/*! Poll the subscription channel until you receive a time message, or the timer runs out.
	\param waitsec Number of seconds to wait before timing out.
	\return ::timestruc with acquired time. The UTC will be set to 0 if no heartbeat was
	acquired.
*/
timestruc agent_poll_time(cosmosstruc *cdata, float waitsec)
{
	int iretn;
	timestruc time;
	char input[60];

	iretn = agent_poll(cdata, input,60,AGENT_MESSAGE_TIME,waitsec);

	if (iretn == AGENT_MESSAGE_TIME)
	{
		iretn = json_parse(input,&cdata[1]);
		if (iretn >= 0)
		{
			time.mjd = cdata[1].node.loc.utc;
		}
	}

	return (time);
}

//! Listen for Location
/*! Poll the subscription channel until you receive a location message, or the timer runs out.
	\param waitsec Number of seconds to wait before timing out.
	\return ::locstruc with acquired location. The UTC will be set to 0 if no heartbeat was
	acquired.
*/
locstruc agent_poll_location(cosmosstruc *cdata, float waitsec)
{
	int iretn;
	locstruc loc;
	char input[50];

	iretn = agent_poll(cdata, input,50,AGENT_MESSAGE_LOCATION,waitsec);

	if (iretn == AGENT_MESSAGE_LOCATION)
	{
		iretn = json_parse(input,&cdata[1]);
		if (iretn >= 0)
		{
			loc = cdata[1].node.loc;
		}
	}

	return (loc);
}

//! Listen for Beacon
/*! Poll the subscription channel until you receive a info message, or the timer runs out.
	\param waitsec Number of seconds to wait before timing out.
	\return ::infostruc with acquired info. The UTC will be set to 0 if no info was
	acquired.
*/
//summarystruc agent_poll_info(float waitsec)
nodestruc agent_poll_info(cosmosstruc *cdata, float waitsec)
{
	int iretn;
	//summarystruc info;
	nodestruc info;
	char input[1000];

	iretn = agent_poll(cdata, input,1000,AGENT_MESSAGE_TRACK,waitsec);

	if (iretn == AGENT_MESSAGE_TRACK)
	{
		iretn = json_parse(input,&cdata[1]);
		if (iretn >= 0)
		{
			strcpy(info.name,cdata[1].node.name);
			//			info.utc = cdata->node.loc.utc;
			info.loc = cdata[1].node.loc;
			info.powgen = cdata[1].node.powgen;
			info.powuse = cdata[1].node.powuse;
			info.battlev = cdata[1].node.battlev;
		}
	}
	else
		info.loc.utc = 0.;

	return (info);
}

//! Listen for IMU device
/*! Poll the subscription channel until you receive a IMU device message, or the timer runs out.
	\param waitsec Number of seconds to wait before timing out.
	\return ::beatstruc with acquired heartbeat. The UTC will be set to 0 if no heartbeat was
	acquired.
*/
imustruc agent_poll_imu(cosmosstruc *cdata, float waitsec)
{
	int iretn;
	imustruc imu;
	char input[50];

	iretn = agent_poll(cdata, input,50,AGENT_MESSAGE_IMU,waitsec);

	if (iretn == AGENT_MESSAGE_IMU)
	{
		iretn = json_parse(input,&cdata[1]);
		if (iretn >= 0)
		{
			imu = *cdata[1].node.imu[0];
		}
	}

	return (imu);
}

//! Open UDP socket
/*! Open a UDP socket and configure it for the specified use. Various
flags are set, and the socket is bound, if necessary. Support is
provided for the extra steps necessary for MS Windows.
	\param channel Pointer to ::agent_channel holding final configuration.
	\param address Destination address
	\param port Source port. If zero, automatically assigned.
	\param role Publish, subscribe, communicate.
	\param blocking True or false.
	\param usectimeo Blocking read timeout in micro seconds.
	\return Zero, or negative error.
*/
int agent_open_socket(agent_channel *channel, uint16_t ntype, const char *address, uint16_t port, uint16_t role, bool blocking, uint32_t usectimeo)
{
	socklen_t namelen;
	int iretn;
	struct ip_mreq mreq;
	int on = 1;

#ifdef COSMOS_WIN_OS
	unsigned long nonblocking = 1;
	struct sockaddr_storage ss;
	int sslen;
	WORD wVersionRequested;
	WSADATA wsaData;
	static bool started=false;

	if (!started)
	{
		wVersionRequested = MAKEWORD( 1, 1 );
		iretn = WSAStartup( wVersionRequested, &wsaData );
	}
#endif

	if ((channel->cudp = socket(AF_INET,SOCK_DGRAM,0)) <0)
	{
		return (-errno);
	}

	if (blocking == AGENT_NONBLOCKING)
	{
		iretn = 0;
#ifdef COSMOS_WIN_OS
		if (ioctlsocket(channel->cudp, FIONBIO, &nonblocking) != 0)
		{
			iretn = -WSAGetLastError();
		}
#else
		if (fcntl(channel->cudp, F_SETFL,O_NONBLOCK) < 0)
		{
			iretn = -errno;
		}
#endif
		if (iretn < 0)
		{
			CLOSE_SOCKET(channel->cudp);
			channel->cudp = iretn;
			return (iretn);
		}
	}

	// this defines the wait time for a response from a request
	if (usectimeo)
	{
#ifdef COSMOS_WIN_OS
		int msectimeo = usectimeo/1000;
		iretn = setsockopt(channel->cudp,SOL_SOCKET,SO_RCVTIMEO,(const char *)&msectimeo,sizeof(msectimeo));
#else
		struct timeval tv;
		tv.tv_sec = usectimeo/1000000;
		tv.tv_usec = usectimeo%1000000;
		iretn = setsockopt(channel->cudp,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv,sizeof(tv));
#endif
	}

	memset(&channel->caddr,0,sizeof(struct sockaddr_in));
	channel->caddr.sin_family = AF_INET;
	channel->caddr.sin_port = htons(port);

	switch (role)
	{
	case AGENT_LISTEN:
#ifdef COSMOS_MAC_OS
		if (setsockopt(channel->cudp,SOL_SOCKET,SO_REUSEPORT,(char*)&on,sizeof(on)) < 0)
#else
		if (setsockopt(channel->cudp,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on)) < 0)
#endif
		{
			CLOSE_SOCKET(channel->cudp);
			channel->cudp = -errno;
			return (-errno);
		}

		channel->caddr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (::bind(channel->cudp,(struct sockaddr *)&channel->caddr, sizeof(struct sockaddr_in)) < 0)
		{
			CLOSE_SOCKET(channel->cudp);
			channel->cudp = -errno;
			return (-errno);
		}

		// If we bound to port 0, then find out what our assigned port is.
		if (!port)
		{
			namelen = sizeof(struct sockaddr_in);
			if ((iretn = getsockname(channel->cudp, (sockaddr*)&channel->caddr, &namelen)) == -1)
			{
				CLOSE_SOCKET(channel->cudp);
				channel->cudp = -errno;
				return (-errno);
			}
			channel->cport = ntohs(channel->caddr.sin_port);
		}
		else
		{
			channel->cport = port;
		}

		if (ntype == AGENT_TYPE_MULTICAST)
		{
			//! 2. Join multicast
			mreq.imr_multiaddr.s_addr = inet_addr(address);
			mreq.imr_interface.s_addr = htonl(INADDR_ANY);
			if (setsockopt(channel->cudp, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0)
			{
				CLOSE_SOCKET(channel->cudp);
				channel->cudp = -errno;
				return (-errno);
			}
		}


		break;
	case AGENT_JABBER:
		switch (ntype)
		{
		case AGENT_TYPE_UDP:
			if ((iretn=setsockopt(channel->cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
			{
				CLOSE_SOCKET(channel->cudp);
				channel->cudp = -errno;
				return (-errno);
			}
			channel->caddr.sin_addr.s_addr = 0xffffffff;
			break;
		case AGENT_TYPE_MULTICAST:
#ifndef COSMOS_WIN_OS
			inet_pton(AF_INET,AGENTMCAST,&channel->caddr.sin_addr);
#else
			sslen = sizeof(ss);
			WSAStringToAddressA((char *)AGENTMCAST,AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
			channel->caddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
#endif
			break;
		}
		channel->cport = port;
		break;
	case AGENT_TALK:
#ifndef COSMOS_WIN_OS
		inet_pton(AF_INET,address,&channel->caddr.sin_addr);
#else
		sslen = sizeof(ss);
		WSAStringToAddressA((char *)address,AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
		channel->caddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
#endif
		channel->cport = port;
		break;
	}

	strncpy(channel->address,address,17);
	channel->type = ntype;
	channel->addrlen = sizeof(struct sockaddr_in);

	return (0);
}

//! @}
