#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>


struct Text{
            char* buf;
            char** lines;
            size_t line_count;};


size_t FileSize   (const char* file_name);
int    ReadFile   (const char* file_name, char** out_buf, size_t* out_size);
size_t CountLines (const char* buf, size_t file_size);
int    SplitLines (char* buf, size_t file_size, char** lines);
int    TextLoad   (const char *file_name, Text *out);
void   TextFree   (Text *t);
void   SortLines  (Text *t);
void   PrintLines (const Text *t, FILE *out);
int    CmpStr     (const void *a, const void *b);


int main(int argc, char **argv)
{
    Text t;

    TextLoad(argv[1], &t);

    SortLines(&t);
    PrintLines(&t, stdout);

    TextFree(&t);

    return 0;
}


size_t FileSize (const char* file_name)
{
    struct stat st;

    stat(file_name, &st);

    return (size_t) st.st_size;
}


int ReadFile(const char* file_name, char** out_buf, size_t* out_size)
{
    size_t file_size = FileSize (file_name);

    int fd = open(file_name, O_RDONLY);

    char* buf = (char*) malloc(file_size + 1);

    if(!buf)
    {
        close(fd);
        return -1;
    }

    size_t total = 0;

    while(total < file_size)
    {
        ssize_t n = read(fd, buf + total, file_size - total);

        if(n == 0) break;

        total += (size_t) n;
    }

    close(fd);

    buf[file_size] = '\0';

    *out_buf = buf;
    *out_size = file_size;

    return 0;
}


size_t CountLines(const char* buf, size_t file_size)
{
    size_t count = 0;

    for(size_t i = 0; i < file_size; i++)
    {
        if(buf[i] == '\n') count++;
    }

    if(buf[file_size - 1] != '\n') count++;

    return count;
}


int SplitLines(char* buf, size_t file_size, char** lines)
{
    size_t line_adr = 0;
    size_t start = 0;

    for(size_t i = 0; i < file_size; i++)
    {
        if(buf[i] == '\n')
        {
            buf[i] = '\0';
            lines[line_adr++] = &buf[start];
            start = i + 1;
        }
    }


    if(start < file_size)
    {
        lines[line_adr++] = &buf[start];
    }

    return 0;
}


int TextLoad(const char *file_name, Text *out)
{
    out->buf = NULL;
    out->lines = NULL;
    out->line_count = 0;

    char* buf = NULL;
    size_t size = 0;
    ReadFile(file_name, &buf, &size);

    size_t count_lines = CountLines(buf, size);

    char **lines = NULL;
    if (count_lines > 0)
    {
        lines = (char**) malloc(count_lines * sizeof(*lines));
        if (!lines)
        {
            free(buf);
            return -1;
        }

        SplitLines(buf, size, lines);
    }

    out->buf = buf;
    out->lines = lines;
    out->line_count = count_lines;

    return 0;
}


void TextFree(Text *t)
{
    free(t->lines);
    free(t->buf);
    t->lines = NULL;
    t->buf = NULL;
    t->line_count = 0;
}


void SortLines(Text *t)
{
    if (t->line_count > 1)
    {
        qsort(t->lines, t->line_count, sizeof(*t->lines), &CmpStr);
    }
}

int CmpStr(const void* a, const void* b)
{
    const char* s1 = *(const char* const *) a;
    const char* s2 = *(const char* const *) b;

    return strcmp(s1, s2);
}


void PrintLines(const Text *t, FILE *out)
{
    for (size_t i = 0; i < t->line_count; i++)
    {
        fputs(t->lines[i], out);
        fputc('\n', out);
    }
}
