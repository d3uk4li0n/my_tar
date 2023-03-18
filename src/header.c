#include "../include/my_tar.h"
#include "../include/utils.h"
#include "../include/header.h"

// add checksum to pieces of data
unsigned int checksum(char *data, int length){
    unsigned int sum = 0;
    for (int i = 0; i < length; i++){
        sum += *data;
        data++;
    }
    return sum;
}

int *create_bytes_offset(void){
    int b[HEADER_LENGTH] = {
        0,        NAMELEN,   MODELEN,     UIDLEN,      GIDLEN,   SIZELEN,
        MTIMELEN, CHKSUMLEN, LINKNAMELEN, TYPFLAGLEN,  TMAGLEN,  UNAMELEN,
        GNAMELEN, TVERSLEN,  DEVMAJORLEN, DEVMINORLEN, PREFIXLEN};

    int *bytes_offset = (int *)malloc(HEADER_LENGTH * sizeof(int));

    for (int i = 0; i < HEADER_LENGTH; i++) bytes_offset[i] = b[i];

    return bytes_offset;
}


void add_chksum(metadata *md)
{
    unsigned int chksum = 0;
    char *temp = md->name;
    int *bytes_offset = create_bytes_offset();

    for (int i = 0; i < HEADER_LENGTH - 1; i++){
        temp += bytes_offset[i];
        chksum += checksum(temp, bytes_offset[i + 1]);
    }
    my_itoa(chksum, md->chksum, 8);
    // printf("%s\n", md->chksum);
    free(bytes_offset);
}

void add_link_reg(metadata *md, char *path){
    struct stat lst;
    if (lstat(path, &lst) == -1){
        printf("error in lstat()");
        return;
    }

    if (S_IFLNK == (lst.st_mode & S_IFMT)){
        md->typeflag = SYMTYPE;
        size_t bufsize = (lst.st_size / sizeof(char)) + 1;
        readlink(path, md->linkname, bufsize);
        md->linkname[bufsize + 1] = '\0';
    }
    else
        md->typeflag = REGTYPE;
}

void add_typeflag(char *path, metadata *md){
    struct stat st;
    if (stat(path, &st) == -1) return;

    if (S_ISDIR(st.st_mode)) md->typeflag = DIRTYPE;
    else if (S_ISREG(st.st_mode)) add_link_reg(md, path);  // md->typeflag = REGTYPE;
    else if (S_ISLNK(st.st_mode)) md->typeflag = SYMTYPE;  // LNKTYPE
    else if (S_ISCHR(st.st_mode)) md->typeflag = CHRTYPE;
    else if (S_ISBLK(st.st_mode)) md->typeflag = BLKTYPE;
    else if (S_ISFIFO(st.st_mode)) md->typeflag = FIFOTYPE;
    // else md->typeflag = FLAGTYPE_ERR;
    else printf("%s\n", FLAGTYPE_ERR);
}

void add_uname_gname(metadata *md, struct stat st){
    struct group *gp;
    struct passwd *psw;

    gp = getgrgid(st.st_gid);
    psw = getpwuid(st.st_uid);
    my_strncpy(md->uname, gp->gr_name, my_strlen(psw->pw_name));
    my_strncpy(md->gname, psw->pw_name, my_strlen(psw->pw_name));
}

void add_major_minor(metadata *md, struct stat st){
    int major = (int)major(st.st_dev);
    int minor = (int)minor(st.st_dev);
    my_itoa(major, md->devmajor, 8);
    my_itoa(minor, md->devminor, 8);
    // printf("%s, %s\n", md->devmajor, md->devminor);
}

void add_name_prefix(metadata *md, char *path){
    int split_at = 0;

    for (int n = my_strlen(path) - 1; n >= 0; n--){
        if (path[n] == '/') break;
        split_at++;
    }

    if (split_at == my_strlen(path)){  // if file is in the current directory
        my_strncpy(md->name, path, my_strlen(path));
        md->name[my_strlen(path)] = '\0';

        // printf("Debug: name in add_name_prefix: %s\n", md->name);
        return;
    }

    my_strncpy(md->prefix, path, (my_strlen(path) - split_at));
    md->prefix[(my_strlen(path) - split_at)] = '\0';

    path += (my_strlen(path) - split_at);
    my_strncpy(md->name, path, my_strlen(path));
    md->name[my_strlen(path)] = '\0';
}

metadata *create_header(char *path, struct stat st){
    metadata *md = (metadata *)calloc(1, sizeof(metadata));
    char mode[8], uid[8], gid[8], size[12];

    my_itoa((int)st.st_mode, mode, 8);
    my_itoa((uintmax_t)st.st_uid, uid, 10);
    my_itoa((uintmax_t)st.st_gid, gid, 10);
    my_itoa((intmax_t)st.st_size, size, 10);

    add_name_prefix(md, path);
    my_strncpy(md->mode, mode, sizeof(md->mode));
    my_strncpy(md->uid, uid, sizeof(md->uid));
    my_strncpy(md->gid, gid, sizeof(md->gid));
    my_strncpy(md->size, size, sizeof(md->size));
    my_strncpy(md->mtime, ctime(&st.st_mtime), 27);  // copy \0
    add_typeflag(path, md);

    my_strncpy(md->magic, TMAGIC, TMAGLEN);
    my_strncpy(md->version, TVERSION, TVERSLEN);
    add_uname_gname(md, st);
    add_major_minor(md, st);
    add_chksum(md);

    return md;
}

void analyze_args(int argc, char **argv, args *flags){
    char c = 0, f = 0, r = 0, u = 0, t = 0, x = 0;

    for (int i = 1; i < argc; i++){
        if (argv[i][0] == '-'){
            // printf("dashy!\n");
            if (my_strlen(argv[i]) > 1){
                if (my_strlen(argv[i]) == 2){
                    if (argv[i][1] == 'c')
                        c = 1;
                    else if (argv[i][1] == 'f')
                        f = 1;
                    else if (argv[i][1] == 'r')
                        r = 1;
                    else if (argv[i][1] == 'u')
                        u = 1;
                    else if (argv[i][1] == 't')
                        t = 1;
                    else if (argv[i][1] == 'x')
                        x = 1;
                    else
                        printf("The following options are allowed: [cfrutx]\n");
                }
                else if (my_strlen(argv[i]) > 2){
                    for (int n = 1; n < my_strlen(argv[i]); n++){
                        switch (argv[i][n]){
                            case 'c':
                                c = 1;
                                break;
                            case 'f':
                                f = 1;
                                break;
                            case 'r':
                                r = 1;
                                break;
                            case 'u':
                                u = 1;
                                break;
                            case 't':
                                t = 1;
                                break;
                            case 'x':
                                x = 1;
                                break;
                            default:
                                printf("you must specify one of the: cfrutx\n");
                                break;
                        }
                    }
                }
            }
            else
                printf("dash alone is not a valid option\n");
        }
    }
    // printf("c:%d f:%d r:%d u:%d t:%d x:%d\n", c, f, r, u, t, x);
    flags->c = c;
    flags->u = u;
    flags->r = r;
    flags->x = x;
    flags->t = t;
    flags->f = f;
}
