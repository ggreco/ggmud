#!/bin/bash

# sample test script to install ggmud in linux

if [ `whoami` != "root" ]; then
    echo "[1;6;31mYou have to execute this script as root[0m"
    exit 1
fi

xdg-icon-resource install --size 48 --novendor ggmud.png 
xdg-desktop-menu install --novendor ggmud.desktop 

cp ggmud /usr/bin/
cp gg_help.txt /usr/bin/
cp leu/* /usr/bin/
