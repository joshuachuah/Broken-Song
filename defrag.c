#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>

void *func(void *);

// shared data stuff
struct data_t{
	char* filecontent;
	unsigned int filesize;
};

int length;
struct data_t **datas;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int main (int argc, char **argv){
	struct dirent *entry;

	if (argc != 3){
		puts("Usage: <executable> <input_directory> <output_file>");
		exit(1);
	}

	if (chdir(argv[1]) != 0){
		perror("chdir");
		exit(1);
	}

	// open current working directory 
	DIR *current = opendir(".");
	if (current == NULL) {
		perror("opendir");
		exit(1);
	}

	char **dirs_buff = NULL;
	int n_dirs = 0;

	// read entries and output based on type
	while ( (entry = readdir(current)) ){
		if (strcmp(entry->d_name, ".") != 0 || strcmp(entry->d_name, "..") != 0){
			dirs_buff = realloc(dirs_buff, sizeof(char*) * (n_dirs + 1));
			dirs_buff[n_dirs] = strdup(entry->d_name);
			n_dirs++;
		}
	}
	datas = NULL;
	datas = realloc(datas, sizeof(struct data_t*) * length);

	for (int i = 0; i < length; i++){
		datas[i] = NULL;
	}

	pthread_t tids[n_dirs];

	// start all of the threads
	for (unsigned long i = 0; i < n_dirs; i++){
		int err = pthread_create(&tids[i], NULL, func, dirs_buff[i]);
		if (err != 0) {
			fprintf(stderr, "! Couldn't create thread");
			abort();
		}
	}

	// wait for threads and join them 
	for (int i = 0; i < n_dirs; i++){
		pthread_join(tids[i], NULL);
	}

	chdir("..");

	FILE *outfile = fopen(argv[2], "wb");

	for (int i = 0; i < length; i++){
		fwrite(datas[i]->filecontent, datas[i]->filesize, 1, outfile);
	}
	fclose(outfile);

	for (int i = 0; i < length; i++){
		free(datas[i]->filecontent);
	}

	for (int i = 0; i < length; i++){
		free(datas[i]);
	}
	free(datas);

	closedir(current);

	return 0;
}


/**
 *
 * Thread Function
 *
 */
void *func(void *arg) {

	
	struct dirent *entry;

	char *dir_name = (char *)arg;

	DIR *current = opendir(dir_name);

	while ((entry = readdir(current))){
		if (entry->d_type == DT_DIR){
			// skip . and .. dirs
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
				continue;
			}
			
			// continue looping until it reaches a non-directory file
			char path[5000];
			snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);
			func(path);

		}else{
			// if not a dir, check if it is a .mp3 file
			if (strstr(entry->d_name, ".mp3") != NULL){


				// find the index for the .mp3 file
				char storage[2000];
				int tempi = 0;
				while (entry->d_name[tempi] >= '0' && entry->d_name[tempi] <= '9'){
					storage[tempi] = entry->d_name[tempi];
					tempi++;
				}
				storage[tempi] = '\0';
				
				int num = atoi(storage);

				char path [5000];
				snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);
				

				struct stat statBuffer;
				lstat(path, &statBuffer);

				// Allocate memory for mp3 file content
                char *filecontent = malloc((statBuffer.st_size + 1) * sizeof(char));

				FILE *infile = fopen(path, "rb");
				if (infile){
					fread(filecontent, statBuffer.st_size, 1, infile);
					fclose(infile);
				}


				// critical section
				pthread_mutex_lock(&lock);

				if (length < (num + 1)){
					length = num + 1;
				}

				datas = realloc(datas, sizeof(struct data_t*) * length);
                datas[num] = malloc(statBuffer.st_size * sizeof(char));
                datas[num]->filesize = statBuffer.st_size;
                datas[num]->filecontent = filecontent;
				// unlock the mutex 
				pthread_mutex_unlock(&lock);
			}
		}
	}
	closedir(current);
	return NULL;
}
