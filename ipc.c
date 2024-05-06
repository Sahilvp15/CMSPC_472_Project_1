#include <windows.h>
#include <stdio.h>
#include <time.h>

#define SHAR_SIZE 256    // Shared memory size
#define SHAR_NAME "SimpleSharedMemoryExample"  // Shared memory name

// Function to create and write to shared memory
void CreateAndWriteSharedMemory(int message_count, int message_size) {
    HANDLE hMapFile;
    LPVOID pBuf;

    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    
        NULL,                    // Default security descriptors
        PAGE_READWRITE,         
        0,                       // Max. object size (high-order DWORD).
        SHAR_SIZE,               // Max. object size (low-order DWORD).
        SHAR_NAME);    

    if (hMapFile == NULL) {
        printf("Could not create file mapping object (%lu).\n", GetLastError());
        return;
    } else {
        printf("Shared memory created successfully.\n");
    }

    pBuf = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHAR_SIZE);

    if (pBuf == NULL) {
        printf("Could not map view of file (%lu).\n", GetLastError());
        CloseHandle(hMapFile);
        return;
    } else {
        printf("Shared memory mapped successfully.\n");
    }

    // Write multiple messages to shared memory
    for (int i = 0; i < message_count; i++) {
        char message[message_size];
        sprintf(message, "Message %d", i);
        CopyMemory(pBuf, message, message_size);
        printf("Message written to shared memory: '%s'\n", message);
    }

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
}

// Function to read from shared memory
void ReadSharedMemory() {
    HANDLE hMapFile;
    LPCVOID pBuf;

    printf("Child process attempting to open shared memory...\n");

    hMapFile = OpenFileMapping(FILE_MAP_READ, FALSE, SHAR_NAME);

    if (hMapFile == NULL) {
        printf("Could not open file mapping object (%lu).\n", GetLastError());
        return;
    } else {
        printf("Shared memory opened successfully.\n");
    }

    pBuf = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, SHAR_SIZE);

    if (pBuf == NULL) {
        printf("Could not map view of file (%lu).\n", GetLastError());
        CloseHandle(hMapFile);
        return;
    } else {
        printf("Shared memory mapped successfully.\n");
    }

    // Read and print the messages from shared memory
    for (int i = 0; i < SHAR_SIZE / sizeof(char); i += sizeof(char)) {
        printf("Message read from shared memory: %s\n", (char*)pBuf + i);
    }

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
}

// Function to measure the performance of IPC mechanisms
void MeasureIPCPerformance(int message_count, int message_size, int mechanism) {
    clock_t start_time, end_time;
    double elapsed_time;

    if (mechanism == 1) {
        // Measure performance of IPC over processes
        start_time = clock();
        CreateAndWriteSharedMemory(message_count, message_size);
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        if (!CreateProcess(NULL, "ipc.exe child", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            printf("Failed to create child process (%lu).\n", GetLastError());
            return;
        }
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        end_time = clock();
        elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        printf("IPC over processes took %.6f seconds.\n", elapsed_time);
    } else {
        // Measure performance of IPC over threads
        start_time = clock();
        CreateAndWriteSharedMemory(message_count, message_size);
        HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadSharedMemory, NULL, 0, NULL);
        if (hThread == NULL) {
            printf("Failed to create thread (%lu).\n", GetLastError());
            return;
        }
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        end_time = clock();
        elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        printf("IPC over threads took %.6f seconds.\n", elapsed_time);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "child") == 0) {
        ReadSharedMemory();
    } else {
        printf("Evaluating IPC over processes...\n");
        MeasureIPCPerformance(100, 10, 1);  // 100 short messages
        printf("\nEvaluating IPC over processes...\n");
        MeasureIPCPerformance(10, 1024, 1); // 10 long messages

        printf("\nEvaluating IPC over threads...\n");
        MeasureIPCPerformance(100, 10, 2);  // 100 short messages
        printf("\nEvaluating IPC over threads...\n");
        MeasureIPCPerformance(10, 1024, 2); // 10 long messages
    }

    return 0;
}