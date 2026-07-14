#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

void find(char *path, char *name) {
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        // 如果传入的不是目录，不处理（实际上主函数传入的是目录）
        close(fd);
        return;
    }

    char buf[512];
    char *p;
    // 复制路径，确保以 '/' 结尾
    strcpy(buf, path);
    p = buf + strlen(buf);
    if (*(p - 1) != '/') {
        *p++ = '/';
    }

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
            continue;
        // 跳过 "." 和 ".."
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;

        // 构造完整路径
        char *q = p;
        char *s = de.name;
        while (*s)
            *q++ = *s++;
        *q = 0;

        if (stat(buf, &st) < 0) {
            // 如果 stat 失败，跳过
            continue;
        }

        if (st.type == T_DIR) {
            // 递归进入子目录
            find(buf, name);
        } else if (st.type == T_FILE) {
            // 比较文件名
            if (strcmp(de.name, name) == 0) {
                printf("%s\n", buf);
            }
        }
        // 其他类型忽略
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <directory> <filename>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
