# Tiralabra 2024 1st period

## Docs
[Specification document](./docs/specification-document.md)\
[User manual](./docs/user-manual.md)\
Testing document coming soon..

## Weekly reports
[1st week](./docs/weekly-report-1.md)\
[2st week](./docs/weekly-report-2.md)\
[3rd week](./docs/weekly-report-3.md)\
[4th week](./docs/weekly-report-4.md)\
[5th week](./docs/weekly-report-5.md)\
[6th week](./docs/weekly-report-6.md)

## Building the program
These instructions are for Linux systems. For MacOS the instructions may be the same. CMake & Make are required for building the project.

First navigate to the folder you want this project, clone the repo and navigate there
```
$ git clone git@github.com:aatls/cooley-tukey-filter.git
$ cd cooley-tukey-filter
```
Create a build directory and move to that directory
```
$ mkdir build
$ cd build
```
Build
```
$ cmake ..
$ make
```
To launch the program, run
```
$ ./ctf
```
To build the program with the tests, run
```
$ cmake -DBUILD_TESTS=ON ..
$ make
```
To run the tests, run
```
$ ./tests
```
