#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include"string_parser.h"
#include"string_parser.c"
#include "account.h"


#define MAX_LINE_LENGTH 256
#define MAX_THREADS 10

void read_account(FILE *fp, account *accounts);
void strip_line(char **buf, size_t *size, FILE *fp);
void *process_transaction(void* arg, account *accounts);
void *update_balance(account *accounts);


void read_account(FILE *fp, account *accounts) {
    char* buf = NULL;
    size_t size = MAX_LINE_LENGTH;
    int i = 0;    

    // Loops through file and reads each line at a time ignoring the index # line then copys it to buffer
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
        i++;
    }


    while (fgets(buf, size, fp) != NULL) {
        command_line *token = malloc(sizeof(command_line));
        *token = str_filler(buf, " ");
        process_transaction(token, accounts);
        free_command_line(token);
        free(token);
    }

    free(buf);
}


void strip_line(char **buf, size_t *size, FILE *fp) {
    char *saveptr;
    getline(buf, size, fp);
    // remove the \n at the end of the line, 
    strtok_r(*buf, "\n", &saveptr);
}


void *process_transaction(void* arg, account *accounts){
    int current, dest;
    char **cmd = ((command_line*)arg)->command_list;

    // Loop thorugh accounts till correct account is found and change index to it
    for (int i = 0; i < MAX_THREADS; i++){
        if (strcmp(cmd[1], accounts[i].account_number) == 0){
            current = i;
        	break;
	    }
    }
    
  
    if (strcmp(accounts[current].password, cmd[2]) == 0 && current != -1) {
        // If the first letter is T,C,D,W do it's corrosponding action
        // Transfer funds
        if (strcmp(cmd[0], "T") == 0){
            for (int i = 0; i < MAX_THREADS; i++){
                if (strcmp(cmd[3], accounts[i].account_number) == 0){
                    dest = i;
                    break;
                }
            }
            // Convert the 4th command in transfer line to double     
            double money = atof(cmd[4]); 
            accounts[dest].balance += money;
            accounts[current].balance -= money;
            accounts[current].transaction_tracker += money;            
        }

        // prints/checks current balance of account
        if (strcmp(cmd[0], "C") == 0){
            printf("Account #%s Current balance: %.2lf\n", accounts[current].account_number, accounts[current].balance);
        }
        
        // Deposit money 
        if (strcmp(cmd[0], "D") == 0){   
            double add_money = atof(cmd[3]);
            accounts[current].balance += add_money;
            accounts[current].transaction_tracker += add_money;            
        }
        
        // Withdraw money
        if (strcmp(cmd[0], "W") == 0){
            // Convert the 4th command in transfer line to double    
            double remove_money = atof(cmd[3]);
            accounts[current].balance -= remove_money;
            accounts[current].transaction_tracker += remove_money;        
        }
        
    }
    
    // printf("here\n");
    return arg;
}

void *update_balance(account *accounts) {
    // Update balance of each account
    int i = 0;
    while (i < MAX_THREADS) {
        accounts[i].balance += accounts[i].reward_rate * accounts[i].transaction_tracker;
	    accounts[i].transaction_tracker = 0;
        i++;
    }   
    return 0;
}


int main(int argc, char* argv[]){
     // Argument handler
    if (argc != 2){
        printf("Error wrong input: ./part1 -f <filename>\n");
        exit(EXIT_FAILURE);
    }

    // Handling input file
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL){
	    printf("Couldn't open the FIle");
    }

    account *accounts = malloc(sizeof(account) * MAX_THREADS);
    // Read and update balances from file
    read_account(fp, accounts);
    update_balance(accounts);
    fclose(fp);
    
    // Write account info into output file
    FILE *write_file = fopen("output.txt", "w");
    for (int i = 0; i < MAX_THREADS; i++){
	    fprintf(write_file, "%d balance:\t%.2lf\n\n", i, accounts[i].balance);
    }
    
    fclose(write_file);
    free(accounts);
    printf("FINISHED\n");
    return 0;
}
