//5. ������� ������ ����� � ��������� �����.
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

typedef struct {  // �������� ���������� �� �������� � ����� ������ � �����
    off_t offset;  // �������� ������
    off_t length; // ����� ������
} Line;

typedef struct {  // ������������ ��� �������� ������� ��������� ���� `Line`
    Line* array;  // ������ ��������� Line
    int cnt;  // ���������� ��������� � �������
    int capacity; // ������� �������
} Array;

void freeArray(Array* a) {  // ����������� ������, ���������� ��� ������� Line � ��������� Array
    free(a->array);
    a->array = NULL;
    a->cnt = 0, a->capacity = 0;
}

void insertArray(Array* a, Line element) {  // ��������� ����� ������� Line � ������ Array
    if (a->cnt == a->capacity) {  // ���� ������ ��������, ��� ������� �����������
        a->capacity = a->capacity * 2;
        a->array = realloc(a->array, a->capacity * sizeof(Line));
    }
    a->array[a->cnt++] = element;
}


void initArray(Array* a) {  // ��������� ������ ��� ������� Line
    a->array = malloc(sizeof(Line));
    a->cnt = 0;
    a->capacity = 1;
}

int main(int argc, char* argv[]) {
    if (argc != 2)
        return 1;
    char* path = argv[1];

    int file = open(path, O_RDONLY);
    if (file == -1)
        return 1;
    lseek(file, 0L, SEEK_CUR);

    Array table;  // ������� table ���� Array
    initArray(&table);  // ��������� ������ ��� ���� array

    char c;
    off_t line_offset = 0, line_len = 0;
    while (read(file, &c, 1) == 1) {
        if (c == '\n') {  // ���� ���� ������� ������,
            Line current = { line_offset, line_len };
            insertArray(&table, current);  // �� ������ ������������ �

            line_offset += line_len + 1;
            line_len = 0;
        }
        else
            line_len++;
    }

    if (line_len > 0) {
        Line current;
        current.offset = line_offset, current.length = line_len;
        insertArray(&table, current);  // ��������� ������ � ������
    }

    while (1) {
        int number;
        printf("Enter number of line: ");
        scanf("%d", &number);

        if (number == 0)  // ���� ����� ������ ����� ����, �� ��������� �����������
            break;
        if (number > table.cnt) {  // ���� ��������� ����� ������ ��������� �����, �� ������ �����������
            printf("Total lines = %d\n", table.cnt);
            continue;
        }

        Line line = table.array[number - 1];  // ��������� ��� ���� ������, � ������� ������������ ��������� ������
        char* buffer = calloc(line.length + 1, sizeof(char));

        lseek(file, line.offset, SEEK_SET);  // ���������� ��������� �� �������� ��������
        read(file, buffer, line.length * sizeof(char));  // ���������� ������ ����������� � �����

        printf("%s\n", buffer);
        free(buffer);
    }

    close(file);
    freeArray(&table);

    return 0;
}