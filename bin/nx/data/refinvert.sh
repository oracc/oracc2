#!/bin/sh
sort $1 | tr ' ' '_' | concord '-fwtcl*' | tr '_' ' ' >$2
