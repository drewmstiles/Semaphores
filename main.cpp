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

// used to signal a status write by the executing agent
static const int MILESTONE = 1000;

// the number of bank accounts
static const int NUM_ACCTS = 4;

// the number of bank accounts
static const int NUM_REQS = 5;

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

// account names
static const char* names[NUM_ACCTS] = { "Checkings", "Savings", "Vacation", "IRA" };

// request names
static const char* requests[NUM_REQS] = { 
	"Deposit", 
	"Withdraw", 
	"Transfer",
	"Transfer to Checking", 
	"Deposit to IRA"
};


// ==================== Function Prototypes ====================


int randomAccount();
// Summary:
//
//  get an integer index for one of the four accounts
//
// Returns:
//
//      an integer index denoting the service to request


int randomRequest();
// Summary:
//
//  Returns one of the five service requests.
//
// Returns:
//
//      an integer denoting the service to request


void deposit(int *account);
// Summary:
//
//  Returns one of the five service requests.
//
// Arguments:
//
//      account		the account to deposit into


void withdraw(int *account);
// Summary:
//
//  Returns one of the five service requests.
//
// Arguments:
//
//      account		the account to withdraw from


void transfer( int * account_from , int * account_to );
// Summary:
//
//  Transfers a fixed amount between two random user bank accounts
//
// Arguments:
//
//      two integer pointers to random generated user bank accounts


void transferToChecking( int * savings , int * vacation , int * checking);
// Summary:
//
//  Transfers a fixed amount from the savings and vacation user bank
//  accounts to the user checking bank account
//
// Arguments:
//
//      three integer pointers; the savings, vacation and checking
//      bank accounts.


void depositIRA(int * IRA , int * account_from);
// Summary:
//
//  Deposits a fixed amount from a random account to the IRA banks account
//
// Arguments:
//
//      two integer pointers; one for the random account that the fixed
//      amount is being withdrawn from, and another for the IRA bank
//      account.


bool canDecrement(int val);
// Summary:
//
//  Checks the current value in the bank account to assure the account's
//  balance won't be negative
//
// Arguments:
//
//      this function passing the intger value of the current balance of
//      the selected account. This function returns a boolean value, true
//      if the balance after the decremnet is greater than 0, false if
//      the balance is negative.


void alert();
// Summary:
//
//  Prints out an alert message that the transfer cannot be performed.
//



// ==================== Function Definitions ====================


int main(int argc, const char * argv[]) {
    
    int shmid[NUM_ACCTS];
    int *bank[NUM_ACCTS];
    for(int a = 0; a < NUM_ACCTS; a++) {
        shmid[a] = shmget(IPC_PRIVATE, sizeof(int), PERMS);
        bank[a] = (int *)shmat(shmid[a], NULL, 0);
        *bank[a] = INIT_BAL;
    }
    SEMAPHORE sem(1);
    sem.V(MUTEX);
    
    int childProcess;
    
    int p;
    for(p = 0; p < NUM_PROC; p++) {
        
        childProcess = fork();
        
        if(childProcess == 0) {
            
            for(int r = 0; r < REQS; r++) {

                int request = randomRequest();
                
                if (r % MILESTONE == 0) {
                	printf("PID (%d) requests a %s\n", getpid(), requests[request - 1]);
                }
                
                switch(request) {
                    case 1: {
                        int account = randomAccount();
                        sem.P(MUTEX);
                        deposit(bank[account]);
                        sem.V(MUTEX);
                        break;
                    }
                    case 2: {
                        int account = randomAccount();
                        sem.P(MUTEX);
                        withdraw(bank[account]);
                        sem.V(MUTEX);
                        break;
                    }
                    case 3: {
                        int account_from = randomAccount();
                        int account_to = randomAccount();
                        
                        while(account_to == account_from) {
                            account_from = randomAccount();
                        }
                        sem.P(MUTEX);
                        transfer(bank[account_from] , bank[account_to]);
                        sem.V(MUTEX);
                        break;
                    }
                    case 4: {
                        sem.P(MUTEX);
                        transferToChecking(bank[SAV_ACCT] , bank[VAC_ACCT] ,bank[CHK_ACCT]);
                        sem.V(MUTEX);
                        break;
                    }
                    case 5: {
                        int account_w = randomAccount();
                        
                        while(account_w == IRA_ACCT) {
                            account_w = randomAccount();
                        }
                        sem.P(MUTEX);
                        depositIRA(bank[IRA_ACCT] , bank[account_w]);
                        sem.V(MUTEX);
                        break;
                    }
                    default: {
                        break;
						printf("ERROR - Default on request = %d\n", request);
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
        --p;
    }
    
    cout << "\nTransactions Finished\n" << endl;
   	
    // output and clean up
    for (int n = 0; n < NUM_ACCTS; n++){
        printf("$%d in %s account\n", *bank[n], names[n]);
        shmctl(shmid[n], IPC_RMID, NULL);
    }
    
    sem.remove();
    
    exit(0);
}


int randomAccount() {
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


void transferToChecking( int *savings , int *vacation , int *checking) {
    
    if (canDecrement(*savings) && canDecrement(*vacation)) {
        (*savings)--;
        (*vacation)--;
        (*checking)++;
    } else {
        alert();
    }
}


void depositIRA(int *IRA , int *account_from) {

    if (canDecrement(*account_from)) {    	
		(*account_from)--;
		(*IRA)++;
	} else {
		alert();
	}
}


void alert() {
    cout << "Transfer Denied" << endl;
}


bool canDecrement(int val) {
    return (--val >= 0);
}
