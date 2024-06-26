#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>


int main(int argc, char *argv[]) {
    int exit_status;
    pid_t pid = fork();
    pid_t cpid;
    if(pid == 0){
        printf("Child process has been created successfully\n");
        cpid = getpid();
        execlp("cat", "cat", "a.txt", "b.txt", NULL);
    } else if(pid == -1) perror("fork err");
    else printf("wtf parent\n");
    pid_t cwpid = waitpid(cpid, &exit_status, WNOHANG);
    if(WIFEXITED(exit_status)) printf("\nDefault exit , status is:%d", WEXITSTATUS(exit_status));
    if(WIFSIGNALED(exit_status)) printf("\nKilled by signal=%d%s", WTERMSIG(exit_status), WCOREDUMP(exit_status) ? " (dumped core)" : "");
    if(WIFSTOPPED(exit_status)) printf("\nStopped by signal=%d", WSTOPSIG(exit_status));
    if(WIFCONTINUED(exit_status)) printf ("\nContinued");
    printf("\n");
    return 0;
}
