#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "city_manager.h"

#define MAX_INSPECTORS 64

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: scorer <district>\n");
        return 1;
    }
    const char *district = argv[1];
    char path[256];
    snprintf(path, sizeof(path), "%s/reports.dat", district);
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("scorer: cannot open %s\n", path);
        return 1;
    }
    char names[MAX_INSPECTORS][INSPECTOR_LEN];
    int scores[MAX_INSPECTORS];
    int count = 0;
    Report r;
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        int found = 0;
        for (int i = 0; i < count; i++) {
            if (strcmp(names[i], r.inspector) == 0) {
                scores[i] += r.severity;
                found = 1;
                break;
            }
        }
        if (!found && count < MAX_INSPECTORS) {
            strncpy(names[count], r.inspector, INSPECTOR_LEN - 1);
            scores[count] = r.severity;
            count++;
        }
    }
    close(fd);
    printf("District: %s\n", district);
    for (int i = 0; i < count; i++) {
        printf("  Inspector: %-20s Score: %d\n", names[i], scores[i]);
    }
    printf("\n");
    fflush(stdout);
    return 0;
}