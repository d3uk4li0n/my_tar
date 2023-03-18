//#define _XOPEN_SOURCE 
//#define __USE_XOPEN 
//#define _GNU_SOURCE
#ifndef UTILS_H
#define UTILS_H

#include "my_tar.h"

bool is_file(char *);
int my_strlen(char *s);
int my_strcmp(char *, char *);
void rev_string(char *, char *);
char *my_itoa(int, char *, int);
int my_atoi(char *);
char *my_strncpy(char *, char *, size_t);
char *my_strcat(char *, char *);
void *my_memset(void *, int, int);
int last_index_of(char, char *);
time_t my_str_to_time(char *);

#endif
