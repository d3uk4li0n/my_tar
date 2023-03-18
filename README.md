# my_tar
a reimplementation of the unix archiving tool

## Description
-c create new archives  
-t list archive content  
-f specify archive to read or write  
-r new entries are appended to the archive  
-u new entries are added only if they have a modification date newer than the corresponding entry in the archive  
-x extract archive (replaces file if it already exists)  

the main file my_tar.c does most of the heavy lifting, implementing all the features  
implementation of the original header: https://www.gnu.org/software/tar/manual/html_node/Standard.html  

## Authorized function(s)
chmod  
close(2)  
free(3)  
fstat(2)  
getpwuid  
getgrgid  
getxattr  
listxattr  
lseek(2)  
major  
malloc(3)  
minor  
mkdir(2)  
open(2)  
opendir  
read(2)  
readdir  
readlink  
stat(2)  
lstat(2)  
symlink  
unlink(2)  
time  
ctime  
utime  
write(2)  

## Installation
make  
make clean (to clean up after usage)

## Usage
my_tar -cf file.tar test1.txt test2.txt  
my_tar -tf file.tar  
my_tar -rf file.tar test3.txt  
my_tar -xf file.tar  

Enjoy!
