/**
* This the main server program.
* For method definitions and usage, please see the server.h file in the include folder.
* Some of the functions implementations can be reviewed in the file: monitor_d_helper.c
*/
#include "server.h"
#include <getopt.h>


int main(int argc, char *argv[]){

	int run_daemon = 1;			// run daemon by default.
	char *port;					// port pointer, it's is assigned in the getopt() switch
	int c;				 		// getopt() returned char

	set_log_type("syslog");
	port = DEFAULT_PORT;		// Assing the default server port.
		

	while((c = getopt (argc, argv, ":hc:l:")) != -1){
		switch(c){
			case 'c':
				run_daemon = 0;
				set_log_type("c");
				port = optarg;
				break;

			case 'l':
				printf("%s\n", optarg);
				set_log_type(optarg);
				if(argv[3] != NULL){
					port = argv[3];
				}
				break;

			case 'h':
                show_help();
                break;
			case ':':
				switch (optopt)
				{
					case 'c':
						run_daemon = 0;
						set_log_type("c");
						break;
					case 'l':
						set_log_type("log.txt");
						break;
				
					default:
						break;
				}
	            break;
            case '?':
				fprintf(stderr, "invalid option: -%c\n\n", optopt);
				show_help();
	            break;
			default:
				show_help();
		}
	}


	if(checkPort(port) < 0){
		printf(" invalid port: %s \n", port);
		exit(1);
	}

		
	
	int listenfd = Open_listenfd(port);
	printf("Listening at port: %s\t", port);
	
	if(listenfd < 0){
		registryLog("Error al crear el server socket..");
		return -1;
	}

	struct sockaddr_in clientaddr;
	unsigned int clientlen = sizeof(clientaddr);

	if(run_daemon)
		daemonize(argv[0]);
	while(1){

		//registryLog("from while");
		int connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		registryConnetionStatus( clientaddr , 1);

		ConnectionBundle connection = {connfd, clientaddr};
		
		pthread_t tid;
		Pthread_create(&tid, NULL, attend, (void *)&connection);
	}

	Pthread_exit(NULL);
	exit(0);
	
}



void daemonize(const char *cmd){
    int i, fd0, fd1, fd2;
    pid_t pid;

    umask(0);
    

    if ((pid = fork()) < 0){
    	registryLog(" no se pudo forkear");
        return;
    }
    else if (pid != 0){
		printf("pid: %d (use it for kill the process)\n", pid);
		exit(0);
	}
        
    setsid();

    /*
    if (chdir("/") < 0){
        registryLog("can't change directory to /");
        exit(1);
    }*/
    
	
    for (i = 0; i < 3; i++)
        close(i);


    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

 
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
    	sprintf(MESSAGE_BUFFER, "Unexpected file descriptors %d %d %d", fd0, fd1, fd2);
        registryLog(MESSAGE_BUFFER);
        exit(1);
    }
}



void *attend(void *connection){

	ConnectionBundle conn = *((ConnectionBundle *) connection);

	Pthread_detach(pthread_self());

	signal(SIGPIPE, client_closed_handle);

	if(sendSystemInfo(conn.connfd) < 0 )
		return NULL;

	while(sendPerformanceInfo(conn.connfd) > 0 ){
		usleep(100000);
	}

	registryConnetionStatus( conn.clientaddr , 0);


	Close(conn.connfd);
	return NULL;
}




void registryConnetionStatus( struct sockaddr_in clientaddr , int type){

	struct hostent *hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
										sizeof(clientaddr.sin_addr.s_addr), 
										AF_INET);
	char *haddrp	   = inet_ntoa(clientaddr.sin_addr);

	time_t ltime = time(NULL);
	if(type == 1)
		sprintf(MESSAGE_BUFFER," Server connected to %s (%s) %s \n", hp->h_name, haddrp, asctime(localtime(&ltime)));
	else
		sprintf(MESSAGE_BUFFER," Server Disconnected to %s (%s) %s \n", hp->h_name, haddrp, asctime(localtime(&ltime)));

	registryLog(MESSAGE_BUFFER);

}




int registryLog(char *msg){


	FILE *settings = fopen("settings.x", "r");
	if(settings == NULL)
		return -1;

	char logtype[SHORT_STRING];
	fscanf(settings,"%s", logtype);
	fclose(settings);

	if(strcmp(logtype, "c") == 0){
		printf("%s\n", msg);
	}
	else if(strcmp(logtype, "syslog") == 0){

		setlogmask (LOG_UPTO (LOG_NOTICE));
		openlog ("monitord : ", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
		syslog (LOG_NOTICE, "%s",msg);
		closelog();
	}
	else{
		FILE *f = fopen(logtype, "a+");

		if(f == NULL)
			return -1;
		fprintf(f, "%s", msg);
		fclose(f);
	}
	return 1;

}



/*
void exit_handler(int sig){ 

    printf("Caught signal %d\n", sig);

    if (sig == 2){
        printf("Stoping Server....\n");
        exit(0);
    }else{
        printf("Unexpected signal received!\n");
    }
    //signal(sig, SIG_IGN);
    //run = 0;
} 
*/


int checkPort(char *port){

	for(int i = 0; i < strlen(port); i++){
		if (port[i] < '0' || port[i] > '9')
			return -1;
	}

	int p = atoi(port);
	if(p <= 1100)
		return -1;

	return 1;

}



void client_closed_handle(int signal){
	registryLog("ignore: Broken PIPE handled correctly.\n");
}


int set_log_type(char *logtype){

	FILE *f = fopen("settings.x", "w+");
	if(f == NULL)
		return -1;

	fprintf(f, "%s", logtype);
	fclose(f);
	return 1;
 }

