#define __USE_XOPEN 
#define _GNU_SOURCE
#include "../include/my_tar.h"
#include "../include/utils.h"
#include "../include/header.h"

bool is_file(char *filename){
    struct stat st;
    if(stat(filename, &st) != 0){
        //printf("error: %s doesn't exist\n", filename);
        return false;
    }
    if((st.st_mode & S_IFMT) == S_IFDIR) return false;
    return true;
}

int my_strlen(char *s){
    int l = 0;
    if(s == NULL) return -1;
    while(s[l] != '\0') l++;

    return l;
}

int my_strcmp(char *s1, char *s2){
    int flag = 0;
    while(*s1 != '\0' || *s2 != '\0'){
        s1++;
        s2++;
    if((*s1 == '\0' && *s2 != '\0') || (*s1 != '\0' && *s2 == '\0')||
    (*s1 != *s2)){
        flag = 1;
        break;
    }
    else flag = 0;
    }
    return flag;
}

void rev_string(char *begin, char *end){
    char aux;

    while(end>begin)
        aux=*end, *end--=*begin, *begin++=aux;
}


char *my_itoa(int value, char *str, int base){
    static char chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char *wstr = str;
    int sign;

    if((sign=value) < 0) 
        value = -value;

    do{
        *wstr++ = chars[value % base];
    }while(value /= base);

    if(sign < 0) *wstr++ = '-';
    *wstr = '\0';
    rev_string(str, wstr-1);
    return wstr;
}

//convert string to integer, base 10
int my_atoi(char *str){
    int value = 0;
    char* temp = str;

    while (*temp != '\0'){
        value = value * 10 + (*temp - '0');
        temp++;
    }

    return value;
}

char *my_strncpy(char *dst, char *src, size_t n){
    size_t i;

    for(i=0; i<n && src[i] != '\0';i++)
        dst[i] = src[i];
    dst[i] = '\0';

    return dst;
}

char *my_strcat(char *src, char *dst){
    int dst_len = my_strlen(dst);

    for(int i=0;src[i] != '\0';i++){
        dst[dst_len] = src[i];
        dst_len += 1;
    }
    dst[dst_len] = '\0';
    return dst;
}

//string to time
time_t my_str_to_time(char* str){
    struct tm tm;
    strptime(str, "%a %b %d %H:%M:%S %Y", &tm);
    return mktime(&tm); 
}
