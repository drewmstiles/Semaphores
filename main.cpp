//
//  main.cpp
//  semaphore_project
//
//  Created by Tyler Cobb on 11/28/15.
//  Copyright (c) 2015 Tyler Cobb. All rights reserved.
//


#include <iostream>
#include <iostream>
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
#include "semaphore.h"

using namespace std;

/*
 * Five service requests for user processes
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

// Summary:
//
//  Returns one of the five service requests.
//
// Returns:
//
//      an integer denoting the service to request
//
int getreq() {


int main(int argc, const char * argv[]) {
   
    int shmid[4];
    int *shmBUF[4];
    int childProcess;
    int sem1 = 1;
    
    SEMAPHORE sem(1);
    
    //set up the four seperate integers for the four seperate bank accounts
    for(int x; x < 4; x++){
        shmid[x] = shmget(IPC_PRIVATE, sizeof(int), PERMS);
        shmBUF[x] = (int *)shmat(shmid[x], 0, SHM_RND);
  
    }
    
    //create new process, this is were i left off, I was trying to see how the
    //semaphoew worked when running multiple processes.
    for(int y; y < 5; y++){
        childProcess = fork();
        
        if(childProcess == 0){
            sem.P(sem1);
            int temp
            temp = *shmBUF[0];
            temp = rand() % 10 + y;
            
            *shmBUF[0] = temp;
            
            cout << *shmBUF[0]<< endl;
            sem.V(sem1);
        }
    }    
}

// TEST
int getreq() {
	while (true) {
		default_random_engine generator(random_device{}());
		uniform_int_distribution<int> distribution(0, pow(2.0, 32.0) - 1.0);
		int random = distribution(generator);
		if (random % ADD == 0) {
			return ADD;
		} else if (random % REM == 0) {
			return REM;
		} else if (random % TRAN == 0) {
			return (TRAN;
		} else if (random % VSTRAN == 0) {
			return VSTRAN;
		} else if (random % IADD == 0) {
			return IADD;
		} else {
			continue;
		} 
	}


