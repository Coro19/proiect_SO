#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>   
#include "city_manager.h"

void cmd_remove_district(const char *role, const char *user, const char *district) {
    if (strcmp(role, "manager") != 0) {
        printf("Only managers are allowed to remove districts\n");
        return;
    }
    log_action(district, user, role, "remove_district");
    // Fork a child to run rm -rf
    pid_t pid = fork();
    if (pid < 0) {
        printf("Fork failed.\n");
        return;
    }
    if (pid == 0) {
        execl("/bin/rm", "rm", "-rf", district, NULL);
        printf("Error: execl failed.\n");
        _exit(1);
    }
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        // Remove the symlink
        char linkname[256];
        snprintf(linkname, sizeof(linkname), "active_reports-%s", district);
        unlink(linkname);
        printf("District '%s' removed.\n", district);
    }
}