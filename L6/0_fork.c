/*
fork.c
- This program demonstrates process creation using the `fork()` system call.
- The parent process waits for the child process to complete before printing a final message.
*/

#include <stdio.h>      // Standard I/O library
#include <stdlib.h>     // Standard library functions
#include <sys/types.h>  // Data types used in system calls
#include <sys/wait.h>   // Wait for process termination
#include <unistd.h>     // POSIX API for UNIX system calls

int main(void) {
    // Create a new process using fork()
    pid_t pid = fork();

    // Check if fork() was successful
    if (pid == 0) {
        // This block executes in the child process
        printf("Child => PPID: %d PID: %d\n", getppid(), getpid());
        exit(EXIT_SUCCESS);  // Exit the child process
    }
    else if (pid > 0) {
        // This block executes in the parent process
        printf("Parent => PID: %d\n", getpid());
        printf("Waiting for child process to finish.\n");
        
        // Wait for the child process to complete
        wait(NULL);
        printf("Child process finished.\n");
    }
    else {
        // If fork() returns a negative value, it means process creation failed
        printf("Unable to create child process.\n");
    }

    return 0;  // Exit the main process
}
/*
Parent => PID: 229
Waiting for child process to finish.
Child => PPID: 229 PID: 230
Child process finished.
*/
