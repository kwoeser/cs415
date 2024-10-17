#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ARG_SIZE 4096
// Changed to global variables
pid_t *pid_array;
int current = 0;
int line_count = 0;

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


void execute(pid_t *processes, FILE *fp, sigset_t sigs) {
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
            int sig_i;
            sigwait(&sigs, &sig_i);
            if (execvp(args[0], args) == -1) {
                perror("Error during execvp process");
                exit(EXIT_FAILURE);
            }
        }

        // Next process
        current_process++;  
    }
}


void signal_handler(pid_t* arr, int size, int sig_name) {
    for(int i = 0; i < size; i++) {
        if (kill(arr[i], sig_name) == -1) {
            perror("Error: Signal couldn't be sent -> SIGNAL_HANDLER()");
        }
        
    }
}



void alarmsig_handler(int signum) {
    // Stop the current process, go to the next process and then continue
    printf("\nALARM SIGNAL!\n");
    printf("Loading Process %d\n", pid_array[current]);
    kill(pid_array[current], SIGSTOP);
    current = (current + 1) % line_count;
    kill(pid_array[current], SIGCONT);

    // Reset the alarm
    alarm(2);
}




int main(int argc, char *argv[]) {
    // Argument handler from project 1
    if((argc == 3) && (strcmp(argv[1], "-f")) != 0) {
        printf("Error wrong input: ./part3 -f <filename>\n");
        exit(EXIT_FAILURE);
    }

    // Handling input file
    FILE *fp = fopen(argv[2] , "r");
    if(fp == NULL){
        perror("ERROR! Invalid or missing file.");
        exit(EXIT_FAILURE);
    }
    
    // Get number of lines 
    char c;
    for (c = getc(fp); c != EOF; c = getc(fp)) {
         if (c == '\n') {
            line_count++;
        }
    }

    // Reset file pointer to the beginning
    fseek(fp, 0, SEEK_SET);


    printf("Building Sigset\n");
    printf("---------------\n");
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);


    // Allocate memory for process array
    pid_array = malloc(line_count * sizeof(pid_t));
    if (pid_array == NULL) {
        perror("Error allocating memory for pid_array");
        fclose(fp);
        exit(EXIT_FAILURE);
    }


    printf("Parent Proccess: #%d\n", getpid());
    execute(pid_array, fp, sigset); 


    // https://www.youtube.com/watch?v=vb-awqgeKvM
    // setup signal SIGALRM and start round-robin scheduling 
    // if (signal(SIGALRM, alarmsig_handler) == SIG_ERR) {
    //     perror("Failed to register SIGALRM handler");
    //     exit(EXIT_FAILURE);
    // }
    signal(SIGALRM, alarmsig_handler);
    signal_handler(pid_array, line_count, SIGUSR1);
    signal_handler(pid_array, line_count, SIGSTOP);
    kill(pid_array[current], SIGCONT);

    // reset alarm
    alarm(2);



    // New variable to handle waiting so no conflict occurs with line_count
    int process_finished = line_count;
    // Wait for child processes to finish
    while (process_finished > 0) {
        int status;
        pid_t process = waitpid(-1, &status, WNOHANG);
        if (process > 0) {
            //print finihsed processes decrement one from process_finished
            if (WIFEXITED(status)) {
                // printf("Process #%d finished\n", process);
                process_finished--;
            }
        } 
    }

    free(pid_array);
    fclose(fp);
    printf("---------------\n");
    printf("Processes have finished.\n");
    return 0;
}


