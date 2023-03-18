#ifndef MY_TAR_H
#define MY_TAR_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <pwd.h>
#include <grp.h>

/* Values used in typeflag field.  */
#define REGTYPE  '0'            /* regular file */
//#define AREGTYPE '\0'           /* regular file */
#define LNKTYPE  '1'            /* link */
#define SYMTYPE  '2'            /* reserved */
#define CHRTYPE  '3'            /* character special */
#define BLKTYPE  '4'            /* block special */
#define DIRTYPE  '5'            /* directory */
#define FIFOTYPE '6'            /* FIFO special */
#define CONTTYPE '7'            /* reserved */
#define MODES_ARR_LEN 9

#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION "00"           /* 00 and no null */
#define TVERSLEN 2

#define FLAGTYPE_ERR "type error\n"

/* This is a dir entry that contains the names of files that were in the
dir at the time the dump was made.  */
#define GNUTYPE_DUMPDIR 'D'

/* Identifies the *next* file on the tape as having a long linkname.  */
#define GNUTYPE_LONGLINK 'K'

/* Identifies the *next* file on the tape as having a long name.  */
#define GNUTYPE_LONGNAME 'L'

/* This is the continuation of a file that began on another volume.  */
#define GNUTYPE_MULTIVOL 'M'

/* This is for sparse files.  */
#define GNUTYPE_SPARSE 'S'

/* This file is a tape/volume header.  Ignore it on extraction.  */
#define GNUTYPE_VOLHDR 'V'

/* Solaris extended header */
#define SOLARIS_XHDTYPE 'X'

/* fields length */
#define NAMELEN 100
#define MODELEN 8
#define UIDLEN 8
#define GIDLEN 8
#define SIZELEN 12
#define MTIMELEN 12
#define CHKSUMLEN 8
#define LINKNAMELEN 100
#define TYPFLAGLEN 1
#define TMAGLEN 6
#define UNAMELEN 32
#define GNAMELEN 32
#define TVERSLEN 2
#define DEVMAJORLEN 8
#define DEVMINORLEN 8
#define PREFIXLEN 155
#define HEADER_LENGTH 17 
//#define BLOCKSIZE 512 //size of a tar block

/* Bits used in the mode field, values in octal.  */
#define TSUID    04000          /* set UID on execution */
#define TSGID    02000          /* set GID on execution */
#define TSVTX    01000          /* reserved */
/* file permissions */
#define TUREAD   00400          /* read by owner */
#define TUWRITE  00200          /* write by owner */
#define TUEXEC   00100          /* execute/search by owner */
#define TGREAD   00040          /* read by group */
#define TGWRITE  00020          /* write by group */
#define TGEXEC   00010          /* execute/search by group */
#define TOREAD   00004          /* read by other */
#define TOWRITE  00002          /* write by other */
#define TOEXEC   00001          /* execute/search by other */

//int stat_modes[MODES_ARR_LEN] = {S_IREAD, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP,  S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};

//reimplementation of the posix header
typedef struct file_metadata{                      
    /* byte offset */
    char name[100];               /*   0 */
    char mode[8];                 /* 100 */
    char uid[8];                  /* 108 */
    char gid[8];                  /* 116 */
    char size[12];                /* 124 */
    char mtime[27];               /* 136 modified: ctime convert to 26 characters:   "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n"*/  
    char chksum[8];               /* 148 */
    char typeflag;                /* 156 */
    char linkname[100];           /* 157 */
    char magic[6];                /* 257 */
    char version[2];              /* 263 */
    char uname[32];               /* 265 */
    char gname[32];               /* 297 */
    char devmajor[8];             /* 329 */
    char devminor[8];             /* 337 */
    char prefix[155];             /* 345 */
    /* 500 */
}metadata;

//main argument parameters
typedef struct args{
    int c;
    int u;
    int r;
    int t;
    int x;
    int f;
} args;

int create_archive(char *);
int write_header(int, metadata *);
int close_archive(int);
void copy_file_content(char *, int);
void add_name_prefix(metadata *, char *);
void add_link_reg(metadata *, char *);
void add_typeflag(char *, metadata *);
metadata *create_header(char *, struct stat);
char *fill_perms(char *, struct stat);
void add_uname_gname(metadata *, struct stat);
void add_major_minor(metadata *, struct stat);

void list_files(char *);
void extract_archive(char *archive_name);
void update_files(int, char **, char *, int);
int check_options(int, char **, args *);

#endif
