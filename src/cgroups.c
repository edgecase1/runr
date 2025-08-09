
#include <sys/stat.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include "cgroups.h"

#define CGROUP_DEVICES "/sys/fs/cgroup/devices/%s"
#define CGROUP_DEVICES_DENY "%s/devices.deny"
#define CGROUP_DEVICES_LIST "%s/devices.list"
#define CGROUP_DEVICES_ALLOW "%s/devices.allow"
#define CGROUP_DEVICES_PROCS "%s/cgroup.procs"

// is /proc mounted?
// is /sys mounted?
// is /sys/fs/cgroup

int
get_cgroup_path(char* cgroup_name, char* cgroup_path)
{
    sprintf(cgroup_path, CGROUP_DEVICES, cgroup_name); 
}

void
create_sub_devicecgroup(char* cgroup_name)
{
    char subgroup_path[255];
    sprintf(subgroup_path, CGROUP_DEVICES, cgroup_name); 

    // check if file exists
    if(mkdir(subgroup_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
    {
        perror("error creating subgroup");
    }
}

void
delete_sub_devicecgroup(char* cgroup_name)
{
    char subgroup_path[255];
    sprintf(subgroup_path, CGROUP_DEVICES, cgroup_name); 

    // check if file exists
    if(rmdir(subgroup_path) == -1)
    {
        perror("error deleting subgroup");
    }
}

void
deny_devices(char* mask, char* cgroup_name)
{
    FILE* f_devices_deny;

    char cgroup_path[255];
    char devices_deny[255];

    get_cgroup_path(cgroup_name, cgroup_path); 
    sprintf(devices_deny, CGROUP_DEVICES_DENY, cgroup_path);

    // check if file is accessible

    f_devices_deny = fopen(devices_deny, "w");
    // error open
    if(fprintf(f_devices_deny, mask) == -1) {
        perror("could not read file");
    }
    fclose(f_devices_deny);
}

void
allow_devices(char* mask, char* cgroup_name)
{
    FILE* f_devices_allow;

    char cgroup_path[255];
    char devices_allow[255];

    get_cgroup_path(cgroup_name, cgroup_path); 
    sprintf(devices_allow, CGROUP_DEVICES_ALLOW, cgroup_path);

    // check if file is accessible
    
    f_devices_allow = fopen(devices_allow, "w");
    // error open
    if(fprintf(f_devices_allow, mask) == -1) {
        perror("could not read file");
    }
    fclose(f_devices_allow);
}

void
add_pid_to_cgroup(pid_t pid, char* cgroup_name)
{
    FILE* f_cgroup_procs;

    char cgroup_path[255];
    char cgroup_procs[255];

    get_cgroup_path(cgroup_name, cgroup_path); 
    sprintf(cgroup_procs, CGROUP_DEVICES_PROCS, cgroup_path);

    // check if file is accessible

    f_cgroup_procs = fopen(cgroup_procs, "w");
    // error open
    if(fprintf(f_cgroup_procs, "%d", pid) == -1) {
        perror("could not add pid to cgroup");
    }
    fclose(f_cgroup_procs);
}

// this process
void
add_self_to_cgroup(char* cgroup_name)
{
       pid_t pid;
       pid = getpid();
       add_pid_to_cgroup(pid, cgroup_name);
}

/*
int main() {
    create_sub_devicecgroup("locker2");
    deny_devices(ALL_DEVICES, "locker2");
    allow_devices(READ_BLOCKDEVICES, "locker2");
    add_self_to_cgroup("locker2");

    execve("/bin/sh", NULL, NULL);
}
*/
