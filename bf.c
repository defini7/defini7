// gcc main.c -o bf -Wpedantic -Wall -std=c11 -Ofast

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* read_file(const char* filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
    {
        fprintf(stderr, "%s: %s\n", filename, strerror(errno));
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* res = malloc(fsize + 1);
    fread(res, fsize, 1, f);
    fclose(f);

    res[fsize] = 0;
    return res;
}

void parse(const char* source)
{
    int stack[30000];
    memset(stack, 0, sizeof(stack));

    int cur = 0;
    int b = 0;

    while (*source)
    {
        switch (*source)
        {
            case '>': cur++; break;
            case '<': cur--; break;
            case '+': stack[cur]++; break;
            case '-': stack[cur]--; break;
            case '.': putchar(stack[cur]); break;
            case ',': stack[cur] = getchar(); break;
            
            case '[':
            {
                if (!stack[cur])
                {
                    b++;
                    while (b)
                    {
                        switch (*(++source))
                        {
                            case '[': b++; break;
                            case ']': b--; break;
                        }
                    }
                }
            }
            break;

            case ']':
            {
                if (stack[cur])
                {
                    b++;
                    while (b)
                    {
                        switch (*(--source))
                        {
                            case '[': b--; break;
                            case ']': b++; break;
                        }
                    }
                    source--;
                }
            }
            break;

        }

        source++;
    }
}

void run_file(const char* filename)
{
    parse(read_file(filename));
}

void run_line(const char* input)
{
    parse(input);
}

void run_shell()
{
    while (1)
    {
        printf(">>> ");
        char input[1024];
        scanf("%s", input);
        run_line(input);
    }
}

int main(int argc, const char** argv)
{
    if (argc > 1)
        run_file(argv[1]);
    else
        run_shell();

    return 0;
}
