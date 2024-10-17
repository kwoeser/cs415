#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "string_parser.h"
#include "command.h"


void run_command(command_line small_token) {
    // small_token.command_list[0] = command
    // small_token.command_list[i + 1] = arguements
    char *command = small_token.command_list[0];
    int parameters = 0;
    
    // Counter for the number of parameters in command
    while (small_token.command_list[parameters + 1] != NULL) {
        parameters++;
    }
    
    
    
    // Commands: ls, pwd, mkdir, cd, cp, mv, rm, cat
    if(strcmp(command, "ls") == 0) {
        if (parameters > 0) {
            fprintf(stdout, "Error! Unsupported parameters for command: %s\n", command);
        } else {
            listDir();
        }
    } else if(strcmp(command, "pwd") == 0) {
        if (parameters > 0) {
            fprintf(stdout, "Error! Unsupported parameters for command: %s\n", command);
        } else {
            showCurrentDir();
        }
    } else if(strcmp(command, "mkdir") == 0) {
        if (parameters != 1) {
            fprintf(stdout, "Error! Unsupported parameters for command: %s\n", command);
        } else {
            makeDir(small_token.command_list[1]);
        }
    } else if(strcmp(command, "cd") == 0) {
        if (parameters != 1) {
            fprintf(stdout, "Error! Unsupported parameters for command: %s\n", command);
        } else {
            changeDir(small_token.command_list[1]);
        }
    } else if(strcmp(command, "cp") == 0) {
        if (parameters != 2) {
            fprintf(stdout, "Error! Unsupported parameters for command: %s\n", command);
        } else {
            copyFile(small_token.command_list[1], small_token.command_list[2]);
        }
    } else if(strcmp(command, "mv") == 0) {
        if (parameters != 2) {
            fprintf(stdout, "Error! Unsupported parameters for command: %s\n", command);
        } else {
            moveFile(small_token.command_list[1],small_token.command_list[2]);
        }
    } else if(strcmp(command, "rm") == 0) {
        if (parameters != 1) {
            fprintf(stdout, "Error! Unsupported parameters for command: %s\n", command);
        } else {
            deleteFile(small_token.command_list[1]);
        }
    } else if(strcmp(command, "cat") == 0) {
        if (parameters != 1) {
            fprintf(stdout, "Error! Unsupported parameters for command: %s\n", command);
        } else {
            displayFile(small_token.command_list[1]);
        }
    } else {
        write(1, "Error! Unrecognized command: ", 29);
        write(1, command, strlen(command));
        write(1, "\n", 1);
    }
}

// Make and run pseudo-shell
void interactive_mode(){
    printf(">>> ");
    size_t len;
    char *line = NULL;
    command_line large_token_buf;
    command_line small_token_buf;

    // Reads commands until crtl c or exit command is entered 
    while (getline (&line, &len, stdin) != -1) {
                // large token is will be spaced by ;
		large_token_buf = str_filler(line, ";");
        	int i = 0;
        	
                while (large_token_buf.command_list[i] != NULL) {
			// small token is will be spaced by " "
			small_token_buf = str_filler(large_token_buf.command_list[i], " ");
			
			// if command exit is entered free all memory and files then exit
			if(strcmp(small_token_buf.command_list[0], "exit") == 0){
			        free(line);
				free_command_line(&small_token_buf);
				free_command_line(&large_token_buf);			
				exit(0); 
			// Otherwise call the run_command function
			} else {
			  run_command(small_token_buf);
			  i++;
			}
                        
                        // Free and reset buffers
			free_command_line(&small_token_buf);
			memset (&small_token_buf, 0, sizeof(small_token_buf));
		
		}
		free_command_line (&large_token_buf);
		memset (&large_token_buf, 0, sizeof(small_token_buf));
		//large_token_buf = NULL;
		printf(">>> ");
	}
    free(line);
}



// Make and run pseudo-shell -f input.txt
void file_mode(FILE *input_file, FILE *output_file){
    size_t len;
    char *line = NULL;
    command_line large_token_buf;
    command_line small_token_buf;

    // Read through input file
    while (getline(&line, &len, input_file) != -1) {
		// large token is tokened by ;
		large_token_buf = str_filler(line, ";");
        	int i = 0;
        	
                while (large_token_buf.command_list[i] != NULL) {
			// small token is tokened by " "
			small_token_buf = str_filler(large_token_buf.command_list[i], " ");
			
			// if command exit is entered free all memory and files then exit
			if(strcmp(small_token_buf.command_list[0], "exit") == 0){
			        free(line);
				free_command_line(&small_token_buf);
				free_command_line(&large_token_buf);
				fclose(input_file);
				fclose(output_file);
				exit(0); 
			// Otherwise call the run_command function and incremnet i
			} else {
			  run_command(small_token_buf);
			  i++;
			}

			free_command_line(&small_token_buf);
			memset (&small_token_buf, 0, sizeof(small_token_buf));
		
		}
		free_command_line (&large_token_buf);
		memset (&large_token_buf, 0, sizeof(small_token_buf));
		
	}
    free(line);
    fclose(input_file);
    fclose(output_file);
}



// Memory error somewhere, ls ; causes error but still works
int main(int argc, char *argv[])
{
    if ((argc == 1)) {
        interactive_mode();

    } else if((argc == 3) && (strcmp(argv[1], "-f")) == 0){
        // Handling input file
        FILE *in;
        in = fopen(argv[2], "r");
        if(in == NULL){
            printf("ERROR! Invalid or missing file.\n");
	    return 0;
	}
	
	// Handling output file 
	FILE *out;
	out = freopen("output.txt", "w", stdout);
	//int out = open("output.txt", O_WRONLY | O_CREAT, 0777);
        file_mode(in, out);
    }
    return 0;
}
