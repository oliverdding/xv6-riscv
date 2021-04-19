#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
    int c2p[2];
    int p2c[2];
    pipe(c2p);
    pipe(p2c);
    uint32 buf;

    if (fork()) // Parent
    {
        close(c2p[1]);
        close(p2c[0]);
        write(p2c[1], "ping", 4);
        int i = read(c2p[0], ((uint8 *)&buf), 4);

        if (i == 4 && ((uint8 *)&buf)[0] == 'p' && ((uint8 *)&buf)[1] == 'o' && ((uint8 *)&buf)[2] == 'n' && ((uint8 *)&buf)[3] == 'g')
        {
            fprintf(1, "%d: received pong\n", getpid());
        }

        close(c2p[0]);
        close(p2c[1]);
        wait(0);
    }
    else // Child
    {
        close(c2p[0]);
        close(p2c[1]);
        int i = read(p2c[0], ((uint8 *)&buf), 4);

        if (i == 4 && ((uint8 *)&buf)[0] == 'p' && ((uint8 *)&buf)[1] == 'i' && ((uint8 *)&buf)[2] == 'n' && ((uint8 *)&buf)[3] == 'g')
        {
            fprintf(1, "%d: received ping\n", getpid());
            write(c2p[1], "pong", 4);
        }

        close(c2p[1]);
        close(p2c[0]);
    }

    exit(0);
}