#!/bin/bash

LOCAL_FILE="alpine-minirootfs-3.9.0-x86_64.tar.gz"
TARGET_URL="http://dl-cdn.alpinelinux.org/alpine/v3.9/releases/x86_64/alpine-minirootfs-3.9.0-x86_64.tar.gz"
CONTAINER_DIR="container"

echo "downlading alpine"
if [ ! -f $LOCAL_FILE ] ; then
    curl -o "$LOCAL_FILE" "$TARGET_URL"
    if [ $? -ne 0 ] ; then
        echo "error downlading file" >&2
	exit 1
    fi
fi

echo "extracting to container"
if [ -f $LOCAL_FILE ] ; then
     if [ -d $CONTAINER_DIR ] ; then
         echo "container directory '$CONTAINER_DIR' already present. Remove first: rm -rf $CONTAINER_DIR"
	 exit 1
     fi
     mkdir $CONTAINER_DIR
     if [ $? -ne 0 ] ; then
         echo "error creating $CONTAINER_DIR"
	 exit 1
     fi
     tar -xzpf $LOCAL_FILE -C $CONTAINER_DIR
     if [ $? -ne 0 ] ; then
         echo "warnings while extracting '$LOCAL_FILE'. mknod warnings are ok."
     fi 
     chmod 755 $CONTAINER_DIR
fi

echo "Your container image can be found in '$CONTAINER_DIR'"
