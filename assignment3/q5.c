#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static void *xmalloc(size_t s)
{
    void *p = malloc(s);
    if (!p)
    {
        fprintf(stderr, "FATAL: malloc(%zu) failed: %s\n", s, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return p;
}

static void *xrealloc(void *ptr, size_t s)
{
    void *p = realloc(ptr, s);
    if (!p)
    {
        fprintf(stderr, "FATAL: realloc(%zu) failed: %s\n", s, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return p;
}

char *readLine(FILE *in)
{
    size_t bufcap = 128;
    size_t len = 0;
    char *buf = xmalloc(bufcap);

    while (1)
    {
        int c = fgetc(in);
        if (c == EOF)
        {
            if (len == 0)
            {
                free(buf);
                return NULL;
            }
            break;
        }
        if (c == '\n')
            break;

        if (len + 1 >= bufcap)
        {
            bufcap *= 2;
            buf = xrealloc(buf, bufcap);
        }
        buf[len++] = (char)c;
    }

    buf = xrealloc(buf, len + 1);
    buf[len] = '\0';
    return buf;
}

void ensureCapacity(char ***linesPtr, int *capacity, int needed)
{
    if (*capacity >= needed)
        return;
    int newcap = (*capacity == 0) ? 4 : *capacity;
    while (newcap < needed)
        newcap *= 2;
    *linesPtr = (char **)xrealloc(*linesPtr, sizeof(char *) * newcap);
    for (int i = *capacity; i < newcap; ++i)
        (*linesPtr)[i] = NULL;
    *capacity = newcap;
}

int insertLine(char ***linesPtr, int *count, int *capacity, int index, const char *text)
{
    if (index < 0 || index > *count)
        return -1;

    ensureCapacity(linesPtr, capacity, *count + 1);
    char **lines = *linesPtr;

    if (index < *count)
    {
        memmove(&lines[index + 1], &lines[index], sizeof(char *) * (*count - index));
    }

    size_t L = strlen(text);
    char *copy = xmalloc(L + 1);
    memcpy(copy, text, L + 1);

    lines[index] = copy;
    (*count)++;
    return 0;
}

int deleteLine(char **lines, int *count, int index)
{
    if (index < 0 || index >= *count)
        return -1;
    free(lines[index]);
    if (index < (*count) - 1)
    {
        memmove(&lines[index], &lines[index + 1], sizeof(char *) * ((*count) - index - 1));
    }
    lines[*count - 1] = NULL;
    (*count)--;
    return 0;
}

void printAllLines(char **lines, int count)
{
    printf("\n--- Buffer (total %d lines) ---\n", count);
    for (int i = 0; i < count; ++i)
    {
        printf("%4d: %s\n", i + 1, lines[i]);
    }
    printf("--- end ---\n");
}

void freeAll(char **lines, int count, char ***linesPtr)
{
    if (lines)
    {
        for (int i = 0; i < count; ++i)
            free(lines[i]);
        free(lines);
    }
    if (linesPtr)
        *linesPtr = NULL;
}

void shrinkToFit(char ***linesPtr, int *capacity, int count)
{
    if (*capacity == count)
        return;
    if (count == 0)
    {
        free(*linesPtr);
        *linesPtr = NULL;
        *capacity = 0;
        return;
    }
    *linesPtr = (char **)xrealloc(*linesPtr, sizeof(char *) * count);
    *capacity = count;
}

int saveToFile(const char *filename, char **lines, int count)
{
    FILE *f = fopen(filename, "w");
    if (!f)
        return -1;
    for (int i = 0; i < count; ++i)
    {
        if (fprintf(f, "%s\n", lines[i]) < 0)
        {
            fclose(f);
            return -1;
        }
    }
    fclose(f);
    return 0;
}

int loadFromFile(char ***linesPtr, int *countPtr, int *capacity, const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return -1;

    if (*linesPtr)
    {
        for (int i = 0; i < *countPtr; ++i)
            free((*linesPtr)[i]);
        free(*linesPtr);
    }
    *linesPtr = NULL;
    *capacity = 0;
    *countPtr = 0;

    char *ln;
    while ((ln = readLine(f)) != NULL)
    {
        ensureCapacity(linesPtr, capacity, (*countPtr) + 1);
        (*linesPtr)[(*countPtr)++] = ln;
    }

    fclose(f);
    return *countPtr;
}

void printMenu(void)
{
    printf("\nCommands:\n");
    printf("  i <index>\n");
    printf("  a\n");
    printf("  d <index>\n");
    printf("  p\n");
    printf("  s <file>\n");
    printf("  l <file>\n");
    printf("  r\n");
    printf("  q\n");
}

int main(void)
{
    char **lines = NULL;
    int capacity = 0;
    int count = 0;

    printf("Minimal Line-Based Text Editor\n");
    printMenu();

    char cmd[8];
    while (1)
    {
        printf("\n> ");
        if (scanf("%7s", cmd) == EOF)
            break;

        if (strcmp(cmd, "i") == 0)
        {
            int idx1;
            if (scanf("%d", &idx1) != 1)
                continue;
            int index0 = idx1 - 1;
            int ch = getchar();
            if (ch != '\n' && ch != EOF)
                ungetc(ch, stdin);
            printf("Enter line: ");
            char *txt = readLine(stdin);
            if (!txt)
                txt = xmalloc(1), txt[0] = '\0';
            insertLine(&lines, &count, &capacity, index0, txt);
            free(txt);
        }
        else if (strcmp(cmd, "a") == 0)
        {
            int ch = getchar();
            if (ch != '\n' && ch != EOF)
                ungetc(ch, stdin);
            printf("Enter line: ");
            char *txt = readLine(stdin);
            if (!txt)
                txt = xmalloc(1), txt[0] = '\0';
            insertLine(&lines, &count, &capacity, count, txt);
            free(txt);
        }
        else if (strcmp(cmd, "d") == 0)
        {
            int idx1;
            if (scanf("%d", &idx1) != 1)
                continue;
            deleteLine(lines, &count, idx1 - 1);
        }
        else if (strcmp(cmd, "p") == 0)
        {
            printAllLines(lines, count);
        }
        else if (strcmp(cmd, "s") == 0)
        {
            char fname[256];
            if (scanf("%255s", fname) != 1)
                continue;
            saveToFile(fname, lines, count);
        }
        else if (strcmp(cmd, "l") == 0)
        {
            char fname[256];
            if (scanf("%255s", fname) != 1)
                continue;
            loadFromFile(&lines, &count, &capacity, fname);
        }
        else if (strcmp(cmd, "r") == 0)
        {
            shrinkToFit(&lines, &capacity, count);
        }
        else if (strcmp(cmd, "q") == 0)
        {
            break;
        }
        else
        {
            printMenu();
        }
    }

    freeAll(lines, count, &lines);
    return 0;
}
