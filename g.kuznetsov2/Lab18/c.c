#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

// Structure to store directory information
struct DirInfo {
    char name[256];
    struct stat st;
};

// Comparison function for scandir (sort directories by time)
int compareDirsByTime(const struct DirInfo **a, const struct DirInfo **b) {
    return (*b)->st.st_mtime - (*a)->st.st_mtime;
}

int main(int argc, char *argv[]) {
    // Parse command-line arguments
    int opt;
    int l_flag = 0, t_flag = 0, a_flag = 0;

    while ((opt = getopt(argc, argv, "lta")) != -1) {
        switch (opt) {
            case 'l':
                l_flag = 1;
                break;
            case 't':
                t_flag = 1;
                break;
            case 'a':
                a_flag = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l] [-t] [-a]\n", argv[0]);
                return EXIT_FAILURE;
        }
    }

    // Read directory entries and sort directories by time
    struct DirInfo **dirs;
    int dirCount = scandir(".", &dirs, NULL, compareDirsByTime);

    if (dirCount == -1) {
        perror("scandir");
        return EXIT_FAILURE;
    }

    // Print directory information
    for (int i = 0; i < dirCount; i++) {
        if (!a_flag && dirs[i]->name[0] == '.') {
            free(dirs[i]);
            continue;
        }

        if (l_flag) {
            char timeStr[20];
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&dirs[i]->st.st_mtime));
            printf("%s\tLast modified: %s", dirs[i]->name, timeStr);
        } else {
            printf("%s\t", dirs[i]->name);
        }

        free(dirs[i]);
        printf("\n");
    }

    free(dirs);

    return EXIT_SUCCESS;
}
