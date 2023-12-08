#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
//#define BUF_SIZE 1024
int buf_len = 2;

int main (int argc, char *argv[]){
    //get cur dir
    takecwd:
    char *cwd = (char*) calloc(buf_len, 1);
    if(!getcwd(cwd, buf_len)){
        if(errno == ERANGE){
            perror("size is too less to write cwd, increasing and trying again\n");
            buf_len *= 2;
            free(cwd);
            goto takecwd;
        } else{
            perror("getcwd");
            return -1;
        }
    }
    printf("cwd is:%s", cwd);

//    struct stat file_stat;
//    struct dirent *entry;
//    void *dir;
//
//    dir = opendir(".");
//    if(!dir){
//        perror("Can't open cur dir");
//        return 1;
//    }
//    while((entry = readdir(dir))){
//        if(stat(entry->d_name, &file_stat) == -1){
//            perror("stat");
//            closedir(dir);
//            return 2;
//        }
//        printf("%s\n", entry->d_name);
//    }
//    closedir(dir);
    return 0;
}
