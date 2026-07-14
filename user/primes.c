#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sieve(int left_fd) {
    int prime;
    // 从左管道读取第一个数（必定是素数）
    if (read(left_fd, &prime, sizeof(prime)) != sizeof(prime)) {
        close(left_fd);
        return;
    }
    printf("prime %d\n", prime);

    int p[2];
    pipe(p);

    if (fork() == 0) {
        // 子进程：关闭新管道的写端，关闭不再使用的左管道，递归
        close(p[1]);
        close(left_fd);
        sieve(p[0]);
        exit(0);
    } else {
        // 父进程：关闭新管道的读端，继续筛选
        close(p[0]);
        int n;
        while (read(left_fd, &n, sizeof(n)) == sizeof(n)) {
            if (n % prime != 0) {
                write(p[1], &n, sizeof(n));
            }
        }
        // 左管道已关闭，关闭新管道写端
        close(left_fd);
        close(p[1]);
        wait(0);  // 等待子进程结束
        exit(0);
    }
}

int main(void) {
    int p[2];
    pipe(p);

    if (fork() == 0) {
        // 子进程：关闭管道的写端，开始筛法
        close(p[1]);
        sieve(p[0]);
        exit(0);
    } else {
        // 父进程：关闭管道的读端，生成数字 2~35
        close(p[0]);
        for (int i = 2; i <= 35; i++) {
            write(p[1], &i, sizeof(i));
        }
        close(p[1]);  // 关闭写端，让子进程读到EOF
        wait(0);      // 等待整个管道链结束
        exit(0);
    }
}
