#include <stdio.h>
#include <windows.h>
#include <shlwapi.h> // For PathFindExtension function
#include <wchar.h>

#pragma comment(lib, "shlwapi.lib") // Link with shlwapi.lib

void CreateFolderForExtension(LPCWSTR extension) {
    WCHAR folderName[MAX_PATH];
    wsprintf(folderName, L"%sFolder", extension); // Folder name will be "ExtensionFolder"

    if (!CreateDirectoryW("folderName", NULL)) {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            wprintf(L"Folder %s already exists.\n", folderName);
        } else {
            wprintf(L"Failed to create folder for extension %s\n", extension);
        }
    }
}

void MoveFileToFolder(LPCWSTR filePath, LPCWSTR folderName) {
    WCHAR newFilePath[MAX_PATH];
    wsprintf(newFilePath, L"%s\\%s", folderName, PathFindFileNameW(filePath));

    if (!MoveFile(filePath, newFilePath)) {
        wprintf(L"Failed to move file %s to folder %s\n", filePath, folderName);
    }
}

int main() {
    // Get the current directory
    WCHAR currentDir[MAX_PATH];
    DWORD dwRet = GetCurrentDirectoryW(MAX_PATH, currentDir);

    if (dwRet == 0) {
        wprintf(L"Failed to get current directory.\n");
        return 1;
    }

    // Print the current directory
    wprintf(L"Current Directory: %s\n", currentDir);

    // Remove the last directory component from the current directory path
    WCHAR* lastSlash = wcsrchr(currentDir, L'\\');
    if (lastSlash != NULL) {
        *lastSlash = L'\0'; // Set the last slash to null terminator to remove the last component
    } else {
        wprintf(L"Already at root directory.\n");
        return 1;
    }

    // Specify the directory path you want to list files from
    WCHAR directoryPath[MAX_PATH];
    wcscpy_s(directoryPath, MAX_PATH, currentDir);
    wcscat_s(directoryPath, MAX_PATH, L"\\*");

    // Find the first file in the directory
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFileW(directoryPath, &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // Get the file extension
            LPCWSTR extension = PathFindExtensionW(findFileData.cFileName);
            if (wcslen(extension) > 0) {
                // Remove leading period
                extension++;

                // Create a folder for the extension if it doesn't exist
                CreateFolderForExtension(extension);

                // Move the file to the folder
                WCHAR filePath[MAX_PATH];
                wsprintf(filePath, L"%s\\%s", currentDir, findFileData.cFileName);
                MoveFileToFolder(filePath, extension);
            }
        } while (FindNextFileW(hFind, &findFileData) != 0);

        // Close the search handle
        FindClose(hFind);
    } else {
        wprintf(L"Failed to list files in directory.\n");
        return 1;
    }

    // Wait for a keystroke before exiting
    wprintf(L"Press any key to exit...\n");
    getwchar();

    return 0;
}
