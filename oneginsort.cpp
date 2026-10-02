#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "BadCode.h"
#include "Bubble.h"


enum errs
{
    NORM_END = 0,
    INPUT_ERR,
    FILE_ERR,
    DATA_INIT_ERR,
    WRITE_ERR
};


struct Text{
            char* buf;
            char** lines;
            size_t line_count;};


int    TextLoad       (const char *file_name, Text *out);
int    ReadFile       (const char* file_name, char** out_buf, size_t* out_size);
size_t FileSize       (const char* file_name);
size_t ProcessingLines(char* buf, size_t file_size, char*** lines);

void   SortLines  (Text *t);
int    CmpStr     (const void *a, const void *b);
int    CmpStrEnd  (const void* a, const void* b);
char*  ReverseStr (const char* line);
void   SortMyLines(Text *t);
void   OrigSort   (Text *t);
int    OrigCmpStr (const void* a, const void* b);



void   PrintLines (const Text *t, FILE *out);

void   TextFree   (Text *t);



int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("%s, %s:%d", "ERROR: Введены не все данные", __FILE__, __LINE__);
        return INPUT_ERR;
    }


    FILE* out = fopen("sortout.txt", "w");
    if(!out)
    {
        printf("%s, %s:%d", "ERROR: Файл для вывода не открылся", __FILE__, __LINE__);
        return FILE_ERR;
    }

    Text t = {};

    if(TextLoad(argv[1], &t) != 0)
    {
        printf("%s, %s:%d", "ERROR: Файл не прочтен", __FILE__, __LINE__);
        return FILE_ERR;
    }

    fputs("Вывожу отсортированный\n", out);
    qsort(t.lines, t.line_count, sizeof(*t.lines), &CmpStr);
    PrintLines(&t, out);

    fputs("\n\n\n\n\n\n\n\n\n\n\n\nВывожу исходный\n", out);
    qsort(t.lines, t.line_count, sizeof(*t.lines), &OrigCmpStr);
    PrintLines(&t, out);

    fputs("\n\n\n\n\n\n\n\n\n\n\n\nВывожу бабл\n", out);
    Bubble(t.lines, t.line_count, sizeof(*t.lines), &CmpStr);
    PrintLines(&t, out);

    fputs("\n\n\n\n\n\n\n\n\n\n\n\nВывожу бабл по концу\n", out);
    Bubble(t.lines, t.line_count, sizeof(*t.lines), &CmpStrEnd);
    PrintLines(&t, out);

    TextFree(&t);

    return NORM_END;
}


int TextLoad(const char *file_name, Text *out)
{
    char* buf = NULL;
    size_t size = 0;
    if(ReadFile(file_name, &buf, &size) != 0) return 1;


    char** lines = NULL;

    size_t count_lines = ProcessingLines(buf, size, &lines);

    if (!lines)
    {
        free(buf);
        printf("%s, %s:%d", "ERROR: Адреса строк не записаны", __FILE__, __LINE__);
        return WRITE_ERR;
    }



    out->buf = buf;
    out->lines = lines;
    out->line_count = count_lines;

    return NORM_END;
}

int ReadFile(const char* file_name, char** out_buf, size_t* out_size)
{
    size_t file_size = FileSize(file_name);

    if(file_size == 0)
    {
        printf("%s, %s:%d", "ERROR: Пустой файл", __FILE__, __LINE__);
        return FILE_ERR;
    }

    int fd = open(file_name, O_RDONLY);

    if(fd == -1)
    {
        close(fd);
        printf("%s, %s:%d", "ERROR: Файл не был открыт", __FILE__, __LINE__);
        return FILE_ERR;
    }

    char* buf = (char*) malloc(file_size + 1);
    if(!buf)
    {
        close(fd);
        printf("%s, %s:%d", "ERROR: Память не выделилась", __FILE__, __LINE__);
        return DATA_INIT_ERR;
    }

    read(fd, buf, file_size);

    close(fd);

    buf[file_size] = '\0';

    *out_buf = buf;
    *out_size = file_size;

    return NORM_END;
}

size_t FileSize (const char* file_name)
{
    struct stat st;

    stat(file_name, &st);

    return (size_t) st.st_size;
}

size_t ProcessingLines(char* buf, size_t file_size, char*** lines_out)
{
    char** lines = (char**) malloc(sizeof(*lines));
    if(!lines)
    {
        printf("%s, %s:%d", "ERROR: Память не выделилась", __FILE__, __LINE__);
        return DATA_INIT_ERR;
    }

    size_t size_str = sizeof(*lines);
    size_t capacity = 1;

    size_t count = 0;

    size_t start = 0;

    for(size_t i = 0; i < file_size; i++)
    {
        if(buf[i] == '\n')
        {
            buf[i] = '\0';

            if((count + 1) > capacity)
            {
                capacity = capacity * 3/2 + 1;

                char** temp = (char**) realloc(lines, capacity * size_str);

                if(!temp)
                {
                    free(lines);
                    *lines_out = NULL;
                    return DATA_INIT_ERR;
                }
                lines = temp;
            }

            lines[count++] = &buf[start];
            start = i + 1;
        }
    }


    if(start < file_size)
    {
        char** temp = (char**) realloc(lines, (count + 1) * sizeof(*lines));
        if(!temp)
        {
            free(lines);
            *lines_out = 0;
            return 0;
        }
        lines = temp;
        lines[count++] = &buf[start];
    }

    *lines_out = lines;

    return count;

}


void SortLines(Text *t)
{
    if (t->line_count > 1)
    {
        qsort(t->lines, t->line_count, sizeof(*t->lines), &CmpStr);
    }
}

void   SortMyLines(Text *t)
{
    if (t->line_count > 1)
    {
        Bubble(t->lines, t->line_count, sizeof(*t->lines), &CmpStr);
    }
}

int CmpStr(const void* a, const void* b)
{
    const char* s1 = *(const char**) a;
    const char* s2 = *(const char**) b;

    return strcmp(s1, s2);
}

int CmpStrEnd(const void* a, const void* b)
{
    const char* s1 = *(const char**) a;
    const char* s2 = *(const char**) b;
    size_t l1 = strlen(s1);
    size_t l2 = strlen(s2);

    while (l1 > 0 && l2 > 0)
    {
        if (s1[l1 - 1] != s2[l2 - 1])
        {
            return (int)s1[l1 - 1] - (int)s2[l2 - 1];
        }

        l1--;
        l2--;
    }
    return (int)l1 - (int)l2;
}


void OrigSort(Text *t)
{
    if (t->line_count > 1)
    {
        qsort(t->lines, t->line_count, sizeof(*t->lines), &OrigCmpStr);
    }
}

int OrigCmpStr(const void* a, const void* b)
{
    const char* s1 = *(const char**) a;
    const char* s2 = *(const char**) b;

    if(s1 < s2) return -1;
    if(s1 > s2) return 1;
    return 0;
}


void PrintLines(const Text *t, FILE *out)
{
    for(size_t i = 0; i < t->line_count; i++)
    {
        fputs("<", out);
        fputs(t->lines[i], out);
        fputs(">", out);
        fputc('\n', out);
    }
}


void TextFree(Text *t)
{
    free(t->lines);
    free(t->buf);
    t->lines = NULL;
    t->buf = NULL;
    t->line_count = 0;
}
