#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "city_manager.h"

int main(int argc, char*argv[]){
    char *role = NULL;
    char *user = NULL;
    char *command = NULL;
    char *arg1 = NULL;
    char *arg2 = NULL;
    char *conditions[16];
    int cond_count = 0;
    for(int i = 1; i < argc; i++){
        if      (strcmp(argv[i], "--role") == 0)  role    = argv[++i];
        else if (strcmp(argv[i], "--user") == 0)  user    = argv[++i];
        else if (strcmp(argv[i], "--add") == 0)           { command = "add";             arg1 = argv[++i]; }
        else if (strcmp(argv[i], "--list") == 0)          { command = "list";            arg1 = argv[++i]; }
        else if (strcmp(argv[i], "--view") == 0)          { command = "view";            arg1 = argv[++i]; arg2 = argv[++i]; }
        else if (strcmp(argv[i], "--remove_report") == 0) { command = "remove_report";   arg1 = argv[++i]; arg2 = argv[++i]; }
        else if (strcmp(argv[i], "--update_threshold")==0){ command = "update_threshold";arg1 = argv[++i]; arg2 = argv[++i]; }
        else if (strcmp(argv[i], "--filter") == 0)        { command = "filter";          arg1 = argv[++i];
             while (i + 1 < argc && argv[i+1][0] != '-') {
                conditions[cond_count++] = argv[++i];
            }
         }
    }
    if(!role || !command || !user){
        printf("Usage: city_manager --role <role> --user <user> --<command> [args]\n");
        return 1;
    }
    if(strcmp(role,"inspector")!=0 && strcmp(role,"manager") != 0){
        printf("User must be either 'inspector' or 'manager'\n");
        return 2;
    }
    if(strcmp(command,"add")==0)
        cmd_add(role,user,arg1);
    else if(strcmp(command,"list")==0)
        cmd_list(role,user,arg1);
    else if(strcmp(command,"view")==0)
        cmd_view(role,user,arg1,atoi(arg2));
    else if(strcmp(command,"remove_report")==0)
        cmd_remove_report(role,user,arg1,atoi(arg2));
    else if(strcmp(command,"update_threshold")==0)
        cmd_update_threshold(role,user,arg1,atoi(arg2));
    else if(strcmp(command,"filter")==0)
    cmd_filter(role, user, arg1, conditions, cond_count);
        return 0;
}