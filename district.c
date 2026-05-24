#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "city_manager.h"

void check_symlink(const char *district) {
    char linkname[256];
    snprintf(linkname, sizeof(linkname), "active_reports-%s", district);

    struct stat lst, st;
    if (lstat(linkname, &lst) < 0) return; 
    if (!S_ISLNK(lst.st_mode)) return;     

    if (stat(linkname, &st) < 0)     
        printf("Warning: dangling symlink '%s' (target does not exist)\n", linkname);
}

void init_district(const char *district){
    char path[256];
    //district dir
    mkdir(district,0750);

    //creating reports.dat
    snprintf(path,sizeof(path),"%s/reports.dat",district);
    int fd = open(path,O_CREAT | O_WRONLY | O_APPEND, 0664);
    if(fd >= 0) close(fd);
    chmod(path,0664);

    //district.cfg
    snprintf(path,sizeof(path),"%s/district.cfg",district);
    fd = open(path,O_CREAT | O_EXCL | O_WRONLY, 0640);
    if( fd >= 0){
        write(fd, "threshold=1\n",12);
        close(fd);
    }
    chmod(path,0640);

    //logged_district
    snprintf(path, sizeof(path), "%s/logged_district", district);
    fd = open(path,O_CREAT | O_WRONLY | O_APPEND, 0664);
    if(fd>=0) close(fd);
    chmod(path, 0644);

    //symlink
    char target[256];
    char linkname[256];
    snprintf(target, sizeof(target), "%s/reports.dat",district);
    snprintf(linkname, sizeof(linkname), "active_reports-%s",district);
    unlink(linkname);
    symlink(target,linkname);
}