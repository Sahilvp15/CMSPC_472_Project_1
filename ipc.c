#include <windows.h>
#include <stdio.h>

#define SHAR_SIZE 256    //It declare the memory size
#define SHAR_NAME "SimpleSharedMemoryExample"  //It declare the memory name

void CreateAndWriteSharedMemory() {
    HANDLE hMapFile;
    LPVOID pBuf;

    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    
        NULL,                    // Default security descriptors
        PAGE_READWRITE,         
        0,                       // Max. object size (high-order DWORD).
        SHAR_SIZE,      // Max. object size (low-order DWORD).
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

    CopyMemory(pBuf, "Hello from parent!", 18);
    printf("Message written to shared memory: 'Hello from parent!'\n");  //Message

}

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

    printf("Message read from shared memory: %s\n", (char*)pBuf); 

}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "child") == 0) {  
        ReadSharedMemory();
    } else {
        CreateAndWriteSharedMemory();
    }

    return 0;
}
