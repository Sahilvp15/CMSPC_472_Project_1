#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>

#define THREAD_COUNT 4

typedef struct {
    char *text;        //  Segment of text for this thread to process
    int start; //  Start index for this segment
    int end; // End  index for this segment
    int *char_counts; // Array  for counting character occurrences
} ThreadData;

void *processFileSegment(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int local_char_counts[26] = {0}; //  Local character counts to avoid concurrency issues

    // Process characters in segment to uppercase  and count occurrences
    for (int i = data->start; i < data->end && data->text[i] != '\0'; i++) {
        if (isalpha(data->text[i])) {
            char upperChar = toupper(data->text[i]);
            local_char_counts[upperChar - 'A']++;
        }
    }

    // Safely update shared character counts
    for (int i = 0; i < 26; i++) {
        __sync_fetch_and_add(&data->char_counts[i], local_char_counts[i]);
    }

    return NULL;
}

void parallelTextProcessing(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Seek to the end to get the file size
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET); // Go back to the start of the file

    // Allocate memory for the entire text
    char *text = (char *)malloc(length + 1);
    if (!text) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Read the file into memory and null-terminate the string
    fread(text, 1, length, file);
    text[length] = '\0';
    fclose(file);

    // Prepare for threading
    pthread_t threads[THREAD_COUNT];
    ThreadData threadData[THREAD_COUNT];
    int char_counts[26] = {0}; // Shared character counts

    
    int segment_length = length / THREAD_COUNT;  // Calculate segment sizes and create threads
    for (int i = 0; i < THREAD_COUNT; i++) {
        threadData[i].text = text;
        threadData[i].start = i * segment_length;
        threadData[i].end = (i == THREAD_COUNT - 1) ? length : (i + 1) * segment_length;
        threadData[i].char_counts = char_counts;

        if (pthread_create(&threads[i], NULL, processFileSegment, &threadData[i])) {
            perror("Failed to create thread");
            free(text);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads  to finish
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print character  counts
    for (int i = 0; i < 26; i++) {
        if (char_counts[i] > 0) {
            printf("%c: %d\n", 'A' + i, char_counts[i]);
        }
    }

    free(text);
}

int main() {
    char filename[] = "input.txt"; // input filename
    parallelTextProcessing(filename);
    return 0;
}

