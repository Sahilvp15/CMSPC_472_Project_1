# Operating System Project

## Overview
This project is a comprehensive simulation of various aspects of an operating system, including multi-process and thread management, inter-process communication (IPC), and parallel text processing. The simulation aims to provide a practical understanding of these core OS concepts through hands-on implementation and execution.

## Table of Contents
1. [Introduction](#introduction)
2. [Multi-Process and Thread Management](#multi-process-and-thread-management)
3. [Inter-Process Communication (IPC)](#inter-process-communication-ipc)
4. [Parallel Text Processing](#parallel-text-processing)
5. [Results and Observations](#results-and-observations)
6. [Conclusion](#conclusion)
7. [Dependencies](#dependencies)

## Introduction
This project embarked on developing a comprehensive operating system (OS) simulation, focusing on key areas: multi-process and thread management, inter-process communication (IPC), and parallel text processing. The simulation is designed to provide an immersive, hands-on experience in managing OS-level processes and threads, implementing IPC mechanisms, and leveraging parallel computing for efficient text file processing.

## Multi-Process and Thread Management
Objective: To simulate an OS's capability to manage multiple processes and threads, offering functionalities like creation, suspension, resumption, and termination.

Implementation:
- Includes a Windows-based multi-threaded code.
- Initiated a separate thread to simulate a task.
- Incorporated thread completion management and clean resource deallocation.

Structure:
- Utilized modular functions for process and thread creation (CreateProcess and CreateThread).
- Implemented synchronization with WaitForSingleObject for orderly execution.
- Integrated error handling via GetLastError for robust operation.
- Encapsulated functionality within a main function for simplicity and scalability.
- Additional functions like DisplaySystemInfo and DisplayProcessInfo enhance system information visibility.

## Inter-Process Communication (IPC)
Objective: The code demonstrates the use of shared memory, an essential mechanism for enabling processes to efficiently share data.

Implementation: The ipc.c file provides an example of both shared memory and message passing mechanisms. It demonstrates creating a shared memory segment for inter-process data sharing and using message queues for process-to-process messaging.

Structure:
- Initializes and maps a specified memory segment into the process's address space for inter-process access.
- Illustrates inter-process communication through direct writing to and reading from shared memory.
- Includes error handling for file mapping, memory mapping, process/thread creation, and other critical operations.

## Parallel Text Processing
Objective: To implement a system for processing large text files in parallel, demonstrating the effectiveness of parallel computing.

Implementation: The parallel_text_processing.c file showcases dividing a large text file into segments processed by individual threads, each converting characters to uppercase and counting character occurrences.

Structure:
- The text file is read into memory, divided into segments.
- Threads are spawned to process each segment in parallel.
- Character occurrences are counted and aggregated.

## Results and Observations
The functionalities were verified through execution, focusing on the correct implementation of process and thread management, IPC mechanisms, and parallel text processing efficiency. Each component behaved as designed, reflecting the core principles intended for demonstration.

Challenges:
- Ensuring accurate synchronization among processes and threads.
- Measuring and comparing IPC mechanism performance under various conditions.

Insights:
- Proper thread and process synchronization are crucial for reliable IPC and parallel processing.
- Performance of IPC mechanisms vary with the data size and operation frequency, highlighting the importance of choosing the right IPC method based on specific needs.

## Conclusion
This project successfully simulates crucial aspects of an operating system, offering valuable insights into process and thread management, IPC, and parallel computing. While challenges were encountered, particularly in synchronization and performance evaluation, the project provides a solid foundation for understanding and exploring operating system functionalities further. Future work could focus on enhancing user interaction, dynamic thread management, and exploring additional IPC mechanisms.

## Dependencies
- C Compiler (e.g., GCC)
