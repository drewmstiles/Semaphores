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

// number of concurrent processes
static const int NUM_PROC = 6;

// the number of requests by each process
static const int REQS = 1;
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
int getreq(default_random_engine, uniform_int_distribution<int>);


int main(int argc, const char * argv[]) {
    
    vector<int> v;
    
    int childProcess;
    
    
    int y;
    for(y = 0; y < NUM_PROC; y++) {
    
        childProcess = fork();
        
        if(childProcess == 0){
        
			default_random_engine generator(random_device{}());
			uniform_int_distribution<int> distribution(0, pow(2.0, 32.0) - 1.0);
			
        	for (int r = 0; r < REQS; r++) {
			   v.push_back(getreq(generator, distribution));
			}
			
			int sum = accumulate(v.begin(), v.end(), 0);
			printf("The average value is %d\n", sum / REQS); 
			
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
	
    exit(0);
}

// TEST
int getreq(default_random_engine generator, uniform_int_distribution<int> distribution) {

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



//  int sem1 = 1;
//  SEMAPHORE sem(1);
// 	sem.P(sem1);
// 	int temp
// 	temp = *shmBUF[0];
// 	temp = rand() % 10 + y;
// 	
// 	*shmBUF[0] = temp;
// 	
// 	cout << *shmBUF[0]<< endl;
// 	sem.V(sem1);
    
    
//     int shmid[4];
//     int *shmBUF[4];
//     int childProcess;
//     
//     
//     //set up the four seperate integers for the four seperate bank accounts
//     for(int x = 0; x < 4; x++){
//         shmid[x] = shmget(IPC_PRIVATE, sizeof(int), PERMS);
//         shmBUF[x] = (int *)shmat(shmid[x], 0, SHM_RND);
//     }
