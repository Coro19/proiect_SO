#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "city_manager.h"
#include <fcntl.h>

int check_permission(const char *path, const char *role, int need_read, int need_write) {
    struct stat st;
    if (stat(path, &st) < 0) {
        printf("Error: cannot stat %s\n", path);
        return 0;
    }

    mode_t m = st.st_mode;

    // Managers-user bits, inspectors-group
    if (strcmp(role, "manager") == 0) {
        if (need_read  && !(m & S_IRUSR)) { printf("Error: manager lacks read on %s\n",  path); return 0; }
        if (need_write && !(m & S_IWUSR)) { printf("Error: manager lacks write on %s\n", path); return 0; }
    } else {
        if (need_read  && !(m & S_IRGRP)) { printf("Error: inspector lacks read on %s\n",  path); return 0; }
        if (need_write && !(m & S_IWGRP)) { printf("Error: inspector lacks write on %s\n", path); return 0; }
    }

    return 1;
}