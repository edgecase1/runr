
#include <sys/stat.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include "cgroups.h"

#define CGROUP_BASE "/sys/fs/cgroup/user.slice/%s"
#define CGROUP_PROCS "%s/cgroup.procs"

// is /proc mounted?
// is /sys mounted?
// is /sys/fs/cgroup

int
get_cgroup_path(char* cgroup_name, char* cgroup_path)
{
    sprintf(cgroup_path, CGROUP_BASE, cgroup_name); 
}

void
create_sub_devicecgroup(char* cgroup_name)
{
    char subgroup_path[255];
    get_cgroup_path(cgroup_name, subgroup_path); 

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
    get_cgroup_path(cgroup_name, subgroup_path); 

    // check if file exists
    if(rmdir(subgroup_path) == -1)
    {
        perror("error deleting subgroup");
    }
}

void // TODO int
add_pid_to_cgroup(pid_t pid, char* cgroup_name)
{
    FILE* f_cgroup_procs;

    char cgroup_path[255];
    char cgroup_procs[255];

    get_cgroup_path(cgroup_name, cgroup_path); 
    sprintf(cgroup_procs, CGROUP_PROCS, cgroup_path);

    f_cgroup_procs = fopen(cgroup_procs, "w");
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
