
#define ALL_DEVICES "a *:* rwm"
#define READ_BLOCKDEVICES "b *:* r"

void deny_devices(char* mask, char* cgroup_name);
void allow_devices(char* mask, char* cgroup_name);
void add_self_to_cgroup(char* cgroup_name);
void add_pid_to_cgroup(pid_t pid, char* cgroup_name);
void create_sub_devicecgroup(char* cgroup_name);
