#include <stdio.h>
#include <string.h>

#define MAX_STRINGS 5

const char* find_prefix_match(const char* input, const char* arr[], int size) {
    for (int i = size - 1; i >= 0; i--) {
        if (strncmp(arr[i], input, strlen(input)) == 0) {
            return arr[i];
        }
    }
    return NULL;
}

int main() {
    const char* words[MAX_STRINGS] = {"device123", "hospital", "monitor", "ss", "scan"};
    const char* input = "dev";

    const char* result = find_prefix_match(input, words, MAX_STRINGS);
    
    if (result) {
        printf("First match found: %s\n", result);
    } else {
        printf("No match found.\n");
    }

    return 0;
}
