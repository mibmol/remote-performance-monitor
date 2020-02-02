#include "csapp.h"
#include "monitor.pb-c.h"
#include "constants.h"
#include "colors.h"
#include <termios.h>
#include <assert.h>

#define HELP_PATH "resources/clientHelp.txt"
#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT "8000"

void printSystemInfo(SystemInfo msg);
void printPerformanceInfo(PerformanceInfo msg);
void formatUptime( float uptime, int *time);
double KB_to_GB();
float KB_to_MB();

void exit_handler(int sig);
int get_input_char();
void *check_exit(void *clientfd);


/*
 * Show monitorc help
 */
void show_help();
