#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include "city_manager.h"
int parse_condition(const char *input, char *field, char *op, char *value) {
    const char *first = strchr(input, ':');
    if (!first) return 0;
    int field_len = first - input;
    if (field_len >= 64) return 0; //safety check for invalid length

    const char *second = strchr(first + 1, ':');
    if (!second) return 0;

    strncpy(field, input, first - input);
    field[first - input] = '\0';

    strncpy(op, first + 1, second - first - 1);
    op[second - first - 1] = '\0';

    strncpy(value, second + 1, 63);
    value[63] = '\0';

    return 1;
}

int match_condition(Report *r, const char *field, const char *op, const char *value) {
    if (strcmp(field, "severity") == 0) {
        int v = atoi(value);
        if (strcmp(op, "==") == 0) return r->severity == v;
        if (strcmp(op, "!=") == 0) return r->severity != v;
        if (strcmp(op, "<")  == 0) return r->severity <  v;
        if (strcmp(op, "<=") == 0) return r->severity <= v;
        if (strcmp(op, ">")  == 0) return r->severity >  v;
        if (strcmp(op, ">=") == 0) return r->severity >= v;
    } else if (strcmp(field, "category") == 0) {
        int cmp = strcmp(r->category, value);
        if (strcmp(op, "==") == 0) return cmp == 0;
        if (strcmp(op, "!=") == 0) return cmp != 0;
        printf("Warning: operator %s not supported for string fields.\n", op);
        return 0;
    } else if (strcmp(field, "inspector") == 0) {
        int cmp = strcmp(r->inspector, value);
        if (strcmp(op, "==") == 0) return cmp == 0;
        if (strcmp(op, "!=") == 0) return cmp != 0;
    } else if (strcmp(field, "timestamp") == 0) {
        time_t v = (time_t)atol(value);
        if (strcmp(op, "==") == 0) return r->timestamp == v;
        if (strcmp(op, "!=") == 0) return r->timestamp != v;
        if (strcmp(op, "<")  == 0) return r->timestamp <  v;
        if (strcmp(op, "<=") == 0) return r->timestamp <= v;
        if (strcmp(op, ">")  == 0) return r->timestamp >  v;
        if (strcmp(op, ">=") == 0) return r->timestamp >= v;
    }
    printf("Warning: unknown field '%s'.\n", field);
    return 0;
}

void cmd_filter(const char *role, const char *user, const char *district, char **conditions, int cond_count) {
    check_symlink(district);
    char path[256];
    snprintf(path, sizeof(path), "%s/reports.dat", district);
    if (!check_permission(path, role, 1, 0)) return;

    int fd = open(path, O_RDONLY);
    if (fd < 0) { printf("District '%s' not found.\n", district); return; }

    // Parse all conditions upfront
    char fields[16][64], ops[16][8], values[16][64];
    for (int c = 0; c < cond_count; c++) {
        if (!parse_condition(conditions[c], fields[c], ops[c], values[c])) {
            printf("Error: invalid condition '%s'. Use field:op:value\n", conditions[c]);
            close(fd);
            return;
        }
    }

    Report r;
    int count = 0;
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        int match = 1;
        for (int c = 0; c < cond_count; c++) {
            if (!match_condition(&r, fields[c], ops[c], values[c])) {
                match = 0;
                break;
            }
        }
        if (match) {
            char ts[64];
            strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", localtime(&r.timestamp));
            printf("[%d] %s | %s | %.4f,%.4f | sev:%d | %s | %s\n",
                r.id, r.inspector, r.category, r.lat, r.lon,
                r.severity, r.description, ts);
            count++;
        }
    }

    close(fd);
    if (count == 0) printf("No reports match the conditions.\n");
    log_action(district, user, role, "filter");
}