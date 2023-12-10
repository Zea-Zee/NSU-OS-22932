#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

int main() {
    //create program channel
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        exit(2);
    }

    if (pid == 0) {
        //close write df
        close(pipe_fd[1]);
        char buffer[100];
        ssize_t bytes_read;
        //read from channel
        while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
            for (ssize_t i = 0; i < bytes_read; ++i) {
                buffer[i] = toupper(buffer[i]);
            }
            write(STDOUT_FILENO, buffer, bytes_read);
        }

        // Закрываем дескриптор чтения
        close(pipe_fd[0]);
        printf("Child process finished.\n");
    } else {
        // Родительский процесс (писатель)

        // Закрываем дескриптор чтения
        close(pipe_fd[0]);

        const char* message = "Hello, World!";

        // Запись в канал
        write(pipe_fd[1], message, strlen(message));

        // Закрываем дескриптор записи
        close(pipe_fd[1]);

        // Ожидание завершения дочернего процесса
        wait(NULL);
        printf("Parent process finished.\n");
    }

    return 0;
}
