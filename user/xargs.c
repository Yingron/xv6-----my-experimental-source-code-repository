#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"   // MAXARG

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs command [args...]\n");
        exit(1);
    }

    char *cmd = argv[1];
    char *fixed_args[MAXARG];
    int fixed_argc = argc - 2;
    for (int i = 0; i < fixed_argc; i++) {
        fixed_args[i] = argv[i + 2];
    }

    char line[512];
    char c;
    int i;

    while (1) {
        // 尝试读取第一个字符，判断 EOF
        if (read(0, &c, 1) != 1) {
            break;   // 没有更多输入
        }

        i = 0;
        // 处理第一个字符（如果不是换行）
        if (c != '\n') {
            line[i++] = c;
        }

        // 继续读直到换行或 EOF
        while (i < sizeof(line) - 1) {
            if (read(0, &c, 1) != 1) {
                break;   // EOF 在行中间，视为行结束
            }
            if (c == '\n') {
                break;
            }
            line[i++] = c;
        }
        line[i] = '\0';

        // 构建 exec 参数数组
        char *exec_argv[MAXARG + 1];
        exec_argv[0] = cmd;
        int idx = 1;
        for (int j = 0; j < fixed_argc; j++) {
            exec_argv[idx++] = fixed_args[j];
        }
        exec_argv[idx++] = line;
        exec_argv[idx] = 0;

        int pid = fork();
        if (pid == 0) {
            exec(cmd, exec_argv);
            fprintf(2, "xargs: exec %s failed\n", cmd);
            exit(1);
        } else if (pid > 0) {
            wait(0);
        } else {
            fprintf(2, "xargs: fork failed\n");
            exit(1);
        }
    }

    exit(0);
}
