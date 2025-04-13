#include "user.h"
#include "types.h"

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf(1, "Usage: diff num\n");
        exit();
    }

    int result = diff(argv[1], argv[2]);

    printf(1, "result: %d\n", result);
    exit();
}