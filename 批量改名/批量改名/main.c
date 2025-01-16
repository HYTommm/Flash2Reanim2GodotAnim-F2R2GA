#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void renameDirectoriesAndFiles(const char *directory, const char *symbol) {
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char dirSpec[MAX_PATH];
    snprintf(dirSpec, MAX_PATH, "%s\\*", directory);

    hFind = FindFirstFileA(dirSpec, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("FindFirstFile failed (%d)\n", GetLastError());
        return;
    }

    do {
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) {
            continue;
        }

        char oldPath[MAX_PATH];
        char newPath[MAX_PATH];
        char num[10] = "0"; // 默认数字为0

        snprintf(oldPath, MAX_PATH, "%s\\%s", directory, findFileData.cFileName);

        // 提取文件名或目录名中的数字
        for (int i = 0; i < strlen(findFileData.cFileName); i++) {
            if (findFileData.cFileName[i] >= '0' && findFileData.cFileName[i] <= '9') {
                int j = 0;
                while (findFileData.cFileName[i] >= '0' && findFileData.cFileName[i] <= '9' && j < sizeof(num) - 1) {
                    num[j++] = findFileData.cFileName[i++];
                }
                num[j] = '\0';
                break;
            }
        }

        // 检查父目录名称是否包含“Symbol”和数字
        const char *parentDirName = strrchr(directory, '\\');

        if (parentDirName != NULL)
        {
            parentDirName++; // 跳过反斜杠
            if (strstr(parentDirName, symbol) != NULL && strpbrk(parentDirName, "0123456789") != NULL)
            {
                snprintf(newPath, MAX_PATH, "%s\\%s", directory, parentDirName);
            }
            else
            {
                snprintf(newPath, MAX_PATH, "%s\\%s %s", directory, symbol, num);
            }
        }
        else
        {
            snprintf(newPath, MAX_PATH, "%s\\%s %s", directory, symbol, num);
        }

        // 如果是文件而非文件夹，添加“.png”后缀
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            strncat_s(newPath, MAX_PATH, ".png", _TRUNCATE);
        }

        if (MoveFileA(oldPath, newPath) == 0) {
            printf("MoveFile failed (%d)\n", GetLastError());
        } else {
            printf("Renamed: %s -> %s\n", oldPath, newPath);
        }

        // 递归处理子目录
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            renameDirectoriesAndFiles(newPath, symbol);
        }
    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <directory>\n", argv[0]);
        return 1;
    }

	printf("test point 1\n");
    const char *directory = argv[1]; // 目标目录
    const char *symbol = "Symbol"; // 符号

    renameDirectoriesAndFiles(directory, symbol);

    return 0;
}
