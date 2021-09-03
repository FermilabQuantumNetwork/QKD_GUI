Qt based user interface for FQNET TDC
=====================================

Installation
------------

First, download the timetagger software for either CentOS 7 or 8 from
[https://www.swabianinstruments.com/time-tagger/downloads/](https://www.swabianinstruments.com/time-tagger/downloads/). On CentOS 7 you can do this via:

```console
$ curl -O -L https://www.swabianinstruments.com/static/downloads/timetagger-2.9.0.el7.x86_64.rpm
$ sudo yum install timetagger-2.9.0.el7.x86_64.rpm
```

and on CentOS 8:

```console
$ curl -O -L https://www.swabianinstruments.com/static/downloads/timetagger-2.9.0.el8.x86_64.rpm
$ sudo dnf install timetagger-2.9.0.el8.x86_64.rpm
```

Next, install the necessary libraries using yum or dnf:

```console
$ sudo dnf install qt5-qtbase qt5-qtmultimedia hdf5 hdf5-devel
```

Next, just run make:

```console
$ make
$ ./PROGRAM
```
