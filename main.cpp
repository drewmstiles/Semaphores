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
static const int REQS = 2000;

// the number of bank accounts
static const int NUM_ACCTS = 4;

// the semaphore id
static const int AVAILABLE = 1;

// The index in shared memory buffers for Checking account
static const int CHECKING = 0;

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
 
static const int CHK_SEM = 1;
static const int SAV_SEM = 2;
static const int VAC_SEM = 3;
static const int IRA_SEM = 4;

static const int semID[4] = {CHK_SEM, SAV_SEM, VAC_SEM, IRA_SEM};
 
 
 /*
  * Account index within bank array
  */

static const int CHK_ACCT = 0;
static const int SAV_ACCT = 1;
static const int VAC_ACCT = 2;
static const int IRA_ACCT = 3;


// Summary:
//
//  Returns one of the five service requests.
//
// Returns:
//
//      an integer denoting the service to request
//
int getreq(default_random_engine, uniform_int_distribution<int>);


// Summary:
//
//  Initializes user bank accounts.
//
// Arguments:
//
//      array of integer pointers for referencing account balances
//
void createAccts(int**);

void deposit( int * account);



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
    
    // use four semaphores to restrict concurrent access to accounts
    SEMAPHORE sem(4);
	sem.V(CHK_SEM);
	sem.V(SAV_SEM);
	sem.V(VAC_SEM);
	sem.V(IRA_SEM);
	
    int childProcess;
    
    int y;
    for(y = 0; y < NUM_PROC; y++) {
    
        childProcess = fork();
        
        
        if(childProcess == 0){
        
        	/*
        	 * Execute with and without P and V operations to observe corruption
        	 */
           
            switch(1)
            {
                case 1: {
                    int account = rand() % 4;
                    sem.P(semID[3]);
                    deposit(bank[3]);
                    sem.V(semID[3]);
                    break;
                }
                    
                default: {
            
                
                    break;
                }
            }

        	
			exit(0);
        }
    }

    
    
    // waiting for all children to exit
	int status;
	pid_t pid;
	while (y > 0) {
	  pid = wait(&status);
	  printf("pid (%d) exits\n", pid);
	  --y; // decrement number of running children
	}
	
	// ensure that total equals number of processes
    int x;
    for (x = 0; x < 4; x++){
        printf("$%d in Checking account\n", *bank[x]);
    }
	// clean up
	for(int x = 0; x < NUM_ACCTS; x++){
       	shmctl(shmid[x], IPC_RMID, NULL);
    } 
    
	sem.remove();
	
    exit(0);
}


int getreq(default_random_engine generator, uniform_int_distribution<int> distribution) {

	// TODO This rarely generates 5 as the return type
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
			continue;
		} 
	}
}

void deposit(int * account){
    
    int bal = *account;
    usleep(ONE_MS);
    *account = (bal + 1);

}


// This code tests the distribution of random bank requests
//
// 			default_random_engine generator(random_device{}());
// 			uniform_int_distribution<int> distribution(0, pow(2.0, 32.0) - 1.0);
// 			
//         	for (int r = 0; r < REQS; r++) {
// 			   v.push_back(getreq(generator, distribution));
// 			}
// 			
// 			int sum = accumulate(v.begin(), v.end(), 0);
// 			printf("The average value is %d\n", sum / REQS); 
