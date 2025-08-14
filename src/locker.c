
#include <sys/types.h>
#include <unistd.h>
#include <grp.h>

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


#include <stdlib.h>
#include <linux/sched.h>    /* Definition of struct clone_args */
#include <sched.h>          /* Definition of CLONE_* constants */
#include <sys/syscall.h>    /* Definition of SYS_* constants */

#include "locker.h"
#include "cgroups.h"
#include "capabilities.h"

/*
 * a lame docker-like container implementation
 * 
 * lots of inspiration from
 * https://man7.org/linux/man-pages/man2/pivot_root.2.html
 *
 */

// prepares the mount points, pivots and runs the target program
static int 
container_setup(void *arg)
{
    char **args = arg;
    char* new_root = "container";
    char* put_old = "oldroot";
    char path[PATH_MAX];

    delete_sub_devicecgroup("locker");
    // create a separate cgroups
    create_sub_devicecgroup("locker");
    //allow_devices(ALL_DEVICES, "locker");

    //deny_devices(ALL_DEVICES, "locker");
    //allow_devices(READ_BLOCKDEVICES, "locker");
    add_self_to_cgroup("locker");
    
    // mknod TODO but dev not mounted yet
    /*
    dev_t dev = makedev(8, 1); 
    if(mknod("/dev/sda1", S_IFBLK|0660, dev) != 0)
    {
        perror("mknod");
    }*/

    // stop propagation of mount events to the initial namespace
    if (mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) == 1)
    {
        perror("mount-MS_PRIVATE");
        exit(1);
    }
    // ensure that 'new_root' is a mount point
    if (mount(new_root, new_root, NULL, MS_BIND, NULL) == -1)
    {
        perror("mount-MS_BIND");
        exit(1);
    }

    /* at this point we can spawn a shell and we have a private mnt namespace */

    // TODO DEBUG bind dev and pts
    /*if(mount("/dev", "container/dev", 0, MS_BIND, 0) != 0) 
    {
        perror("bind dev");
    }*/

    // pivot root to the target directory
    snprintf(path, sizeof(path), "%s/%s", new_root, put_old); // concat
    mkdir(path, 0777);
    if(pivot_root(new_root, path) == -1)
    {
        perror("error pivot root");
        exit(1);
    }
    // Switch the current working directory to /
    if (chdir("/") == -1) 
    {
        perror("error chdir to new_root");
        exit(1);
    }

    // we can mount mount -t tmpfs
    // inspiration from docker strace

    // /dev
    /*
    if(mount("dev", "/dev", "tmpfs", MS_NOSUID|MS_STRICTATIME, "mode=755,size=65536k") != 0) 
    {
        perror("error mounting dev");
    }
    */

    mkdir("/dev/pts", 0755);
    // /dev/pts
    if(mount("pts", "/dev/pts", "devpts", MS_NOSUID|MS_NOEXEC, "newinstance,ptmxmode=0666,mode=0755") != 0) 
    {
        perror("error mounting devpts");
    }
    mkdir("/dev/shm", 0755);
    if(mount("shm", "/dev/shm", "tmpfs", MS_NOSUID|MS_NODEV|MS_NOEXEC, "mode=1777,size=67108864") != 0) 
    {
        perror("error mounting devshm");
    }
    // /proc
    if(mount("proc", "/proc", "proc", MS_NOSUID|MS_NODEV|MS_NOEXEC, NULL) != 0) 
    {
        perror("error mounting proc");
    }
    // /sys
    if (mount("sysfs", "/sys", "sysfs", MS_RDONLY|MS_NOSUID|MS_NODEV|MS_NOEXEC, 0) != 0)
    {
        perror("error mounting sys");
    }

    // tmpfs on /sys/fs/cgroup type tmpfs (ro,nosuid,nodev,noexec,mode=755)
    /*
    mkdir("/sys/fs/cgroup", 0755);
    if (mount("cgroup2", "/sys/fs/cgroup", "cgroup2", MS_NOSUID|MS_NODEV|MS_NOEXEC, 0) != 0)
    {
        perror("error mounting cgroup");
    }*/
    
    ///dev/mqueue



    // umount -l /oldroot
    if (umount2(put_old, MNT_DETACH) == -1)
    {
        perror("umount2");
    }
    if (rmdir(put_old) == -1)
    {
        perror("rmdir oldroot");
    }

    /* capabilties 
    if(drop_cap_dangerous() == -1)
    {
        perror("error cap");
    }*/

    // set user mapping in namespace (TODO)
    prep_cap();

    int uid = 1000;
    int gid = 1000;
    printf("resuid to %d %d\n", uid, gid);
    // dropping supplementary groups
    if(setgroups(0, NULL) == -1) // no groups
    {
        perror("error setgroups");
        exit(1);
    }
    //setuid(1000);
    //setgid(1000);
    if(setreuid(uid, uid) == -1)
    {
        perror("error resuid");
        exit(1);
    }
    if(setregid(gid, gid) == -1)
    {
        perror("error resgid");
        exit(1);
    }


    printf("now running as uid=%d, euid=%d\n", getuid(), geteuid());
    
    // exec the target process
    //execv("/busybox-x86_64", &args[0]);
    char *argv[] = {"/bin/busybox", "sh", 0};
    printf("execve into '%s'\n", argv[0]); 
    execv(argv[0], argv);
}

// creates a namespace and passes control to container_setup
int
main(int argc, char *argv[])
{
    struct stat st = {0};

    int child_pid = -1;
    // extract the image
    // download alpine
    // check existing target
    if (stat("container", &st) != 0) {
        perror("directory 'container' not present. Download an image and extract it to container/ !");
        exit(1);
    }

    // create a new namespace
    // unshare(CLONE_NEWNS|CLONE_NEWPID) is implemented via clone
    // clone is like fork, but more control
    // we also have to care to create a stack 
    char *stack = mmap(NULL,
                       STACK_SIZE,
                       PROT_READ | PROT_WRITE,  // rw-
                       MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, // private stack, not shared
                       -1,
                       0);
    if (stack == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }
    if ((child_pid = clone(container_setup,        // called routine          
                           stack + STACK_SIZE,
                           CLONE_NEWNS | CLONE_NEWCGROUP | SIGCHLD, 
                           NULL)) == -1)
    {
        perror("clone");
        exit(1);
    }
    printf("child pid is %d\n", child_pid);
    /* Parent falls through to here; wait for child */

    // father tasks: wait until the child dies and clean up
    if (wait(NULL) == -1)
    {
        perror("error waiting");
        exit(1);
    }
    // clean up
    printf("\nfather cleans up.\n");
    // delete cgroup

    printf("exit\n");
    exit(EXIT_SUCCESS);
}
