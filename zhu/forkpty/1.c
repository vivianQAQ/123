#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int handle_terminal(int *terminal, char **argv)
{
    int pid = forkpty(terminal, 0, 0, 0);

    if (-1 == pid)
        /* forkpty failed */
        exit_error("could not fork or more pseudo terminals available");

    /* are we parent ? */
    if (0 != pid)
        /* yes - return with child pid */
        return pid;

    /* remove cr/lf translation from terminal */
    struct termios settings;
    tcgetattr(STDOUT_FILENO, &settings);
    settings.c_oflag &= ~OPOST;
    settings.c_oflag &= ~ONLCR;
    tcsetattr(STDOUT_FILENO, TCSANOW, &settings);

    /* stdin/stdout/stderr is connected to fd. Launch application */
    execvp(argv[0], &argv[0]);

    DEBUG(LID, "Could not execute\n");
    /* application terminated - die and return */
    close(*terminal);
    _exit(1);
}

int login_pty(int fd)
{
    pid_t s;

    s = setsid();
    if (s == -1) {
        s = getsid(0);
    }

    if (tcsetsid(fd, s) == -1) {
        return (-1);
    }

    (void)dup2(fd, 0);
    (void)dup2(fd, 1);
    (void)dup2(fd, 2);

    if (fd > 2) {
        (void)close(fd);
    }

    return (0);
}

int fork_pty(int *amaster, char *name, struct termios *termp,
             struct winsize *winp)
{
    int master, slave, pid;

    if (open_pty(&master, &slave, name, termp, winp) == -1) {
        return (-1);
    }

    switch (pid = fork()) {
    case -1:
        (void)close(slave);
        return (-1);
    case 0:
        /*
         * child
         */
        (void)close(master);
        login_pty(slave);
        return (0);
    }
    /*
     * parent
     */
    *amaster = master;
    (void)close(slave);
    return (pid);
}

int main()
{
    struct pollfd *fds;
    int c, i, pid;
    char buf[256];

    fds = malloc(sizeof(struct pollfd) * 3);
    pid = forkpty(&ptm, NULL, NULL, NULL);

    if (pid == 0) {
        execl("/bin/bash", "", NULL);
    } else {
        fds[0].fd = ptm;
        fds[0].events = POLLIN;

        fds[1].fd = 0;
        fds[1].events = POLLIN;

        while (1) {
            if (poll(fds, 2, 100) > 0) {
                for (i = 0; i < 2; i++) {
                    if (fds[i].revents & POLLIN) {
                        if (fds[i].fd == ptm) {
                            c = read(ptm, buf, 1);
                            write(1, buf, c);
                        }

                        if (fds[i].fd == 0) {
                            c = read(0, buf, 1);
                            //write(1, buf, с);
                            write(ptm, buf, c);
                        }
                    }
                }
            }
        }
    }
    free(fds);
    return 0;
}
