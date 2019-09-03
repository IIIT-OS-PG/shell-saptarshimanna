#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	int pipefd[2];
	char buf;
	pipe(pipefd);
    int pid = fork();
    if (pid==0) {
        int err;
        char *env[1] = { 0 };
        char *argv[3] = { "ls", "-l", 0 };
        //close(pipefd[1]);
        err = execve("/bin/ls", argv, env);  //syscall, libc has simpler wrappers (man exec)
        close(pipefd[1]);
        while (read(pipefd[0], &buf, 1) > 0)
            write(STDOUT_FILENO, &buf, 1);
        write(STDOUT_FILENO, "\n", 1);
        close(pipefd[0]);
        _exit(EXIT_SUCCESS);
        //exit(err); //if it got here, it's an error
    } else if(pid<0) {
        printf("fork failed with error code %d\n", pid);
        exit(-1);
    }

    /*int status;
    wait(&status); //simplest one, man wait for others
    printf("child pid was %d, it exited with %d\n", pid, status);
    exit(0);*/
    close(pipefd[0]);          /* Close unused read end */
        write(pipefd[1], argv[1], strlen(argv[1]));
        close(pipefd[1]);          /* Reader will see EOF */
        wait(NULL);                /* Wait for child */
        exit(EXIT_SUCCESS);
    return 0;
}

