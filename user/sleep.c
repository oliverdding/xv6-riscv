#include "kernel/types.h"
#include "user/user.h"

void usage(char *prog) {
    fprintf(2, "usage: %s SECONDS\n", prog);
}

int
main(int argc, char *argv[]) {
    if (argc !=2)
    {
        usage(argv[0]);
        exit(1);
    }
    int times = atoi(argv[1]);
    sleep(times);
    exit(0);
}