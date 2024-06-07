#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define FILE_NAME ".note.txt"
#define SIZE_LINE 1024

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

/*
 * Wrapper over file opening. Opens files and terminates the program
 * if it fails to open the file. Returns FILE on success.
 */
FILE* open_file(const char* path_to_file, const char* mode){
	FILE* file;

	file = fopen(path_to_file, mode);
	if (file == NULL){
		perror("open file");
		exit(errno);
	}

	return file;
}

ssize_t count_lines(const char* path_to_file){
	ssize_t count = 0;
	char ch = 0;
	FILE* file;

	file = open_file(path_to_file, "r");

	while ((ch = fgetc(file)) != EOF){
		if (ch == '\n')
			count++;
	}

	fclose(file);
	return count;
}

/*
 * Loading data into the passed array. If successful, returns an
 * array; if unsuccessful, exits the program with an error code.
 */
char** load_data(const char* path_to_file, ssize_t* count_line){
	char** array = NULL;
	FILE* file = NULL;
	ssize_t index = 0;

	(*count_line) = count_lines(path_to_file);

	file = open_file(path_to_file, "r");

	array = malloc((*count_line) * sizeof(char*));
	for (ssize_t i = 0; i < (*count_line); i++)
		array[i] = malloc(SIZE_LINE * sizeof(char));
	
	while (fgets(array[index++], SIZE_LINE, file) != NULL);

	fclose(file);
	return array;
}

/*
 * Uploading data back to a file and freeing up memory.
 */
void unload_data(const char* path_to_file, char*** array,
		const ssize_t count_line){
	FILE* file;

	file = open_file(path_to_file, "w");

	for (ssize_t i = 0; i < count_line; i++){
		fputs((*array)[i], file);
		free((*array)[i]);
	}

	fclose(file);
}

void print_options(const char* options, const char* description){
	printf("\t%-20s\t%s\n", options, description);
}

void print_help(const char* name_program){
	printf("\nA program for saving notes in the Linux terminal.\n\n");
	printf("Usage: %s [OPTIONS] [VALUE] ...\n\n", name_program);
	print_options("-a \"[NOTE]\"", "Add a note");
	print_options("-d [INDEX NOTE]", "Delete note");
	print_options("-s", "Show all notes");
	print_options("-h", "Display this message.");
	putchar('\n');
}

void print_error(const char* text_error, const ssize_t error_code){
	fprintf(stderr, "[ERROR %i]: %s\n", error_code, text_error);
	exit(error_code);
}

void print_elem(const char* text, const ssize_t index){
	printf("[%i]: %s", index, text);
}

/*
 * Adding an element (note) to the passed array with memory
 * allocation.
 */
void add_elem(char*** array, ssize_t* count_line, const char* elem){
	if (strlen(elem) >= SIZE_LINE)
		print_error("The note is too long.", 2);

	(*array) = realloc((*array), ++(*count_line) * sizeof(char*));
	(*array)[(*count_line) - 1] = malloc(SIZE_LINE * sizeof(char));

	sprintf((*array)[(*count_line) - 1], "%s\n", elem);
}

/*
 * Deleting an element (note) by its index.
 */
void delete_elem(char*** array, ssize_t* count_line,
		const ssize_t index){
	if (index >= (*count_line))
		print_error("The index entered is too large.", 3);

	for (ssize_t i = index; i < (*count_line) - 1; i++){
		strcpy((*array)[i], (*array)[i + 1]);
	}
	free((*array)[*count_line - 1]);

	(*count_line)--;
}

void show_elems(char** array, const ssize_t count_elem){
	for (ssize_t i = 0; i < count_elem; i++)
		print_elem(array[i], i);
}



int main(int argc, char** argv){
	const char* path_to_file = NULL;

	char** notes = NULL;
	ssize_t count_line = 0;

	int result_arg = 0;

	if (argc < 2){
		print_help(argv[0]);
		return 0;
	}

	// Working with a notes file
	path_to_file = get_path_to_file(FILE_NAME);
	if (check_file(path_to_file)){
		print_error("Failed to create notes file.", 1);
		exit(1);
	}

	notes = load_data(path_to_file, &count_line);

	while ((result_arg = getopt(argc, argv, "a:d:sh")) != -1){
		switch (result_arg){
			case 'a':
				add_elem(&notes, &count_line, optarg);
				break;
			case 'd':
				delete_elem(&notes, &count_line, atoi(optarg));
				break;
			case 's':
				show_elems(notes, count_line);
				break;
			case 'h':
				print_help(argv[0]);
				break;
		}
	}

	unload_data(path_to_file, &notes, count_line);
	return 0;
}

