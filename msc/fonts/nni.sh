#!/bin/sh
if [ ! -r Nabuninuaihsus/Nabuninuaihsus.ttf ]; then
    git clone https://github.com/eggrobin/Nabuninuaihsus.git
else
    (cd Nabuninuaihsus ; git pull)
fi
