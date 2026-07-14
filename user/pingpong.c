#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int p1[2], p2[2]; // p1: parent->child, p2: child->parent
  char buf[1];

  if (pipe(p1) < 0 || pipe(p2) < 0) {
    fprintf(2, "pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // child process
    close(p1[1]); // close write end of p1
    close(p2[0]); // close read end of p2

    if (read(p1[0], buf, 1) != 1) {
      fprintf(2, "child: read failed\n");
      exit(1);
    }
    printf("%d: received ping\n", getpid());

    if (write(p2[1], buf, 1) != 1) {
      fprintf(2, "child: write failed\n");
      exit(1);
    }

    close(p1[0]);
    close(p2[1]);
    exit(0);
  } else {
    // parent process
    close(p1[0]); // close read end of p1
    close(p2[1]); // close write end of p2

    buf[0] = 'a';
    if (write(p1[1], buf, 1) != 1) {
      fprintf(2, "parent: write failed\n");
      exit(1);
    }

    if (read(p2[0], buf, 1) != 1) {
      fprintf(2, "parent: read failed\n");
      exit(1);
    }
    printf("%d: received pong\n", getpid());

    close(p1[1]);
    close(p2[0]);
    wait(0);
    exit(0);
  }
}
