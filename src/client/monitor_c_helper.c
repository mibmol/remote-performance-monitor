#include "client.h"



void printSystemInfo(SystemInfo msg){
        printf(YELLOW"\t\tPress \'q\' or \'Q\' for exit. \n\n"NORMAL);

        // OS INFO
        printf(GREEN_BOLD"System Name: "WHITE"%s\n", msg.system_name);
        printf(GREEN_BOLD"Linux version: "WHITE"%s\n", msg.version);

        //num_processors
        printf(BLUE_BOLD"Cores:"MAGNETA"\t%d \n", msg.num_processors);
        // mem_total
        printf(BLUE_BOLD"Total Memory: "MAGNETA"%d KB | %.2f MB | %.2f GB\n" , 
                msg.mem_total, 
                KB_to_MB(msg.mem_total), 
                KB_to_GB(msg.mem_total));
}




void printPerformanceInfo(PerformanceInfo msg){

         // UPTIME
        int time[3];
        formatUptime(msg.uptime, time);
        printf(BLUE_BOLD"Uptime:"WHITE"\t%d H : %d m : %d s\n"NORMAL, time[0], time[1], time[2]);


        // LOAD AVERAGE
        printf(BLUE_BOLD"Promedio de Carga:\n");
        printf(CYAN"\t 1 min:"WHITE" %.2f\t "CYAN"5 min: "WHITE"%.2f\t "CYAN"15 min: "WHITE"%.2f\n"CYAN_BOLD, msg.loadavg -> min5, msg.loadavg -> min10, msg.loadavg -> min15 );
        
        printf("\n");

        // CORES
        printf(BLUE_BOLD"Processor Usage:\n");
        for(int i = 0; i< msg.n_processor_usage; i++){
                printf(CYAN_BOLD"\tcore %d:"CYAN" %d%%\n ", i + 1, msg.processor_usage[i]);
        }

        // MEMORY INFO
        printf(BLUE_BOLD"Free Memory: "CYAN_BOLD"%d KB | %.2f MB | %.2f GB\n" , 
                msg.mem_free,
                KB_to_MB(msg.mem_free),
                KB_to_GB(msg.mem_free));


        // PROCESSES INFO
        printf(BLUE_BOLD"Processes:\n");
        printf(MAGNETA"\tTotal: %d\n", msg.num_process);
        printf(CYAN"\tRunning: %d \n" , msg.num_process_running);
}



void formatUptime(float uptime, int *time){

        int h = (int)((uptime)/60)/60;
        int m = (int)(uptime)/60 - h*60;
        int s = (int)uptime - m*60 - h*3600;

        * time     = h;
        *(time +1) = m;
        *(time +2) = s;
}




double KB_to_GB(long KB){
    return (double)(KB/1024.00)/1024.00;
}




float KB_to_MB(long KB){
    return (float)(KB/1024.00);
}




int get_input_char() {
      int c=0;

      struct termios org_opts, new_opts;
      int res=0;
          //-----  store old settings -----------
      res = tcgetattr(STDIN_FILENO, &org_opts);
      assert(res==0);
          //---- set new terminal parms --------
      memcpy(&new_opts, &org_opts, sizeof(new_opts));
      new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
      tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
      c = getchar();
          //------  restore old settings ---------
      res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
      assert(res==0);
      return c;
}




void show_help(){
  
    FILE *f = fopen(HELP_PATH,"r");

    if(!f){
        fprintf(stderr, "Posible error durante lectura de archivo: %s\n", HELP_PATH);
    }
    
    char c;
    while((c = getc(f))!= EOF){
        printf("%c",c);
    }
    fclose(f);

    return;
}