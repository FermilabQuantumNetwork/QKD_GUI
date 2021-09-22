Qt based user interface for FQNET TDC
=====================================

Installation
------------

First, download the timetagger software for either CentOS 7 or 8 from
[https://www.swabianinstruments.com/time-tagger/downloads/](https://www.swabianinstruments.com/time-tagger/downloads/).
On CentOS 7 you can do this via:

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
$ sudo dnf install qt5-qtbase qt5-qtbase-devel qt5-qtmultimedia qt5-qtmultimedia-devel hdf5 hdf5-devel gsl gsl-devel
```

Additionally, on CentOS 7 you will have to install a newer version of GSL:

```console
$ curl -O -L https://mirror.ibcp.fr/pub/gnu/gsl/gsl-latest.tar.gz
$ tar -xzvf gsl-latest.tar.gz
$ cd gsl-*
$ ./configure
$ make
$ sudo make install
```

Next, clone this repo and run make:

```console
$ git clone https://github.com/FermilabQuantumNetwork/QKD_GUI
$ cd QKD_GUI
$ cd source
$ make
$ ./qkd_gui
```
