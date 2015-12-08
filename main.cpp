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
static const int REQS = 5000;

// the number of bank accounts
static const int NUM_ACCTS = 4;

// initialize each account to one thousand dollars
static const int INIT_BAL = 10000;

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

static const int MUTEX = 0;



/*
 * Account index within bank array
 */

static const int CHK_ACCT = 0;
static const int SAV_ACCT = 1;
static const int VAC_ACCT = 2;
static const int IRA_ACCT = 3;


static const char* names[4] = { "Checkings", "Savings", "Vacation", "IRA" };
static const char* ops[5] = { "Deposits", "Withdraws", "Transfers", 
	"Random Transfers", "IRA Deposits" };

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

bool canDecrement(int val) {
	return (--val >= 0);
}

void alert() {
	cout << "Transfer Denied" << endl;
}

int main(int argc, const char * argv[]) {
    
   	//set up the four seperate integers for the four seperate bank accounts
    int shmid[NUM_ACCTS + 5];
    
    // array holds references to all accounts in shared memory
    int *bank[NUM_ACCTS];
    
    int *count[4];
    
    for(int x = 0; x < NUM_ACCTS; x++){
    	shmid[x] = shmget(IPC_PRIVATE, sizeof(int), PERMS);
        bank[x] = (int *)shmat(shmid[x], NULL, 0);
    }
    
    for (int z = 0; z < 4; z++) {
    	shmid[z + 4] = shmget(IPC_PRIVATE, sizeof(int), PERMS);
        count[z] = (int *)shmat(shmid[z + 4], NULL, 0);
    }

    int money = INIT_BAL;
    for(int y = 0; y < NUM_ACCTS; y++){
        *bank[y] = money;
    }
    
    // use four semaphores to restrict concurrent access to four accounts
    SEMAPHORE sem(1);
    sem.V(MUTEX);
    
    int childProcess;
    
    int p;
    for(p = 0; p < NUM_PROC; p++) {
        
        childProcess = fork();
        
        if(childProcess == 0) {
            
            for(int s = 0; s < REQS; s++){
                
                int request = randomRequest();
                switch(request)
                {
                    case 1: {
                        int account = randomAccount();
                        sem.P(MUTEX);
                        deposit(bank[account]);
                        *count[0] = *count[0] + 1;
                        sem.V(MUTEX);
                        break;
                    }
                    case 2: {
                        int account = randomAccount();
                        sem.P(MUTEX);
                        withdraw(bank[account]);
						*count[1] = *count[1] + 1;
                        sem.V(MUTEX);
                        break;
                    }
                    case 3: {
                        int account_from = randomAccount();
                        int account_to = randomAccount();
                        while(account_to == account_from){
                            account_from = randomAccount();
                        }
                        sem.P(MUTEX);
                        transfer(bank[account_from] , bank[account_to]);
                        *count[2] = *count[2] + 1;
                        sem.V(MUTEX);
                        break;
                    }
                    case 4: {
                        sem.P(MUTEX);
                        transferToChecking(bank[SAV_ACCT] , bank[VAC_ACCT] ,bank[CHK_ACCT]);
                        *count[3] = *count[3] + 1;
                        sem.V(MUTEX);
                        break;
                    }
                    case 5: {
                        int account_w = randomAccount();
                        while(account_w == IRA_ACCT){
                            account_w = randomAccount();
                        }
                        sem.P(MUTEX);
                        depositIRA(bank[IRA_ACCT] , bank[account_w]);
                        sem.V(MUTEX);
                        break;
                    }
                    default: {
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
    
    
    cout << endl;
    
    // print operation percentages
    int sum = *count[0] + *count[1] + *count[2] + *count[3];
    int n = NUM_PROC * REQS;
   	for (int i = 0; i < 5; i++) {
   		double c;
   		if (i < 4) {
   			c = *count[i];
   		} else {
   			c = n - sum;
   		}
   		
   		printf("%-17s %5.2lf%%\n", ops[i], 100 * (c / (double)n));
   	}
   	
   	cout << endl;
   	
    // output and clean up
    for (int x = 0; x < NUM_ACCTS; x++){
        printf("$%d in %s account\n", *bank[x], names[x]);
    }
    
    for (int y = 0; y < 9; y++) {
    	shmctl(shmid[y], IPC_RMID, NULL);
    };
    
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
    
    while (true) {
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
			// try again
        }
    }
}


void deposit(int *account) {
    (*account)++;
}

void withdraw(int *account) {
    if (canDecrement(*account)) {
    	(*account)--;
    } else {
    	alert();
    }
}

void transfer( int *account_from , int *account_to ){
    
    if (canDecrement(*account_from)) {
    	(*account_from)--;
    	(*account_to)++;
    	}
    else {
    	alert();
    }   
}
		
void transferToChecking( int *savings , int *vacation , int *checking){
    
    if (canDecrement(*savings) && canDecrement(*vacation)) {
		(*savings)--;
		(*vacation)--;
		(*checking)++;
	} else {
		alert();
    }
}

void depositIRA(int *IRA , int *account_from){

    if (canDecrement(*account_from)) {    	
		(*account_from)--;
		(*IRA)++;
	} else {
		alert();
	}
}


