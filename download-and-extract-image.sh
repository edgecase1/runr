#!/bin/bash

LOCAL_FILE="alpine-minirootfs-3.9.0-x86_64.tar.gz"
TARGET_URL="http://dl-cdn.alpinelinux.org/alpine/v3.9/releases/x86_64/alpine-minirootfs-3.9.0-x86_64.tar.gz"
CONTAINER_DIR="container"

perror()
{
    echo $* >&2
    exit 1
}

[ ! -d $CONTAINER_DIR ] || perror "container directory '$CONTAINER_DIR' already present. Remove first: rm -rf $CONTAINER_DIR"
# folder should not exist at this stage
mkdir $CONTAINER_DIR || perror "error creating $CONTAINER_DIR"

echo "downlading and extracting file"
curl -o - "$TARGET_URL" | tar -xzp -C $CONTAINER_DIR || perror "error downlading file"

chmod 755 $CONTAINER_DIR

echo "Your container image can be found in '$CONTAINER_DIR'"
