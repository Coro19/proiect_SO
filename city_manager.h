#pragma once
#include <time.h>

#define INSPECTOR_LEN 64
#define CATEGORY_LEN 32
#define DESC_LEN 128

typedef struct {
    double lat;
    double lon;
    time_t timestamp;
    int id;
    int severity;
    char inspector[INSPECTOR_LEN];
    char category[CATEGORY_LEN];
    char description[DESC_LEN];
} Report;
void cmd_add(const char *role, const char *user, const char *district);
void cmd_list(const char *role, const char *user, const char *district);
void cmd_view(const char *role, const char *user, const char *district, int id);
void cmd_remove_report(const char *role, const char *user, const char *district, int id);
void cmd_update_threshold(const char *role, const char *user, const char *district, int threshold);
void cmd_filter(const char *role, const char *user, const char *district, char **conditions, int cond_count);
void permission_to_string(mode_t mode, char *str);
int parse_condition(const char *input, char *field, char *op, char *value);
int match_condition(Report *r, const char *field, const char *op, const char *value);
int check_permission(const char *path, const char *role, int need_read, int need_write);
void log_action(const char *district, const char *user, const char *role, const char *action);
void init_district(const char *district);
void check_symlink(const char *district);