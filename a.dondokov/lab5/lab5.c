#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
    off_t offset;
    off_t length;
} Line;

typedef struct {
    Line *array;
    int count;
    int capacity;
} Array;

void init(Array *arr) {
    arr->array = malloc(sizeof(Line));
    arr->count = 0;
    arr->capacity = 1;
}

void insert(Array *arr, Line element) {
    if (arr->count == arr->capacity) {
        arr->capacity *= 2;
        arr->array = realloc(arr->array, arr->capacity * sizeof(Line));
    }

    arr->array[arr->count++] = element;
}

void freeArray(Array *arr) {
    free(arr->array);
    arr->array = NULL;
    arr->count = arr->capacity = 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) { return 1; }
    char *path = argv[1];

    Array arr;
    init(&arr);

    int fd = open(path, O_RDONLY);
    if (fd == -1) { return 1; }
    lseek(fd, 0L, SEEK_CUR);

    char c;
    off_t lineOffset = 0;
    off_t lineLength = 0;
    while (read(fd, &c, 1) == 1) {
        if (c == '\n') {
            Line current = {lineOffset, lineLength};
            insert(&arr, current);

            lineOffset += lineLength + 1;
            lineLength = 0;
        } else {
            lineLength++;
        }
    }

    if (lineLength > 0) {
        Line current = {lineOffset, lineLength};
        insert(&arr, current);
    }

    while (1) {
        int num;
        printf("line number: ");
        scanf("%d", &num);

        if (num == 0) { break; }
        if (arr.count < num) {
            printf("only %d lines\n", arr.count);
            continue;
        }
        Line line = arr.array[num - 1];
        char *buf = calloc(line.length + 1, sizeof(char));
        lseek(fd, line.offset, SEEK_SET);
        read(fd, buf, line.length * sizeof(char));
        printf("%s\n", buf);
        free(buf);
    }
    close(fd);
    freeArray(&arr);
    return 0;
}