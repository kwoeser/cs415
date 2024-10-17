#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ARG_SIZE 4096

void token_line(char *line, char **arguments) {
    // Split line into tokens based on spaces and newlines, then store then in arguemnts
    char *token;
    token = strtok(line, " \n");
    int i = 0;
    while (token != NULL && i < ARG_SIZE) {
        arguments[i++] = token;
        token = strtok(NULL, " \n");
    }
    arguments[i] = NULL;
}


void execute(pid_t *processes, FILE *fp) {
    char line[ARG_SIZE];
    int current_process = 0;

    // Read lines in file, fork, and then execute.
    while (fgets(line, sizeof(line), fp)) {
        char *args[256];
        token_line(line, args);

        // Fork a new process
        processes[current_process] = fork();
        if (processes[current_process] < 0) {
            perror("Error during forking process");
            exit(EXIT_FAILURE);
        }

        // Child process
        if (processes[current_process] == 0) { 
            // if command fails execvp returns -1
            if (execvp(args[0], args) == -1) {
                perror("Error during execvp process");
                exit(EXIT_FAILURE);
            }
        }

        printf("Starting process %d\n", processes[current_process]);
        // Next process
        current_process++;  
    }
}


void signal_handler(pid_t* arr, int size, int sig_name) {
    // Sleep before sending signal
    sleep(1); 
    for(int i = 0; i < size; i++) {
        if (kill(arr[i], sig_name) == -1) {
            perror("Error: Signal couldn't be sent -> SIGNAL_HANDLER()");
        }
        
    }
}




int main(int argc, char *argv[]) {
    // Argument handler from project 1
    if ((argc != 3) || (strcmp(argv[1], "-f") != 0)) {
        printf("Error wrong input: ./part2 -f <filename>\n");
        exit(EXIT_FAILURE);
    }

    // Handling input file
    FILE *fp = fopen(argv[2], "r");
    if (fp == NULL) {
        perror("ERROR! Invalid or missing file");
        exit(EXIT_FAILURE);
    }
    
    // Get number of lines 
    int line_count = 0;
    char c;
    for (c = getc(fp); c != EOF; c = getc(fp)) {
         if (c == '\n') {
            line_count++;
        }
    }

    // Reset file pointer to the beginning
    fseek(fp, 0, SEEK_SET);

    // lab 4 and 5 
    // build our sigset
    printf("Building Sigset\n");
    printf("---------------\n");
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    // Allocate memory for process array
    pid_t *pid_array = malloc(line_count * sizeof(pid_t));
    if (pid_array == NULL) {
        perror("Error allocating memory for pid_array");
        fclose(fp);
        exit(EXIT_FAILURE);
    }


    printf("Parent Proccess: #%d\n", getpid());
    execute(pid_array, fp);
    
    // execute function when signal comes
    sleep(1); 
    printf("Sending SIGUSR1 to processes\n");
    signal_handler(pid_array, line_count, SIGUSR1); 
    printf("Sending SIGSTOP to processes\n");
    signal_handler(pid_array, line_count, SIGSTOP);   
    printf("Sending SIGCONT to processes\n");
    signal_handler(pid_array, line_count, SIGCONT);

    // Wait for all child processes to finish
    for (int j = 0; j < line_count; j++) {
        if (waitpid(pid_array[j], NULL, 0) == -1) {
            perror("Error during wait");
        }
    }

    free(pid_array);
    fclose(fp);
    printf("---------------\n");
    printf("Processes have finished.\n");
    return 0;
}

















 

 








