# Build and prepare
You need a container image in the folder `container`.
The following script downloads and extracts an `alpine` container image to the `container` folder.
`sudo` is used to extract (create) files with the correct owner and mode.
```sh
sudo bash download-and-extract-image.sh
pushd src && make && popd
```

# Use
Run the `locker` program. It does:
- create a separate cgroup (v2)
- creates namespaces (mnt)
- pivot_root into the `container` folder
- create a simple `/dev`, `/proc`
- drops into user ID, group ID to 1000, drops capabilties except `CAP_MKNOD`, `CAP_NET_BIND_SERVICE`
- executes `sh` (`busybox`)

```
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

/ $ ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host 
       valid_lft forever preferred_lft forever
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP qlen 1000
    link/ether 00:0c:29:4b:62:79 brd ff:ff:ff:ff:ff:ff
    inet 192.168.134.128/24 brd 192.168.134.255 scope global dynamic eth0
       valid_lft 1539sec preferred_lft 1539sec
    inet6 fe80::7293:7b6d:7dfc:dc4e/64 scope link 
       valid_lft forever preferred_lft forever

```
Limitations:
- the network namespace (thus the network interfaces and routes) are shared with the host.
- the user IDs and Capabilties are hardcoded
