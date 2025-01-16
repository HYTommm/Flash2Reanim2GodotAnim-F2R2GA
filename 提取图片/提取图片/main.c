#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h> // 包含 TCHAR 相关的定义

// 复制文件函数
void copyFile(const TCHAR* source, const TCHAR* destination) {
    FILE* srcFile = _tfopen(source, _T("rb")); // 打开源文件
    if (srcFile == NULL) {
        _tperror(_T("Failed to open source file")); // 打开失败，打印错误信息并退出
        exit(EXIT_FAILURE);
    }

    FILE* destFile = _tfopen(destination, _T("wb")); // 打开目标文件
    if (destFile == NULL) {
        _tperror(_T("Failed to open destination file")); // 打开失败，打印错误信息并退出
        fclose(srcFile);
        exit(EXIT_FAILURE);
    }

    char buffer[1024]; // 缓冲区
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), srcFile)) > 0) { // 读取源文件内容
        fwrite(buffer, 1, bytesRead, destFile); // 写入目标文件
    }

    fclose(srcFile); // 关闭源文件
    fclose(destFile); // 关闭目标文件
}

// 统计目录中的文件数量函数
int countFilesInDirectory(const TCHAR* sourceDir) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    TCHAR searchPath[MAX_PATH];
    _stprintf_s(searchPath, MAX_PATH, _T("%s\\*"), sourceDir); // 构建搜索路径

    hFind = FindFirstFile(searchPath, &findFileData); // 查找第一个文件
    if (hFind == INVALID_HANDLE_VALUE) {
        _tperror(_T("Failed to open source directory")); // 查找失败，打印错误信息并退出
        exit(EXIT_FAILURE);
    }

    int fileCount = 0;
    do {
        if (_tcscmp(findFileData.cFileName, _T(".")) == 0 || _tcscmp(findFileData.cFileName, _T("..")) == 0) {
            continue; // 跳过当前目录和父目录
        }
        fileCount++;
    } while (FindNextFile(hFind, &findFileData) != 0); // 查找下一个文件

    FindClose(hFind); // 关闭查找句柄
    return fileCount;
}

// 复制目录中的文件函数
void copyFilesInDirectory(const TCHAR* sourceDir, const TCHAR* destDir) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    TCHAR searchPath[MAX_PATH];
    _stprintf_s(searchPath, MAX_PATH, _T("%s\\*"), sourceDir); // 构建搜索路径

    hFind = FindFirstFile(searchPath, &findFileData); // 查找第一个文件
    if (hFind == INVALID_HANDLE_VALUE) {
        _tperror(_T("Failed to open source directory")); // 查找失败，打印错误信息并退出
        exit(EXIT_FAILURE);
    }

    do {
        if (_tcscmp(findFileData.cFileName, _T(".")) == 0 || _tcscmp(findFileData.cFileName, _T("..")) == 0) {
            continue; // 跳过当前目录和父目录
        }

        TCHAR sourcePath[MAX_PATH];
        _stprintf_s(sourcePath, MAX_PATH, _T("%s\\%s"), sourceDir, findFileData.cFileName); // 构建源文件路径

        TCHAR destPath[MAX_PATH];
        _stprintf_s(destPath, MAX_PATH, _T("%s\\%s"), destDir, findFileData.cFileName); // 构建目标文件路径

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // 如果是目录，递归处理
            int subDirFileCount = countFilesInDirectory(sourcePath);
            if (subDirFileCount > 1) {
                _tprintf(_T("Skipping directory %s because it contains more than one file.\n"), sourcePath);
                continue; // 跳过该子目录
            }
            //CreateDirectory(destPath, NULL); // 创建目标目录
            copyFilesInDirectory(sourcePath, destDir); // 递归复制子目录中的文件
        }
        else {
            copyFile(sourcePath, destPath); // 如果不是目录，则复制文件
        }
    } while (FindNextFile(hFind, &findFileData) != 0); // 查找下一个文件

    FindClose(hFind); // 关闭查找句柄
}

int _tmain(int argc, TCHAR* argv[]) {
    if (argc != 3) {
        _ftprintf(stderr, _T("Usage: %s <source_directory> <destination_directory>\n"), argv[0]); // 打印用法信息
        exit(EXIT_FAILURE);
    }

    const TCHAR* sourceDir = argv[1];
    const TCHAR* destDir = argv[2];

    copyFilesInDirectory(sourceDir, destDir); // 复制目录中的文件

    _tprintf(_T("Files copied from %s to %s successfully.\n"), sourceDir, destDir); // 打印成功信息

    return 0;
}
