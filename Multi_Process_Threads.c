#include <windows.h>
#include <stdio.h>

// Thread function
DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    printf("Thread running...\n");
    //This code stimulate the work
    Sleep(2000);
    printf("Thread work done.\n");
    return 0;
}

// Here is the main function to process and thread creation
int main(void) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess("C:\\Windows\\System32\\notepad.exe", 
                       NULL,          
                       NULL,         
                       NULL,          
                       FALSE,         
                       0,             // No creation flags
                       NULL,        
                       NULL,        
                       &si,           
                       &pi)         
    ) {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return -1;
    }

    printf("Process created successfully.\n");

    // It creates a thread
    DWORD threadId;
    HANDLE threadHandle = CreateThread(NULL, 0, ThreadFunction, NULL, 0, &threadId);

    if (threadHandle == NULL) {
        printf("CreateThread failed (%d)\n", GetLastError());
        //It properly close the process handle if thread creation fails
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return -1;
    }

    // This wait for the thread untill complete
    WaitForSingleObject(threadHandle, INFINITE);
    printf("Main thread work done.\n");

    // It close process and thread handles
    CloseHandle(threadHandle);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
