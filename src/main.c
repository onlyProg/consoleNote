#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define FILE_NAME ".note.txt"

/*
 * Getting the path to a file in the user's home directory.
 */
char* get_path_to_file(const char* file_name){
	char* path;

	path = malloc((strlen(getenv("HOME")) + strlen(file_name)) * sizeof(char));
	sprintf(path, "%s/%s", getenv("HOME"), file_name);

	return path;
}

int create_file(const char* path_to_file){
	int fd = 0;
	int perms = 0666;

	fd = creat(path_to_file, perms);
	if (fd == -1)
		return 1;

	if (close(fd) == -1)
		return 1;

	return 0;
}

/*
 * Checking the file for existence. If it does not exist, then
 * create it. If everything went well, return 0.
 */
int check_file(const char* path_to_file){
	if (!access(path_to_file, F_OK))
		return 0;

	return create_file(path_to_file);
}



int main(void){
	const char* path_to_file = NULL;

	// Working with a notes file
	path_to_file = get_path_to_file(FILE_NAME);
	if (check_file(path_to_file)){
		fprintf(stderr, "Failed to create notes file.");
		exit(1);
	}

	return 0;
}

