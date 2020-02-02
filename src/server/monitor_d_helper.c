#include "server.h"


int sendSystemInfo(int connfd){

	SystemInfo msg = SYSTEM_INFO__INIT;

	SystemInfo_x data;
	loadSystemInfo_x(&data);

	msg.system_name = Malloc(SHORT_STRING);
	strcpy(msg.system_name, data.system_name);
	msg.version = Malloc(MAX_STRING);
	strcpy(msg.version, data.version);

	msg.num_processors = data.num_processors;
	msg.mem_total = data.mem_total;


	size_t size = system_info__get_packed_size(&msg);
	uint8_t *buff = Malloc(size);
	size_t n = system_info__pack(&msg, buff);
	if(n < 1)
		return -1;

	if(rio_writen(connfd, &size, 2) < 0){
		Free(msg.system_name);
		Free(msg.version);
		Free(buff);
		sprintf(MESSAGE_BUFFER,"Something went wrong while sending info to client. \"sendSystemInfo(int connfd)\"\n");
		registryLog(MESSAGE_BUFFER);
		return -1;
	}
	if(rio_writen(connfd, buff, size) < 0){
		Free(msg.system_name);
		Free(msg.version);
		Free(buff);
		sprintf(MESSAGE_BUFFER,"Something went wrong while sending info to client. \"sendSystemInfo(int connfd)\"\n");
		registryLog(MESSAGE_BUFFER);
		return -1;
	}

	Free(msg.system_name);
	Free(msg.version);
	Free(buff);

	return 1;
}



int sendPerformanceInfo(int connfd){
	PerformanceInfo msg = PERFORMANCE_INFO__INIT;
	PerformanceInfo_x data;

	loadUptime(&data);
	loadavg(&data);
	load_processor_usage(&data);
	load_mem_free(&data);
	load_processInfo(&data);

	msg.uptime = data.uptime;

	msg.n_processor_usage = data.n_processor_usage;
	msg.processor_usage = Malloc(msg.n_processor_usage * sizeof(int32_t));

	for(int i = 0; i < msg.n_processor_usage; i++){
		msg.processor_usage[i] = (int) (data.processor_usage[i] + 1);
	}

	msg.mem_free = data.mem_free;
	msg.num_process = data.num_process;
	msg.num_process_running = data.num_process_running;

	PerformanceInfo__LoadAvg la = PERFORMANCE_INFO__LOAD_AVG__INIT;
	la.min5 = data.loadavg[0];
	la.min10 = data.loadavg[1];
	la.min15 = data.loadavg[2];
	msg.loadavg = &la;


	size_t size = performance_info__get_packed_size(&msg);
	uint8_t *buff = Malloc(size);
	size_t n = performance_info__pack(&msg, buff);
	if(n < 1)
		return -1;

	if(rio_writen(connfd, &size, sizeof(short)) < 0){
		Free(msg.processor_usage);
		Free(buff);
		sprintf(MESSAGE_BUFFER,"ignore: something went wrong while sending info to client. \"sendPerformanceInfo(connfd)\"\n");
		registryLog(MESSAGE_BUFFER);
		return -1;
	}
	if(rio_writen(connfd, buff, size) < 0){
		Free(msg.processor_usage);
		Free(buff);
		sprintf(MESSAGE_BUFFER,"ignore: Something went wrong while sending info to client. \"sendPerformanceInfo(int connfd)\"\n");
		registryLog(MESSAGE_BUFFER);
		return -1;
	}


	Free(msg.processor_usage);
	Free(buff);

	return 1;
}


int loadSystemInfo_x(SystemInfo_x *data){

	//load system Version,
	FILE *f = fopen(VERSION_PATH, "r");
	
	char os_name[SHORT_STRING];
	char os_version[SHORT_STRING];
	if(fscanf(f, "Linux version %s %*s (gcc version %*s (%s %s))", data-> version, os_name, os_version) < 1)
		return -1;
	os_version[strlen(os_version)-2] = ' ';
	os_version[strlen(os_version)-1] = ' ';
	
	data-> system_name[0] = ' ';
	strcat(data-> system_name, os_name);
	strcat(data-> system_name, " ");
	strcat(data-> system_name, os_version);
	fclose(f);


	//load number of processors.
	int count_cores = 0; // | 0: all cores | 1: cpu0 | 2: cpu1 | 3: cpu2 | ... ...
	char ltemp[MAX_STRING];
	char stemp[SHORT_STRING];
	
    f = fopen(STAT_PATH, "r");

	while(fscanf(f, "%s ", stemp) > 0){
		if(strstr(stemp,"cpu") != NULL){
			count_cores++;
			fscanf(f, "%[^\n]", ltemp);
		}
		else break;
	}
	fclose(f);
	data->num_processors = count_cores - 1;


	// load total Memory.
	f = fopen(MEMINFO, "r");
	if(fscanf(f, "%s ", stemp) > 0){
		if(strstr(stemp,"MemTotal") != NULL){
			fscanf(f, "%ld %*s\n", &(data-> mem_total));
		}
		else return -1;
	}
	else return -1;
	fclose(f);

	return 0;
}


int loadUptime(PerformanceInfo_x *data){

	FILE *f = fopen(UPTIME_PATH, "r");

	char s[20];
	if(fscanf(f, "%s", s) < 1)
		return -1;
	data-> uptime = atof(s);
	fclose(f);
	return 0;
}



int loadavg(PerformanceInfo_x *data){

	float *temp;

	FILE *f = fopen(LOAD_AVG, "r");

	char min1[SHORT_STRING];
	char min5[SHORT_STRING];
	char min15[SHORT_STRING];

	if(fscanf(f, "%s %s %s", min1, min5, min15) < 1)
		return -1;

	temp = data-> loadavg;

	temp[0] = atof(min1);
	temp[1] = atof(min5);
	temp[2] = atof(min15);


	fclose(f);
	return 0;

}



int load_processor_usage(PerformanceInfo_x *data){

	char ltemp[MAX_STRING];
	char stemp[SHORT_STRING];


	FILE *f = fopen(STAT_PATH, "r");
	int count_cores = 0;
	while(fscanf(f, "%s ", stemp) > 0){
		if(strstr(stemp,"cpu") != NULL){
			count_cores++;
			fscanf(f, "%[^\n]", ltemp);
		}
		else break;
	}
	fclose(f);

	data-> n_processor_usage = count_cores - 1;
	
	int user;
	int nice;
	int system;
	int idle;
	int iowait;
	int irq;
	int softirq;
	int steal;
	int guest;
	int guest_nice;

	int Total_CPU_time_since_boot[count_cores][2];
	int Total_CPU_Idle_time_since_boot[count_cores][2];
	int Total_CPU_usage_time_since_boot;
    double Total_CPU_percentage;

	int coreID;
	int time = 900000;

	for (int i =0; i < 2; i++){
		coreID = 0;
		f = fopen(STAT_PATH, "r");
		while(fscanf(f, "%s ", stemp) > 0){
			if(strstr(stemp,"cpu") != NULL){
				
				fscanf(f, "%d %d %d %d %d %d %d %d %d %d\n", 
					&user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice );
				
				Total_CPU_time_since_boot[coreID][i] = user + nice + system + idle + iowait + irq + softirq + steal;
				Total_CPU_Idle_time_since_boot[coreID][i] = idle + iowait;

				
				/*
				Total_CPU_time_since_boot = user + nice + system + idle + iowait + irq + softirq + steal;
				double xx= (double)idle / (double) Total_CPU_time_since_boot;
				Total_CPU_percentage = (1.000 - xx) * 100.00;
				*/

				coreID++;
			}
			else break;
		}
		usleep(time);
		time = 0;
		fclose(f);
	}


	for (coreID = 0; coreID < count_cores; coreID ++){

		long TB = Total_CPU_time_since_boot[coreID][1] - Total_CPU_time_since_boot[coreID][0] ;
		long TI = Total_CPU_Idle_time_since_boot[coreID][1] - Total_CPU_Idle_time_since_boot[coreID][0];
		Total_CPU_usage_time_since_boot = TB - TI;
		Total_CPU_percentage = ((double)Total_CPU_usage_time_since_boot / (double)TB) * 100.00;
			
		*((data-> processor_usage) + coreID) = Total_CPU_percentage;
	}
	
	return 0;
}



int load_mem_free(PerformanceInfo_x *data){

	char temp[SHORT_STRING];

	FILE *f = fopen(MEMINFO, "r");

	while(fscanf(f, "%s ", temp) > 0){
		
		if(strstr(temp,"MemFree") != NULL){
			fscanf(f, "%ld %*s\n", &(data-> mem_free));	
			break;
		}
	}
	fclose(f);
	
	return 0;
}



int load_processInfo(PerformanceInfo_x *data){

	char ltemp[MAX_STRING];
	char temp[SHORT_STRING];

	FILE *f = fopen(STAT_PATH, "r");

	while(fscanf(f, "%s ", temp) > 0){
		
		if(strstr(temp,"intr") != NULL){
			while(strstr(temp,"\n") == NULL){
				fscanf(f, "%c", temp);
			}
		}

		else if(strstr(temp,"processes") != NULL){
			fscanf(f, "%d\n", &(data -> num_process));
		}

		else if(strstr(temp,"procs_running") != NULL){
			fscanf(f, "%d\n", &(data -> num_process_running));
		}

		else{
			fscanf(f, "%[^\n]", ltemp);
		}
	}

	fclose(f);
	return 0;
}



void show_help(){

	FILE *f = fopen(HELP_PATH,"r");

	if(!f){
		sprintf(MESSAGE_BUFFER, "runtime: Posible error durante lectura de archivo: resources/helpServer.txt\n");
		registryLog(MESSAGE_BUFFER);
		exit(0);
	}
	char c;
	while((c = getc(f))!= EOF){
		printf("%c",c);
	}

	fclose(f);
	exit(1);
}

