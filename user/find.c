#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define BUF_SIZE 512

char buf[2 * BUF_SIZE];
int match(char *, char *);

void
usage(char *prog)
{
    fprintf(2, "usage: %s path pattern\n", prog);
}

void
lookfor(int fd, char *path, char *pattern)
{
    char path_buf[BUF_SIZE], *p;
    struct dirent de;
    struct stat st;

    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, ".."))
        {
            continue;
        }

        strcpy(path_buf, path);
        p = path_buf + strlen(path_buf);
        *p++ = '/';
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        if (stat(path_buf, &st) < 0)
        {
            fprintf(2, "ls: cannot stat %s\n", path_buf);
            continue;
        }

        switch (st.type)
        {
            case T_FILE:
            case T_DEVICE:
                if (match(pattern, de.name))
                {
                    fprintf(1, "%s\n", path_buf);
                }

                break;

            case T_DIR:
                {
                    int sub_fd;

                    if ((sub_fd = open(path_buf, 0)) < 0)
                    {
                        fprintf(2, "find: cannot open %s\n", path_buf);
                    }

                    lookfor(sub_fd, path_buf, pattern);
                    close(sub_fd);
                    break;
                }
        }
    }
}

int
main(int argc, char *argv[])
{
    if (argc != 3)
    {
        usage(argv[0]);
        exit(1);
    }

    int fd;
    struct stat st;

    if ((fd = open(argv[1], 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", argv[1]);
        exit(1);
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", argv[1]);
        close(fd);
        exit(1);
    }

    switch (st.type)
    {
        case T_FILE:
        case T_DEVICE:
            fprintf(2, "find: can only find in directory\n");
            close(fd);
            exit(1);

        case T_DIR:
            if (strlen(argv[1]) + 1 + DIRSIZ + 1 > BUF_SIZE)
            {
                fprintf(2, "ls: path too long\n");
                close(fd);
                exit(1);
            }

            lookfor(fd, argv[1], argv[2]);
            break;
    }

    close(fd);
    exit(0);
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9.

int matchhere(char *, char *);
int matchstar(int, char *, char *);

int
match(char *re, char *text)
{
    if (re[0] == '^')
    {
        return matchhere(re + 1, text);
    }

    do   // must look at empty string
    {
        if (matchhere(re, text))
        {
            return 1;
        }
    }
    while (*text++ != '\0');

    return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
    if (re[0] == '\0')
    {
        return 1;
    }

    if (re[1] == '*')
    {
        return matchstar(re[0], re + 2, text);
    }

    if (re[0] == '$' && re[1] == '\0')
    {
        return *text == '\0';
    }

    if (*text != '\0' && (re[0] == '.' || re[0] == *text))
    {
        return matchhere(re + 1, text + 1);
    }

    return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
    do   // a * matches zero or more instances
    {
        if (matchhere(re, text))
        {
            return 1;
        }
    }
    while (*text != '\0' && (*text++ == c || c == '.'));

    return 0;
}

