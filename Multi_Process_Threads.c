#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <winternl.h>

// Import the NtQueryInformationProcess function from ntdll.dll
typedef NTSTATUS(WINAPI* LPFN_NTQUERYINFORMATIONPROCESS)(HANDLE, PROCESS_INFORMATION_CLASS, PVOID, ULONG, PULONG);

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

void DisplayProcessInfo(DWORD processId);
DWORD GetParentProcessId(HANDLE processHandle);
void DisplaySystemInfo();
void KillProcess(DWORD processId);
void SuspendProcess(DWORD processId);
void ResumeProcess(DWORD processId);
HANDLE CreateUserThread(DWORD processId, LPTHREAD_START_ROUTINE threadFunction, LPVOID parameter);
DWORD WINAPI ThreadFunction(LPVOID lpParam);

void DisplayProcessInfo(DWORD processId) {
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (processHandle != NULL) {
        DWORD numberOfThreads = 0;
        if (GetProcessHandleCount(processHandle, &numberOfThreads)) {
            TCHAR processName[MAX_PATH] = TEXT("<unknown>");
            GetModuleBaseName(processHandle, NULL, processName, sizeof(processName) / sizeof(TCHAR));
            DWORD priority = GetPriorityClass(processHandle);

            printf("Process with PID: %lu, Parent PID: %lu, Name: %s, Status: running\n",
                   processId, GetParentProcessId(processHandle), processName);

            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
            if (snapshot != INVALID_HANDLE_VALUE) {
                THREADENTRY32 threadEntry;
                threadEntry.dwSize = sizeof(THREADENTRY32);
                if (Thread32First(snapshot, &threadEntry)) {
                    do {
                        if (threadEntry.th32OwnerProcessID == processId) {
                            HANDLE threadHandle = OpenThread(THREAD_QUERY_INFORMATION, FALSE, threadEntry.th32ThreadID);
                            if (threadHandle != NULL) {
                                int threadPriority = GetThreadPriority(threadHandle);
                                printf("    Thread ID: %lu, Priority: %d\n", threadEntry.th32ThreadID, threadPriority);
                                CloseHandle(threadHandle);
                            }
                        }
                    } while (Thread32Next(snapshot, &threadEntry));
                }
                CloseHandle(snapshot);
            }
        }
        CloseHandle(processHandle);
    }
}

DWORD GetParentProcessId(HANDLE processHandle) {
    PROCESS_BASIC_INFORMATION pbi;
    ULONG returnLength;
    // Call NtQueryInformationProcess using the function pointer
    if (NtQueryInformationProcess(processHandle, ProcessBasicInformation, &pbi, sizeof(pbi), &returnLength) == STATUS_SUCCESS) {
        return (DWORD)pbi.InheritedFromUniqueProcessId;
    }
    return 0;
}

void DisplaySystemInfo() {
    DWORD processIds[1024];
    DWORD cbNeeded;
    if (EnumProcesses(processIds, sizeof(processIds), &cbNeeded)) {
        printf("Processes: %lu\n", cbNeeded / sizeof(DWORD));
        DWORD numThreads = 0;
        for (DWORD i = 0; i < cbNeeded / sizeof(DWORD); i++) {
            DisplayProcessInfo(processIds[i]);
            HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processIds[i]);
            if (processHandle != NULL) {
                DWORD threadCount;
                if (GetProcessHandleCount(processHandle, &threadCount)) {
                    numThreads += threadCount;
                }
                CloseHandle(processHandle);
            }
        }
        printf("Threads: %lu\n", numThreads);
    }
}

void KillProcess(DWORD processId) {
    HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (processHandle != NULL) {
        TerminateProcess(processHandle, 0);
        CloseHandle(processHandle);
    }
}

void SuspendProcess(DWORD processId) {
    HANDLE processHandle = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, processId);
    if (processHandle != NULL) {
        SuspendThread(processHandle);
        CloseHandle(processHandle);
    }
}

void ResumeProcess(DWORD processId) {
    HANDLE processHandle = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, processId);
    if (processHandle != NULL) {
        ResumeThread(processHandle);
        CloseHandle(processHandle);
    }
}

HANDLE CreateUserThread(DWORD processId, LPTHREAD_START_ROUTINE threadFunction, LPVOID parameter) {
    HANDLE processHandle = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, processId);
    if (processHandle != NULL) {
        HANDLE threadHandle = CreateRemoteThread(processHandle, NULL, 0, threadFunction, parameter, 0, NULL);
        CloseHandle(processHandle);
        return threadHandle;
    }
    return NULL;
}

DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    printf("Thread running...\n");
    Sleep(2000);
    printf("Thread work done.\n");
    return 0;
}

int main() {
    DisplaySystemInfo();

    // Kill a process
    KillProcess(123456); // Replace 123456 with the process ID you want to kill

    // Suspend a process
    SuspendProcess(123456); // Replace 123456 with the process ID you want to suspend

    // Resume a process
    ResumeProcess(123456); // Replace 123456 with the process ID you want to resume

    // Create a user-level thread
    HANDLE threadHandle = CreateUserThread(GetCurrentProcessId(), ThreadFunction, NULL);
    if (threadHandle != NULL) {
        WaitForSingleObject(threadHandle, INFINITE);
        CloseHandle(threadHandle);
    }

    return 0;
}
