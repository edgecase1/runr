#include "capabilities.h"
#include <linux/capability.h>
#include <linux/securebits.h>
#include <sys/capability.h>
#include <sys/prctl.h>

int
prep_cap()
{
    cap_t caps;
    int ncap = 0;
    cap_value_t cap_list[] = { CAP_MKNOD, CAP_NET_BIND_SERVICE };

    ncap = sizeof(cap_list)/sizeof(cap_list[0]); // len(cap_list)

    if (!CAP_IS_SUPPORTED(CAP_SETFCAP))
    {
        perror("no capabiltities are supported");
	return -1;
    }

    caps = cap_get_proc();
    if(caps == NULL) {
        perror("error cap_get_proc");
        return -1;
    }

    // raising inhertiable capabilties for execve
    if(cap_set_flag(caps, CAP_INHERITABLE, ncap, cap_list, CAP_SET) == -1)
    {
        perror("error cap_set_flag effective");
        cap_free(caps);
	return -1;
    }

    if(cap_set_proc(caps) == -1)
    {
        perror("error cap_set_proc");
	return -1;
    }

    if (cap_free(caps) == -1)
    {
        perror("error cap_free");
    }

    // raising ambient capabilties for execve
    for(int i=0; i < ncap; i++)
    { 
        if (prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, cap_list[i], 0, 0) == -1) {
            perror("error prctl PR_CAP_AMBIENT");
            return -1;
        }
    }

    // try to keep the capabilities after execve
    if (prctl(PR_SET_SECUREBITS, 
	      SECBIT_NO_SETUID_FIXUP) 
              == -1) {
        perror("error prctl PR_SET_SECUREBITS");
	return -1;
    }

    return 0;
}
