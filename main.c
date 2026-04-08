#include<stdio.h>
#include<unistd.h>
#include <fcntl.h>
#include<string.h>
#include<errno.h>
#include<sys/wait.h>

int main(int argc, char** argv){
    if(argc < 2){
        fprintf(stderr, "Missing target program. Format: ./program \"target program\"\n");
        return 0;
    }
    
    int fd[2];
    int p = pipe(fd);
    int out_fd = open("file.out", O_WRONLY | O_CREAT | O_APPEND);

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int cpid = fork();
    setvbuf(stdout, NULL, _IOLBF, 0);

    if(cpid == 0) {
        close(fd[0]);
        int status = dup2(fd[1], STDOUT_FILENO);
        if(status == -1) {
            fprintf(stderr, "Error: %s, exiting\n", strerror(errno));
            return 0;
        }
        close(fd[1]);
        execlp("sh", "sh", "-c", argv[1], NULL);
    } else {
        close(fd[1]);
        while(read(fd[0], buffer, sizeof(buffer)) > 0){
            write(STDOUT_FILENO, buffer, strlen(buffer));
            write(out_fd, buffer, strlen(buffer));
            memset(buffer, 0, sizeof(buffer));
        }
        close(fd[0]);
        wait(NULL);
    }

    return 0;
}