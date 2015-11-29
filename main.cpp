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
