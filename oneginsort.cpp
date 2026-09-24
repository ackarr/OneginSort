#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "BadCode.h"
#include "Bubble.h"


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
void   SortMyLines(Text *t);
void   OrigSort   (Text *t);
int    OrigCmpStr (const void* a, const void* b);



void   PrintLines (const Text *t, FILE *out);

void   TextFree   (Text *t);



int main(int argc, char **argv)
{
    Text t = {};

    TextLoad(argv[1], &t);

    printf("%s\n", "Вывожу отсортированный");
    SortLines(&t);
    PrintLines(&t, stdout);

    printf("%s\n", "Вывожу бабл");
    SortMyLines(&t);
    PrintLines(&t, stdout);


    printf("%s\n", "Вывожу исходный");
    OrigSort(&t);
    PrintLines(&t, stdout);

    TextFree(&t);

    return 0;
}


int TextLoad(const char *file_name, Text *out)
{
    char* buf = NULL;
    size_t size = 0;
    ReadFile(file_name, &buf, &size);


    char** lines = NULL;

    size_t count_lines = ProcessingLines(buf, size, &lines);

    if (!lines)
    {
        free(buf);
        return -1;
    }



    out->buf = buf;
    out->lines = lines;
    out->line_count = count_lines;

    return 0;
}

int ReadFile(const char* file_name, char** out_buf, size_t* out_size)
{
    size_t file_size = FileSize(file_name);

    int fd = open(file_name, O_RDONLY);

    char* buf = (char*) malloc(file_size + 1);

    if(!buf)
    {
        close(fd);
        return -1;
    }

    ssize_t n = read(fd, buf, file_size);

    close(fd);

    buf[file_size] = '\0';

    *out_buf = buf;
    *out_size = file_size;

    return 0;
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

    size_t count = 0;

    size_t start = 0;

    for(size_t i = 0; i < file_size; i++)
    {
        if(buf[i] == '\n')
        {
            buf[i] = '\0';

            char** temp = (char**) realloc(lines, (count + 1) * sizeof(*lines));
            if(!temp)
            {
                free(lines);
                *lines_out = NULL;
                return 0;
            }
            lines = temp;
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
