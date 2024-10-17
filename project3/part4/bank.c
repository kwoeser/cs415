#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "string_parser.h"
#include "string_parser.c"
#include "account.h"


// ONLY WORKS WHEN I DO valgrind ./bank input.txt???
#define MAX_LINE_LENGTH 256
#define MAX_THREADS 10
#define UPDATE 5000

pthread_t thread_id[MAX_THREADS], bank_thread;
int thread_args[MAX_THREADS], current_threads = 10, transactions, waiting_work, waiting_bank, requests; 
pthread_mutex_t workerTID_lock, bankTID_lock, bank_wait, trans_locks = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bankTID_cond, workTID_cond = PTHREAD_COND_INITIALIZER; 
pthread_barrier_t starter_barrier;

// Have to change to global
account *accounts;
command_line *token;


void read_account(FILE *fp, account *accounts);
void thread_handling();
void strip_line(char **buf, size_t *size, FILE *fp);
int check_pass(char *acct, char *password);
void *process_transaction(void* arg);
void *update_balance();
int transactions_count(FILE* fp);

void read_account(FILE *fp, account *accounts) {
    char* buf = NULL;
    size_t size = MAX_LINE_LENGTH;
    int i = 0;    

    // Loops through file and reads each line at a time ignoring the index # line then copies it to buffer
    getline(&buf, &size, fp);
    while (i < MAX_THREADS && fgets(buf, size, fp) != NULL) {
        // Read account number line
        strip_line(&buf, &size, fp);
        strcpy(accounts[i].account_number, buf);
        
        // Read password line
        strip_line(&buf, &size, fp);
        strcpy(accounts[i].password, buf);
        
        // Read balance line and double conversion
        strip_line(&buf, &size, fp);
        accounts[i].balance = atof(buf);
        
        // read reward rate line and double conversion
        strip_line(&buf, &size, fp);
        accounts[i].reward_rate = atof(buf);
        accounts[i].transaction_tracker = 0;
        pthread_mutex_init(&accounts[i].ac_lock, NULL);
        i++;
    }

    token = malloc(sizeof(command_line) * transactions);
    int index = 0;
    while (getline(&buf, &size, fp) != EOF) {
        token[index] = str_filler(buf, " ");
        index++;
    }
    free(buf);
    fclose(fp);
    pthread_barrier_init(&starter_barrier, NULL, MAX_THREADS + 2);
}

void thread_handling() {
    for (int i = 0; i < MAX_THREADS; i++) {
        thread_args[i] = i;
    }

    // Create worker threads
    for (int i = 0; i < MAX_THREADS; i++) {
        int ret = pthread_create(&thread_id[i], NULL, &process_transaction, &thread_args[i]);
        if (ret != 0) {
            printf("Thread could not be created\n");
        }
    }
    
    // Create bank thread
    int error = pthread_create(&bank_thread, NULL, &update_balance, NULL);
    if (error != 0) {
        printf("Thread could not be created\n");
    }

    pthread_barrier_wait(&starter_barrier);

    // Join worker threads
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(thread_id[i], NULL);
    }
    
    // Join bank thread
    pthread_join(bank_thread, NULL);
    printf("The bank thread is done\n");
    for (int i = 0; i < MAX_THREADS; i++){
		pthread_mutex_destroy(&accounts[i].ac_lock);
	}

    // destroy everything
    pthread_barrier_destroy(&starter_barrier);
    pthread_mutex_destroy(&bankTID_lock);
	pthread_mutex_destroy(&bank_wait);
    pthread_mutex_destroy(&workerTID_lock);
    pthread_mutex_destroy(&trans_locks);
	pthread_exit(NULL);
}

void strip_line(char **buf, size_t *size, FILE *fp) {
    char *saveptr;
    getline(buf, size, fp);
    // remove the \n at the end of the line
    strtok_r(*buf, "\n", &saveptr);
}


// Might now work issues!
int check_pass(char *acct, char *password) {
	for(int i = 0; i < MAX_THREADS; i++) {
		if(strcmp(accounts[i].account_number, acct) == 0) {
			if(strcmp(accounts[i].password, password) == 0) {
				return 0;
			}
            else {
				return 1;
			}
		}
	} 
    // Return anything other then 0 but all accounts will be checked before this
    return 2;
}


// Struct to use for index handling globally NOT USEFUL? HANDLE TMR
typedef struct {
    int start_index;
    int end_index;
} worker_index;


void *process_transaction(void* arg) {
    pthread_barrier_wait(&starter_barrier);
    worker_index* argument = malloc(sizeof(worker_index));
    int thread_transactions = transactions / MAX_THREADS;
    argument->start_index = *(int*)arg * thread_transactions;
    argument->end_index = argument->start_index + thread_transactions;

    for (int start = argument->start_index; start < argument->end_index; start++) {
        command_line cmd = token[start];
        int current, dest;

        if (check_pass(cmd.command_list[1], cmd.command_list[2]) == true) {
            printf("Incorect password! Account %s\n", cmd.command_list[1]);
        } else {

            // Loop through accounts till correct account is found and change index to it
            for (int i = 0; i < MAX_THREADS; i++) {
                if (strcmp(cmd.command_list[1], accounts[i].account_number) == 0) {
                    current = i;
                    break;
                }
            }
            
            if (current != -1 && strcmp(accounts[current].password, cmd.command_list[2]) == 0) {
                // If the first letter is T,C,D,W do its corresponding action
                if (strcmp(cmd.command_list[0], "T") == 0) {
                    for (int j = 0; j < MAX_THREADS; j++) {
                        if (strcmp(cmd.command_list[3], accounts[j].account_number) == 0) {
                            dest = j;
                            break;
                        }
                    }
                    // Convert the 4th command in transfer line to double     
                    double money = atof(cmd.command_list[4]); 
                    // Lock array from other threads
                    pthread_mutex_lock(&accounts[current].ac_lock);
                    accounts[dest].balance += money;
                    pthread_mutex_unlock(&accounts[current].ac_lock);
                    pthread_mutex_lock(&accounts[current].ac_lock);
                    accounts[current].balance -= money;
                    accounts[current].transaction_tracker += money;
                    pthread_mutex_unlock(&accounts[current].ac_lock);
                } 
                
                if (strcmp(cmd.command_list[0], "C") == 0) {
                    // prints/checks current balance of account
                    printf("Account #%s Current balance: %.2lf\n", accounts[current].account_number, accounts[current].balance);
                } 
                
                if (strcmp(cmd.command_list[0], "D") == 0) {
                    // Deposit money
                    double add_money = atof(cmd.command_list[3]);
                    pthread_mutex_lock(&accounts[current].ac_lock);
                    accounts[current].balance += add_money;
                    accounts[current].transaction_tracker += add_money;
                    pthread_mutex_unlock(&accounts[current].ac_lock);
                } 
                
                if (strcmp(cmd.command_list[0], "W") == 0) {
                    // Withdraw money
                    double remove_money = atof(cmd.command_list[3]);
                    pthread_mutex_lock(&accounts[current].ac_lock);
                    accounts[current].balance -= remove_money;
                    accounts[current].transaction_tracker += remove_money;
                    pthread_mutex_unlock(&accounts[current].ac_lock);
                }

                pthread_mutex_lock(&trans_locks);
                requests++;
                pthread_mutex_unlock(&trans_locks);
                
                // number of requests processed across all threads reaches 5000
                if (requests >= UPDATE){
                    pthread_mutex_lock(&bank_wait);
                    waiting_work++;
                    if (current_threads == waiting_work){
                        while (waiting_bank == 0){
                            continue;
                        }
                        pthread_mutex_lock(&bankTID_lock);
                        pthread_cond_broadcast(&bankTID_cond);
                        pthread_mutex_unlock(&bankTID_lock);
                    }
                    pthread_cond_wait(&workTID_cond, &bank_wait);
                    pthread_mutex_unlock(&bank_wait);
                }


            }
        }
    }
    
    pthread_mutex_lock(&workerTID_lock);
    current_threads--;

    if (current_threads == 0){
		while (waiting_bank == 0){
			continue;
		}
		pthread_mutex_lock(&bankTID_lock);
		while (waiting_bank != -1){
			continue;
		}
		pthread_cond_signal(&bankTID_cond);
		pthread_mutex_unlock(&bankTID_lock);
	}
	pthread_mutex_unlock(&workerTID_lock);
	pthread_exit(NULL);
	return arg;

}

void *update_balance() {
    // Update balance of each account
    pthread_barrier_wait(&starter_barrier);
    while (current_threads != 0) {
        pthread_mutex_lock(&bankTID_lock);
        waiting_bank = -1;
        // Wait for the signal from worker threads to update balances
        pthread_cond_wait(&bankTID_cond, &bankTID_lock);
        pthread_mutex_unlock(&bankTID_lock);
        pthread_mutex_lock(&bank_wait);
        for (int i = 0; i < MAX_THREADS; i++) {
            accounts[i].balance += accounts[i].reward_rate * accounts[i].transaction_tracker;
            accounts[i].transaction_tracker = 0;
            
            // Write current balance into files made during main() 
            FILE* update_file = fopen(accounts[i].out_file, "a");
            fprintf(update_file, "Current Balance:\t%.2f\n", accounts[i].balance);
            fclose(update_file);
        
            // Write current savings balance into files made during main() 
            // FILE* update_savingfile = fopen(accounts[i].out_file, "a");
            // fprintf(update_savingfile, "Current Savings Balance\t%.2f\n", accounts[i].balance);
            // fclose(update_savingfile);
        
        }   
        


        // Reset transaction counters and signal worker threads to continue
        requests = 0;
        waiting_work = 0;
        waiting_bank = 0;
        pthread_cond_broadcast(&workTID_cond);
        pthread_mutex_unlock(&bank_wait); 
    }

    pthread_exit(NULL);
    return NULL;
}

int transactions_count(FILE* fp) {
    rewind(fp);
    
    int count = 0;
    char* buf = NULL;
    size_t s = 0;
    int line = 0;

    // Ingore the first 50 lines
    while (line <= 50) {
        if (getline(&buf, &s, fp) == EOF) {
            break; 
        }
        line++;
    } 

    while (getline(&buf, &s, fp) != EOF) {
        count++;
    }

    free(buf);
    return count;
}

int main(int argc, char* argv[]) {
    // Argument handler
    if (argc != 2) {
        printf("Error wrong input: ./part1 -f <filename>\n");
        exit(EXIT_FAILURE);
    }

    // Handling input file
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Couldn't open the File\n");
        exit(EXIT_FAILURE);
    }

    accounts = malloc(sizeof(account) * MAX_THREADS);
    if (accounts == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Create output dir and the account files
    char *dir = "output";
    mkdir(dir, 0777);
    // Creates account files
    for (int i = 0; i < MAX_THREADS; i++) {
        sprintf(accounts[i].out_file,"%s/account%d.txt", dir, i);
        FILE *account_files = fopen(accounts[i].out_file, "w");
        fprintf(account_files, "account %d:\n", i);
        fclose(account_files);
    }

    // Create savings file
    char *saving_dir = "savings";
    mkdir(saving_dir, 0777);
    // Creates savings files
    // for (int i = 0; i < MAX_THREADS; i++) {
    //     sprintf(accounts[i].out_file,"%s/saving%d.txt", saving_dir, i);
    //     FILE *savings_files = fopen(accounts[i].out_file, "w");
    //     fprintf(savings_files, "account %d:\n", i);
    //     fclose(savings_files);
    // }

    transactions = transactions_count(fp); 
    rewind(fp);

    // Read and update balances from file
    read_account(fp, accounts);
    thread_handling();
    
    // Free resources
    for (int i = 0; i < transactions; i++){
		free_command_line(&token[i]);
	}

    // Write account info into output file
    FILE *write_file = fopen("output.txt", "w");
    for (int i = 0; i < MAX_THREADS; i++){
	    fprintf(write_file, "%d balance:\t%.2lf\n\n", i, accounts[i].balance);
    }

	free(token);
    free(accounts);
    printf("FINISHED\n");
    return 0;
}












