# Oracc II Installation Guide

This file documents the process and requirements for installing Oracc
II--Oracc I is still required as of 2026-03-02 but it is being phased
out and should currently be installed after installing Oracc II.

## Installation Components

- Prerequisites -- software needed to build and run Oracc

- The `oracc' User -- Oracc can be installed as multi user or single user

- Directories -- required directories for Oracc operation

- Apache -- an Apache config with default settings for key files

- Cron -- cron jobs for updating server components and keeping the installation clean

- Configuration -- the install uses Gnu Configure to set things up for the build

- Build -- after configuration Oracc should build and install with `make ; make install'

- Core -- Oracc treats OSL, PCSL, CDLI, QCAT, and XCAT as required projects

