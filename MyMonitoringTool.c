#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <time.h>

#define MICROSEC_TO_SEC 1000000
#define NANOSEC_TO_SEC 1000000000
#define MICROSEC_TO_NANOSEC 1000
#define MICROSEC_TO_MILISEC 1000

#define KB_TO_GB 1000000

#define MAX_CHAR 256
#define ROW_NUMBER 12
#define NOTIN -1

#define GRAPH_CHAR '#'

void printStat(int samples, int tdelay) {
	printf("%s %i %s %i %s %.3f %s", "\n\n Number of samples:", samples, " --  every", tdelay, "microSecs (", 
		((double)(tdelay)/MICROSEC_TO_SEC), "secs )");
}

void wait_ms(int tdelay) {
	clock_t start_time = clock();
  	while ((clock() - start_time) * 1000 / CLOCKS_PER_SEC < tdelay/MICROSEC_TO_MILISEC);
}

void refresh(int tdelay) {
    wait_ms(tdelay);
    printf("\x1b[%d;%df", 1, 1); 
	
}


char** initialize_graph(int sample_size) {
	char** data_graph = (char**) malloc((ROW_NUMBER+1)*sizeof(char*));

	for (int i = 0; i < ROW_NUMBER+1; i++) {
		*(data_graph+i) = malloc((sample_size)*sizeof(char));
	}

	for (int i = 0; i < ROW_NUMBER; i++) {
		for (int k = 0; k < sample_size; k++) {
			*(*(data_graph+i)+k) = ' ';
		}
	}
	for (int k = 0; k < sample_size; k++) {
			*(*(data_graph+ROW_NUMBER)+k) = '_';
		}
	return data_graph;


}

char** update_graph(int current_sample_size, double max_num, double data, char** data_graph) {

	int data_display = round((data/max_num)*ROW_NUMBER);

	*(*(data_graph+(ROW_NUMBER-data_display))+current_sample_size-1) = GRAPH_CHAR;

	if (current_sample_size > 1 && *(*(data_graph+ROW_NUMBER)+current_sample_size-2) == GRAPH_CHAR) {
		*(*(data_graph+ROW_NUMBER)+current_sample_size-2) = '_';
	}

	return data_graph;
	
}

void free_graph(char** data_graph, int sample_size) {

	for (int i = 0; i < ROW_NUMBER+1; i++) {
		free(*(data_graph+i));
	}
	
	free(data_graph);
}


double getTotalMemory() {
	FILE* f = fopen("/proc/meminfo", "r");

	char word[MAX_CHAR];
	int total_memory;
	char memTotal[MAX_CHAR] = "MemTotal:";

	while (fscanf(f, "%s %i", word, &total_memory) != EOF) {
		if (strcmp(word, memTotal) == 0) {
			fclose(f);
			return (double)total_memory/KB_TO_GB;
		}
	}
	fclose(f);
	return NOTIN;
}

double getMemoryUsage() {
	FILE* memory_file = fopen("/proc/meminfo", "r");
	double total_memory = getTotalMemory()*KB_TO_GB;

	char word[MAX_CHAR];
	int free_memory;
	char memFree[MAX_CHAR] = "MemFree:";



	while (fscanf(memory_file, "%s %i", word, &free_memory) != EOF) {
		if (strcmp(word, memFree) == 0) {
			fclose(memory_file);
			return (double)(total_memory - free_memory)/KB_TO_GB;
		}
	}
	fclose(memory_file);
	return NOTIN;

}

void printMemoryLeftside(double max_mem, int i) {

	int max_mem_digits = ceil(log10(max_mem))+3;

	if (i == 0) {
		printf("%s %.2f %s", "\n", max_mem, "GB |");
	}
	else if (i == ROW_NUMBER) {
		printf("%s", "\n ");
		for (int k = 0; k < max_mem_digits-1; k++) {
			printf("%s", " ");
		}
		printf("%s", "0 GB |");
	}
	else {
		printf("%s", "\n");
		for (int k = 0; k < max_mem_digits+5; k++) {
			printf("%s", " ");
		}
		printf("%s", "|");
	}
	

}

void printMemoryData(char** mem_data_display, double max_mem, double current_mem_avg) {
	printf("%s %.2f %s", "\n\n Memory: ", current_mem_avg, "GB");

	for (int i = 0; i < ROW_NUMBER+1; i++) {
		printMemoryLeftside(max_mem, i);
		printf("%s", *(mem_data_display+i));
	}
}


void getTotalCpuUsageInfo(long long int* cpu_info) {

	long long int cpu_times[8];
	long long int cpu_idle_times[2];

	char word[MAX_CHAR];

	FILE* cpuinfo_file = fopen("/proc/stat", "r");

	fscanf(cpuinfo_file, "%s %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld", word, 
		&cpu_times[0], &cpu_times[1], &cpu_times[2], &cpu_idle_times[0], &cpu_idle_times[1], 
		&cpu_times[3], &cpu_times[4], &cpu_times[5], &cpu_times[6], &cpu_times[7]);

	for (int i = 0; i < 8; i++) {
		*(cpu_info) += cpu_times[i];
	}
	for (int i = 0; i < 2; i++) {
		*(cpu_info) += cpu_idle_times[i];
	}
	for (int i = 0; i < 2; i++) {
		*(cpu_info+1) += cpu_idle_times[i];
	}

	fclose(cpuinfo_file);

}


double getCpuUsagePercentage(long long int* currentTotalCpuUsageInfo, long long int* previousTotalCpuUsageInfo) {

	long long int currentTotalCpuTime = *(currentTotalCpuUsageInfo) - *(previousTotalCpuUsageInfo);
	long long int currentCpuInactiveTime = *(currentTotalCpuUsageInfo+1) - *(previousTotalCpuUsageInfo+1);

	return (1 - (double)currentCpuInactiveTime/currentTotalCpuTime)*100;
}

void printCpuData(char** cpu_data_display, double current_cpu_avg) {
	printf("%s %.2f %s", "\n\n CPU ", current_cpu_avg, "%");

	for (int i = 0; i < ROW_NUMBER+1; i++) {
		if (i == 0) {
			printf("%s", "\n 100% |");
		}
		else if (i == ROW_NUMBER){
			printf("%s", "\n   0% |");
		}
		else {
			printf("%s", "\n      |");
		}
		printf("%s", *(cpu_data_display+i));
	}
}

int getCoreAmount() {
	FILE* core_file = fopen("/proc/cpuinfo", "r");

	char word1[MAX_CHAR];
	char word2[MAX_CHAR];
	char ch;
	int value;

	char cpu[MAX_CHAR] = "cpu";
	char cores[MAX_CHAR] = "cores";


	while (fscanf(core_file, "%s %s %c %i", word1, word2, &ch, &value) != EOF) {
		if (strcmp(word1, cpu) == 0 && strcmp(word2, cores) == 0) {
			fclose(core_file);
			return value;
		}

	}

	fclose(core_file);
	return NOTIN;

}

double getGhz() {

	FILE* ghz_file = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");

	int freq;

	fscanf(ghz_file, "%i", &freq);

	fclose(ghz_file);

	return (double)freq/1000000;
	

}

void printCores(int cores, double ghz) {

	int row_num = ceil(sqrt(cores));

	printf("%s %i %s %.2f %s", "\n\n Number of Cores:", cores, " @ ", ghz, "GHZ");

	for (int i = 0; i < row_num; i++) {
		int cores_left = row_num;
		if (row_num > cores-(i*row_num)) {
			cores_left = cores-(i*row_num);
		}

		printf("%s", "\n ");
		for (int k = 0; k < cores_left; k++) {	
			printf("%s", "+--+ ");
		}
		printf("%s", "\n ");
		for (int k = 0; k < cores_left; k++) {
			printf("%s", "|  | ");	
		}
		printf("%s", "\n ");
		for (int k = 0; k < cores_left; k++) {
			printf("%s", "+--+ ");	

		}
	}

}



void cores_display(int sample_size, int tdelay) {
	int cores_amount = getCoreAmount();
	double ghz = getGhz();
	printCores(cores_amount, ghz);
}

void display_info(int sample_size, int tdelay, int show_mem, int show_cpu, int show_cores) {
	
	char** memory_data_graph = initialize_graph(sample_size);
	double max_memory = getTotalMemory();
	double memory_usage = 0;
	double memory_sum = 0;
	double current_memory_avg = 0;

	char** cpu_data_graph = initialize_graph(sample_size);
	double cpu_usage = 0;
	double cpu_sum = 0;
	double current_cpu_avg = 0;

	long long int* previousTotalCpuUsageInfo = (long long int*)malloc(2*sizeof(long long int));
	getTotalCpuUsageInfo(previousTotalCpuUsageInfo);
	long long int* currentTotalCpuUsageInfo = (long long int*)malloc(2*sizeof(long long int));
	getTotalCpuUsageInfo(currentTotalCpuUsageInfo);

	printf("\033[2J\033[H");


	for (int i = 1; i < sample_size+1; i++) {

		refresh(tdelay);

		printStat(sample_size, tdelay);

		if (show_mem == 1) {
			memory_usage = getMemoryUsage();
			memory_sum += memory_usage;
			current_memory_avg = memory_sum/i;
			update_graph(i, max_memory, memory_usage, memory_data_graph);
			printMemoryData(memory_data_graph, max_memory, current_memory_avg);

		}

		if (show_cpu == 1) {
			getTotalCpuUsageInfo(currentTotalCpuUsageInfo);
			cpu_usage = getCpuUsagePercentage(currentTotalCpuUsageInfo, previousTotalCpuUsageInfo);
			getTotalCpuUsageInfo(previousTotalCpuUsageInfo);
			cpu_sum += cpu_usage;
			current_cpu_avg = cpu_sum/i;
			update_graph(i, 100, cpu_usage, cpu_data_graph);
			printCpuData(cpu_data_graph, current_cpu_avg);
		}

		if (show_cores == 1) {
			int cores = getCoreAmount();
			double ghz = getGhz();
			printCores(cores, ghz);
		}
		
	}

	free(previousTotalCpuUsageInfo);
	free(currentTotalCpuUsageInfo);
	free_graph(cpu_data_graph, sample_size);
	free_graph(memory_data_graph, sample_size);

}

int getSampleSize(char* command) {
	char SAMPLE[MAX_CHAR] = "--samples=";
	char num_word[MAX_CHAR] = "";
	char word[MAX_CHAR] = "";

	for (int i = 0; i < strlen(command); i++ ) {
		if (strcmp(word, SAMPLE) == 0) {
			num_word[i-10] = command[i];
		}
		else if (i == 10) {
			return -1;
		}
		else {
			word[i] = command[i];
		}
	}

	return atoi(num_word);
}

int getTdelay(char* command) {
	char SAMPLE[MAX_CHAR] = "--tdelay=";
	char num_word[MAX_CHAR] = "";
	char word[MAX_CHAR] = "";

	for (int i = 0; i < strlen(command); i++ ) {
		if (strcmp(word, SAMPLE) == 0) {
			num_word[i-9] = command[i];
		}
		else if (i == 9) {
			return -1;
		}
		else {
			word[i] = command[i];
		}
	}

	return atoi(num_word);
}


int main(int argc, char **argv) {

	int invalid_syntax = 0;
	
	int show_mem = 0;
	int show_cpu = 0;
	int show_cores = 0;

	int sample_size = 20;
	int tdelay = 500000;
	

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--memory") == 0) {
			show_mem = 1;
		}
		else if (strcmp(argv[i], "--cpu") == 0) {
			show_cpu = 1;
		}
		else if (strcmp(argv[i], "--cores") == 0) {
			show_cores = 1;
		}
		else if (i == 1 && atoi(argv[i]) != 0) {
			sample_size = atoi(argv[i]);
		}
		else if (i == 2 && atoi(argv[i]) != 0) {
			tdelay = atoi(argv[i]);
		}
		else if (getSampleSize(argv[i]) != -1) {
			sample_size = getSampleSize(argv[i]);
		}
		else if (getTdelay(argv[i]) != -1) {
			tdelay = getTdelay(argv[i]);
		}
		else {
			invalid_syntax = 1;
		}

	}

	int default1 = 0;
	int default2 = 0;
	int default3 = 0;
	if (argc == 1) {
		default1 = 1;
	}
	if (argc == 2 && atoi(argv[1]) != 0) {
		default2 = 1;
	}
	if (argc == 3 && atoi(argv[1]) != 0 && atoi(argv[2]) != 0) {
		default3 = 1;
	}

	if (default1 || default2 || default3) {
		show_mem = 1;
		show_cpu = 1;
		show_cores = 1;
	}
	

	if (invalid_syntax == 0) {
		display_info(sample_size, tdelay, show_mem, show_cpu, show_cores);
	}
	else {
		printf("\nInvalid command\n");
	}

	printf("\x1b[%d;%df", 50, 1); 

	
	return 0;
	
}

//  gcc --std=c99 MyMonitoringTool.c -lm -o myMonitoringTool

 // gcc --std=c99 MyMonitoringTool.c -Wall -Werror -lrt -lm -o myMonitoringTool 

//   ./myMonitoringTool

//cd Documents/UTSC_y2_proj/CSCB09_A1



