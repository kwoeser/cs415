#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// #define MAX_LINE_LENGTH 256
// #define TRANSACTION_THRESHOLD 5000

typedef struct {
    char account_number[17];
    char password[9];
    double balance;
    double reward_rate;
    double transaction_tracker;
    char out_file[64];
    pthread_mutex_t ac_lock;
} account;

#endif /* ACCOUNT_H_ */
