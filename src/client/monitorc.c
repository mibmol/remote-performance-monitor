/**
* This the main client program.
* For method definitions and usage, please see the client.h file in the include folder.
*/


#include "client.h"

static int run = 1;
static int clientfd;


int main(int argc, char *argv[]){

    char *port;
    char *host;

    if (argc > 3) {
        show_help();
    }

    switch(argc){
        case 1:
            host = DEFAULT_HOST;
            port = DEFAULT_PORT;
            break;

        case 2:
            if(strcmp(argv[1], "-h") != 0){
                printf("Option \"%s\" is not recognized.\n", argv[1]);
                exit(0);
            }
            else
                show_help();
            break;

        case 3:
            host = argv[1];
            port = argv[2];
            break;

        default:
            show_help();
    }
   

    signal(SIGINT, exit_handler);
    clientfd = Open_clientfd(host, port);
    printf("connected\n");

    ssize_t n = 0;
    char pack_size[2];
    uint8_t system_info_buff[MAX_STRING];


    /**
    * the first read gets the packet size send by server,
    * and the second, the serialized bytes to unpack.
    */
    n = Rio_readn(clientfd, pack_size, sizeof(short));
    n = Rio_readn(clientfd, system_info_buff, *((short *)pack_size));


    /**
    * at this point the buffer "system_info_buff" is deserialized
    * by system_info__unpack()
    */
    SystemInfo *system_info_msg = system_info__unpack(NULL, n, system_info_buff);

    if(system_info_msg == NULL){
        fprintf(stderr, "error unpacking incoming message: SystemInfo\n");
    }

    pthread_t tid;
    Pthread_create(&tid, NULL, check_exit, (void *)&clientfd);
    while(run > 0){

        
        system("clear");
        clear();
        
        /**
        * The first read gets the packet size sent by server,
        * and the second, the serialized bytes to unpack.
        */
        uint8_t performance_buff[MAX_STRING];
        n = Rio_readn(clientfd, pack_size, sizeof(short));
        n = Rio_readn(clientfd, performance_buff, *((short *)pack_size));


        /**
        * at this point the buffer "performance_buff" is deserialized
        * by performance_info__unpack()
        */
        PerformanceInfo *performance_msg = performance_info__unpack(NULL, n, performance_buff);
        if(performance_msg == NULL){
            fprintf(stderr, "error unpacking incoming message: PerformanceInfo\n");
        }

        printSystemInfo(*system_info_msg);
        printPerformanceInfo(*performance_msg);

        performance_info__free_unpacked(performance_msg , NULL);

    }

    system_info__free_unpacked(system_info_msg , NULL);
    Close(clientfd);
    printf("\n\t"RED_BOLD"<---------------- closed --------------->\n");
    exit(0);

}



void *check_exit(void *clientfd){

    Pthread_detach(pthread_self());

    int run_check = 1;
    while(run_check){
        
        char input = get_input_char(stdin);
        if(input == 'q' || input == 'Q' ){
            run = -1;
            run_check = 0;
        }
    }
    return NULL;
}



void exit_handler(int sig){ 

    printf("\n Caught signal %d\n", sig);

    if (sig == SIGINT){
        run = -1;
        return;
    }else{
        printf("Unexpected signal received!\n");
    }
    signal(sig, SIG_IGN);
} 

