#include "kernel/types.h"
#include "user/user.h"

#define BUF_SIZE 16

int
main(void)
{
    int c2p[2];
    int p2c[2];
    pipe(c2p);
    pipe(p2c);
    uint8 buf[BUF_SIZE];
    int pid = fork();

    if (pid == 0)
    {
        close(c2p[0]);
        close(p2c[1]);
        int i = read(p2c[0], buf, BUF_SIZE);

        if (i > 0)
        {
            fprintf(1, "%d: received ping\n", getpid());
            write(c2p[1], "hello!", 6);
        }

        close(c2p[1]);
        close(p2c[0]);
    }
    else
    {
        close(c2p[1]);
        close(p2c[0]);
        write(p2c[1], "hello!", 6);
        int i = read(c2p[0], buf, BUF_SIZE);

        if (i > 0)
        {
            fprintf(1, "%d: received pong\n", getpid());
        }

        close(c2p[0]);
        close(p2c[1]);
        wait(&pid);
    }

    exit(0);
}