#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void print_file_info(const char *filename);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        print_file_info(argv[i]);
    }

    return 0;
}

void print_file_info(const char *filename) {
    struct stat file_stat;

    if (stat(filename, &file_stat) == -1) {
        perror("stat");
        return;
    }

    printf("%c", S_ISDIR(file_stat.st_mode) ? 'd' : (S_ISREG(file_stat.st_mode) ? '-' : '?'));

    printf("%c%c%c%c%c%c%c%c%c",
           file_stat.st_mode & S_IRUSR ? 'r' : '-',
           file_stat.st_mode & S_IWUSR ? 'w' : '-',
           file_stat.st_mode & S_IXUSR ? 'x' : '-',
           file_stat.st_mode & S_IRGRP ? 'r' : '-',
           file_stat.st_mode & S_IWGRP ? 'w' : '-',
           file_stat.st_mode & S_IXGRP ? 'x' : '-',
           file_stat.st_mode & S_IROTH ? 'r' : '-',
           file_stat.st_mode & S_IWOTH ? 'w' : '-',
           file_stat.st_mode & S_IXOTH ? 'x' : '-');

    printf(" %2lu", (unsigned long)file_stat.st_nlink);

    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group *gr = getgrgid(file_stat.st_gid);
    printf(" %-8s %-8s", pw ? pw->pw_name : "unknown", gr ? gr->gr_name : "unknown");

    if (S_ISREG(file_stat.st_mode)) {
        printf(" %8ld", (long)file_stat.st_size);
    } else {
        printf("          ");
    }

    struct tm *tm_info = localtime(&file_stat.st_mtime);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%b %d %H:%M", tm_info);
    printf(" %s", time_str);

    printf(" %s\n", filename);
}
