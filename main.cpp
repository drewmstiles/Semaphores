//
//  main.cpp
//  semaphore_project
//
//  Created by Tyler Cobb on 11/28/15.
//  Copyright (c) 2015 Tyler Cobb. All rights reserved.
//


#include <iostream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cctype>
#include <cstdlib>
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include "semaphore.h"

using namespace std;

// one millisecond is one thousand microseconds
static const int ONE_MS = 1000;

// number of concurrent processes
static const int NUM_PROC = 6;

// the number of requests by each process
static const int REQS = 500;

// the number of bank accounts
static const int NUM_ACCTS = 4;

// the amount exchanged during a transaction
static const int PAYMENT = 1;

/*
 *  Service requests for user processes
 */

// add to a random account
static const int ADD = 3001;

// remove from a random account
static const int REM = 503;

// transfer to an account
static const int TRAN = 10007;

// transfer from Vacation and Savings to Checking
static const int VSTRAN = 20011;

// add to IRA
static const int IADD = 999331;


/*
 * Semaphores for each account
 */

static const int CHK_SEM = 0;
static const int SAV_SEM = 1;
static const int VAC_SEM = 2;
static const int IRA_SEM = 3;

static const int locks[4] = {CHK_SEM, SAV_SEM, VAC_SEM, IRA_SEM};


/*
 * Account index within bank array
 */

static const int CHK_ACCT = 0;
static const int SAV_ACCT = 1;
static const int VAC_ACCT = 2;
static const int IRA_ACCT = 3;


static const char* names[4] = { "Checkings", "Savings", "Vacation", "IRA" };

// Summary:
//
//  Returns one of the five service requests.
//
// Returns:
//
//      an integer denoting the service to request
//
int randomRequest();

void deposit(int *account);

void withdraw(int *account);

int randomAccount();

// Summary:
//
//  Transfers a fixed amount between two random user bank accounts
//
// Arguments:
//
//      two integer pointers to random generated user bank accounts
//
void transfer( int * account_from , int * account_to );

// Summary:
//
//  Transfers a fixed amount from the savings and vacation user bank
//  accounts to the user checking bank account
//
// Arguments:
//
//      three integer pointers; the savings, vacation and checking
//      bank accounts.
//
void transferToChecking( int * savings , int * vacation , int * checking);

void depositIRA(int * IRA , int * account_from);

/*
 * Function definitions
 */


int main(int argc, const char * argv[]) {
    
   	//set up the four seperate integers for the four seperate bank accounts
    int shmid[NUM_ACCTS];
    
    // array holds references to all accounts in shared memory
    int *bank[NUM_ACCTS];
    
    for(int x = 0; x < NUM_ACCTS; x++){
        shmid[x] = shmget(IPC_PRIVATE, sizeof(int), PERMS);
        bank[x] = (int *)shmat(shmid[x], 0, SHM_RND);
    }
    
    int money = 10000;
    for(int y = 0; y < NUM_ACCTS; y++){
        *bank[y] = money;
    }
    
    // use four semaphores to restrict concurrent access to four accounts
    SEMAPHORE sem(4);
    sem.V(CHK_SEM);
    sem.V(SAV_SEM);
    sem.V(VAC_SEM);
    sem.V(IRA_SEM);
    
    int childProcess;
    
    int p;
    for(p = 0; p < NUM_PROC; p++) {
        
        childProcess = fork();
        
        if(childProcess == 0) {
            
            for(int s = 0; s < 200; s++){
                
                int request = randomRequest();
                switch(request)
                {
                    case 1: {
                        int account = randomAccount();
                        sem.P(locks[account]);
                        printf("Deposit to %s account\n", names[account]);
                        deposit(bank[account]);
                        sem.V(locks[account]);
                        break;
                    }
                    case 2: {
                        int account = randomAccount();
                        sem.P(locks[account]);
                        printf("Withdraw from %s account\n", names[account]);
                        withdraw(bank[account]);
                        sem.V(locks[account]);
                        break;
                    }
                    case 3: {
                        //printf("request = %d\n", request);
                        int account_from = randomAccount();
                        int account_to = randomAccount();
                        while(account_to == account_from){
                            account_from = randomAccount();
                        }
                        
                        sem.P(locks[account_to]);
                        sem.P(locks[account_from]);
                        transfer(bank[account_from] , bank[account_to]);
                        printf("Transfer from %s account to %s account\n ", names[account_from] , names[account_to]);
                        sem.V(locks[account_to]);
                        sem.V(locks[account_from]);
                        break;
                    }
                    case 4: {
                        sem.P(locks[SAV_SEM]);
                        sem.P(locks[VAC_SEM]);
                        sem.P(locks[CHK_SEM]);
                        transferToChecking(bank[SAV_ACCT] , bank[VAC_ACCT] ,bank[CHK_ACCT]);
                        printf("Transfer from %s and %s account to %s account\n ", names[SAV_ACCT] , names[VAC_ACCT], names[CHK_ACCT]);
                        sem.V(locks[SAV_SEM]);
                        sem.V(locks[VAC_SEM]);
                        sem.V(locks[CHK_SEM]);
                        break;
                    }
                    case 5: {
                        
                        int account_w = randomAccount();
                        while(account_w == IRA_ACCT){
                            account_w = randomAccount();
                        }
                        sem.P(locks[account_w]);
                        sem.P(locks[IRA_SEM]);
                        depositIRA(bank[IRA_ACCT] , bank[account_w] );
                        printf("Deposit from %s account to %s account\n", names[account_w] , names[IRA_ACCT]);
                        sem.V(locks[account_w]);
                        sem.V(locks[IRA_SEM]);
                        break;
                    }
                    default: {
                            printf("d\n");
//                         printf("ERROR - Default on request = %d\n", request);
                        break;
                    }
                }
                
                
            }
            exit(0);
        }
    }
    
    // waiting for all children to exit
    int status;
    pid_t pid;
    while (p > 0) {
        pid = wait(&status);
        printf("pid (%d) exits\n", pid);
        --p;
    }
    
    // output and clean up
    for (int x = 0; x < NUM_ACCTS; x++){
        printf("$%d in %s account\n", *bank[x], names[x]);
        shmctl(shmid[x], IPC_RMID, NULL);
    }
    
    sem.remove();
    
    exit(0);
}
int randomAccount()
{
    default_random_engine generator(random_device{}());
    uniform_int_distribution<int> distribution(0, NUM_ACCTS - 1);
    return distribution(generator);
}
int randomRequest() {
    default_random_engine generator(random_device{}());
    uniform_int_distribution<int> distribution(0, pow(2.0, 32.0) - 1.0);
    
    // TODO This rarely generates 5 as the return type
//     while (true) {
        int random = distribution(generator);
        if (random % ADD == 0) {
            return 1;
        } else if (random % REM == 0) {
            return 2;
        } else if (random % TRAN == 0) {
            return 3;
        } else if (random % VSTRAN == 0) {
            return 4;
        } else if (random % IADD == 0) {
            return 5;
        } else {
return 0;
        }
//     }
}
void deposit(int *account) {
    int bal = *account;
    usleep(ONE_MS);
    *account = (bal + 1);
}
void withdraw(int *account) {
    int oldBalance = *account;
    int newBalance = oldBalance - PAYMENT;
    if (newBalance < 0) {
         		printf("Withdraw Notice - Insufficient Funds (BAL = $%d, REQ = $%d)\n",
         			oldBalance, PAYMENT);
    }
    else {
        *account = newBalance;
    }
}

void transfer( int * account_from , int * account_to ){
    
    *account_from = *account_from - 1;
    *account_to = *account_to + 1;
    
}

void transferToChecking( int * savings , int * vacation , int * checking){
    
    *savings = *savings - 1;
    *vacation = *vacation - 1;
    *checking = *checking + 2;
    usleep(ONE_MS);
    
}

void depositIRA(int * IRA , int * account_from){
    
    *account_from = *account_from - 1;
    *IRA = *IRA + 1;
    usleep(ONE_MS);
    
}







