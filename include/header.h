#ifndef HEADER_H
#define HEADER_H

void analyze_args(int, char **, args *);
metadata *create_header(char *, struct stat);
void print_header(metadata *);
metadata *init_header(metadata *);
//add checksum to pieces of data
unsigned int checksum(char *, int);
void add_chksum(metadata *);
//check wether it's regular file or link 
//if it's link, fill the linkname field
void add_link_reg(metadata *, char *);
void add_typeflag(char *, metadata *);
void add_uname_gname(metadata *, struct stat);
void add_major_minor(metadata *, struct stat);
void add_name_prefix(metadata *, char *);
int *create_bytes_offset(void);

#endif
