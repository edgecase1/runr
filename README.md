# RunR
A custom container runtime inspired by RunC/Crun to learn about containers.
This container runtime works like `chroot container/` but does a pivot_root instead, creates a mnt namespace, mounts proc/sys/dev and drops privileges.
It isn't complete, but enough to get the gist of containers and to play with it on a managable level.

## Build
```sh
apt install libcap-dev
pushd src && make && popd
```

## Get a Container 
The following script downloads and extracts an `alpine` container image to the `container` folder.
To extract and create files with the correct owner and mode `sudo` is used.
```sh
sudo bash download-and-extract-image.sh
```

## Run
You need a container image in the folder `container` (created in the previous step).
Run the `locker` program.
```sh
sudo ./src/locker

child pid is 1770436
resuid to 1000 1000
now running as uid=1000, euid=1000
execve into '/bin/busybox'
/ $ ls
bin    dev    etc    home   lib    media  mnt    opt    proc   root   run    sbin   srv    sys    tmp    usr    var
/ $ id
uid=1000 gid=1000
/ $ cat /proc/self/status|grep Cap
CapInh: 0000000808000400
CapPrm: 0000000008000400
CapEff: 0000000008000400
CapBnd: 000001ffffffffff
CapAmb: 0000000008000400
```
This invocation did:
- create a separate cgroup (v2)
- creates namespaces (mnt)
- pivot_root into the `container` folder
- create a simple `/dev`, `/proc`
- drops into user ID, group ID to 1000, drops capabilties except `CAP_MKNOD`, `CAP_NET_BIND_SERVICE`
- executes `sh` (`busybox`)

Limitations:
- the network namespace (thus the network interfaces and routes) are shared with the host.
- the user IDs and Capabilties are hardcoded
- the container folder is hardcoded
