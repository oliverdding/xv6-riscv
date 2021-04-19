#include "kernel/types.h"
#include "user/user.h"

void
prime(int left_fd)
{
    int base;

    if (read(left_fd, &base, sizeof(int)) == 0) // Read current prime
    {
        return;
    }
    fprintf(1, "prime %d\n", base);

    int fd[2];
    pipe(fd);

    if (fork()) // Parent
    {
        close(fd[0]);
        int tmp;

        while (read(left_fd, &tmp, sizeof(int)) != 0)
        {
            if (tmp % base != 0)
            {
                write(fd[1], &tmp, sizeof(int));
            }
        }

        close(fd[1]);
    }
    else // Child
    {
        close(fd[1]);
        prime(fd[0]);
        close(fd[0]);
        exit(0);
    }

    wait(0);
}

int
main(void)
{
    int fd[2];
    pipe(fd);

    if (fork()) // Parent
    {
        close(fd[0]); // Close read port

        for (int i = 2; i < 36; ++i)
        {
            write(fd[1], &i, sizeof(int));
        }

        close(fd[1]);
    }
    else // Child
    {
        close(fd[1]); // Close write port
        prime(fd[0]);
        close(fd[0]);
        exit(0);
    }

    wait(0);
    exit(0);
}

