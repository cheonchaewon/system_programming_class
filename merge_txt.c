#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#define BUF_SIZE 1024

int has_txt_extension(const char *filename) {
    size_t len = strlen(filename);
    return len > 4 && strcmp(filename + len - 4, ".txt") == 0;
}

void write_filtered(int out_fd, const char *buf) {
    for (int i = 0; buf[i] != '\0'; i++) {
        if (buf[i] != ' ' && buf[i] != '\n' && buf[i] != '\r') {  // 공백, 줄바꿈 전부 제거
            write(out_fd, &buf[i], 1);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "사용법: %s <source_directory> <output_file>\n", argv[0]);
        exit(1);
    }

    const char *src_dir = argv[1];
    const char *out_file = argv[2];

    DIR *dir = opendir(src_dir);
    if (!dir) {
        perror("디렉토리 열기 실패");
        exit(1);
    }

    int out_fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd < 0) {
        perror("결과 파일 열기 실패");
        closedir(dir);
        exit(1);
    }

    struct dirent *entry;
    char filepath[1024];
    char buf[BUF_SIZE];

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && has_txt_extension(entry->d_name)) {
            snprintf(filepath, sizeof(filepath), "%s/%s", src_dir, entry->d_name);
            FILE *fp = fopen(filepath, "r");
            if (!fp) {
                perror("입력 파일 열기 실패");
                continue;
            }

            while (fgets(buf, sizeof(buf), fp)) {
                write_filtered(out_fd, buf);
            }

            fclose(fp);
        }
    }

    close(out_fd);
    closedir(dir);

    printf("병합 완료: %s\n", out_file);
    return 0;
}

