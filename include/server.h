#include <syslog.h>
#include <sys/resource.h>
#include <time.h>
#include <sys/mman.h>
#include "csapp.h"
#include "monitor.pb-c.h"
#include "constants.h"

#define HELP_PATH 		"resources/serverHelp.txt"
#define VERSION_PATH	"/proc/version"
#define STAT_PATH		"/proc/stat"
#define UPTIME_PATH		"/proc/uptime"
#define MEMINFO			"/proc/meminfo"
#define LOAD_AVG		"/proc/loadavg"
#define MAX_CONNECTION 10
#define DEFAULT_PORT "8000"



static char MESSAGE_BUFFER[MAX_STRING];


/*
	Struct for store system info which will be send through the
	network using the protobuff serializer for the first time.
*/
typedef struct{
	char system_name[SHORT_STRING];		// Distribution name.
	char version[MAX_STRING];			// linux version.
	int num_processors;					// Number of processor cores.
	long mem_total;						// (KB)

} SystemInfo_x;


/*
	Struct for store performance info which will be send through the
	network every second using the protobuff serializer.
*/
typedef struct {
	float uptime;						// PC running time.
	int n_processor_usage;
	double processor_usage[10];			// Percentage of use for each processor core.
	long mem_free;						// (KB)
	int num_process;					// Total processes installed in the system
	int num_process_running;			// Total of running processes
	float loadavg[3];					// Average load [ 1min, 5min, 15min ]
} PerformanceInfo_x;



/*
 *	Contain the client connection info.
 */
typedef struct{
	int connfd;
	struct sockaddr_in clientaddr;
} ConnectionBundle;



/*
	Functions for load system info.
	It use a SystemStateInfo struct reference which contains the 
	system info. To fill all the info in the struct it's necessary
	to call all the functions sequentially.
	loadData() performs said task.
*/
int loadSystemInfo_x(SystemInfo_x *data);

int loadUptime(PerformanceInfo_x *data);
int loadavg(PerformanceInfo_x *data);
int load_processor_usage(PerformanceInfo_x *data);
int load_mem_free(PerformanceInfo_x *data);
int load_processInfo(PerformanceInfo_x *data);


/* 
 * Performs the response to client.
 * *connection: pointer to a ConnectionBundle struct 
 * which contains the info of client connecion.
 * Call to echo() function.
 */
void *attend(void *connection);


/*
* it's used for check if the port "*port" is 
* a valid number.
*/
int checkPort(char *port);



/*
 * Send the system info data to client with
 * file descriptor "connfd"
 */
int sendSystemInfo(int connfd);


/*
 * Send the system performance data to client with
 * file descriptor "connfd"
 */
int sendPerformanceInfo(int connfd);


/**
* Make the program run as daemon
*/
void daemonize(const char *cmd);


/*
 * Show monitord help
 *
 */
void show_help();


/*
 * Save log to the log file.
 * it uses the the static variable "logtype" which
 * identifies the log mode.
 * *msg: pointer to the message string buffer.
 */
int registryLog(char *msg);


/*
* It registry a connection or disconnection from the client
* clientadd: sockaddr_in of the client.
* type: int 1: connection || 0: disconnection.
*/
void registryConnetionStatus( struct sockaddr_in clientaddr , int type);

void exit_handler(int sig);

void client_closed_handle(int signal);


/*
 * It is used to set the way in which system messages will be recorded.
 * "logtype" could be  "-l"  "-c"  or the default: "syslog".
 */
int set_log_type(char *logtype);

