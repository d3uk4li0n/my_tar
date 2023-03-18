#include "../include/utils.h"
#include "../include/my_tar.h"
#include "../include/header.h"

int create_archive(char *archive_name){
    // int fd = open(archive_name, O_CREAT|O_WRONLY); //S_IRUSR|S_IWUSR);
    int fd = open(archive_name, O_CREAT | O_RDWR | O_TRUNC);
    chmod(archive_name, TUREAD | TUWRITE | TGREAD | TOREAD);
    if (fd == -1){
        printf("error creating %s archive\n", archive_name);
        return -1;
    }
    return fd;
}

// writes metadata file info into archive file descriptor
int write_header(int fd, metadata *md){
    // printf("size of metadata = %d\n", sizeof(metadata));
    int bytes;

    if ((bytes = write(fd, md, sizeof(metadata))) != sizeof(metadata)){
        printf("error writing header to archive, wrote %d\n", bytes);
        return -1;
    }
    else
    {
        // printf("successfully wrote file data to archive\n");
        return 0;
    }
    return 0;
}

int close_archive(int fd){
    if (close(fd) == 0){
        // printf("successfully closed archive\n");
        return 0;
    }
    printf("error closing archive");
    return -1;
}

int main(int argc, char **argv){
    // printf("%d, %s\n\n", argc, argv[0]);
    int i;
    char filename[100];  // tar file
                         // archive file descriptor
    int archive_fd;

    args flags;
    analyze_args(argc, argv, &flags);

    // validate arguments
    if (check_options(argc, argv, &flags) == 0){
        return 0;
    }

    int archived_file_index = 2;
    if (flags.f){
        archived_file_index = 3;
    }

    char *tar_file = NULL;
    if (flags.f){
        tar_file = argv[2];
    }
    else{
        scanf("%s", filename);
        tar_file = &filename[0];
    }

    // printf("c:%d f:%d r:%d u:%d t:%d x:%d\n", flags[0],
    //    flags[1], flags[2], flags[3], flags[4], flags[5]);

    if (flags.c){            
        if (flags.f){
            char *tar_file = argv[2];
            archive_fd = create_archive(tar_file);
            close_archive(archive_fd);
        }

        // open to write
        if (flags.f){
            char *tar_file = argv[2];
            archive_fd = open(tar_file, O_WRONLY);
        }
        else{
            archive_fd = 1;  // stdout
        }

        for (i = archived_file_index; i < argc; i++){
            char *src_file = argv[i];  // file in tar file
            struct stat st;
            stat(src_file, &st);
            metadata *md = create_header(src_file, st);

            write_header(archive_fd, md);
            // copy content of file
            copy_file_content(src_file, archive_fd);
            free(md);
        }

        if (flags.f){
            close_archive(archive_fd);
        }
    }
    else if (flags.t){  // list files
        list_files(tar_file);
    }
    else if (flags.x){  // extract
        extract_archive(tar_file);
    }
    else if (flags.r){ 
        // open to append
        archive_fd = open(tar_file, O_WRONLY | O_APPEND);

        if (archive_fd == -1){
            printf("error opening: %s for appending\n", tar_file);
        }
        else{
            for (i = archived_file_index; i < argc; i++){
                char *src_file = argv[i];  // file in tar file

                // printf("srcFile: %s \n", srcFile);

                struct stat st;
                stat(src_file, &st);
                metadata *md = create_header(src_file, st);
                write_header(archive_fd, md);
                // copy content of file
                copy_file_content(src_file, archive_fd);
                free(md);
            }
        }

        close_archive(archive_fd);
    }
    else if (flags.u){  // update if new file has modification date is greater than existing file
        // update files
        update_files(argc, argv, tar_file, archived_file_index);
    }

    return 0;
}

// update files
void update_files(int argc, char **argv, char *tar_file, int archived_file_index){
    int i;
    ssize_t bytes;

    int archive = open(tar_file, O_RDONLY);

    if (archive == -1){
        printf("error opening: %s for updating\n", tar_file);
        return;
    }

    // create a temp file
    int archive_temp = create_archive("temp.archive.tar");

    // printf("archive: %d\n", archive);
    close_archive(archive_temp);

    // printf("Debug: name: %s\n", md->name);

    archive_temp = open("temp.archive.tar", O_WRONLY);

    while (1){
        // read the meta header
        metadata md;
        bytes = read(archive, &md, sizeof(metadata));
        if (bytes != sizeof(metadata)){
            break;
        }

        int need_modify = 0;  // need to update existing file

        // check with new file
        for (i = archived_file_index; i < argc; i++){
            char *src_file = argv[i];  // file in tar file

            // printf("src_file: %s \n", src_file);

            if (my_strcmp(md.name, src_file) == 0){  // same name
                // printf("same name\n");
                struct stat st2;
                stat(src_file, &st2);
                metadata *md2 = create_header(src_file, st2);

                // compare modification time
                time_t t1 = my_str_to_time(md.mtime);
                time_t t2 = my_str_to_time(md2->mtime);

                // printf("old %s \n", md.mtime);
                // printf("new %s \n", md2->mtime);

                // printf("old %s \n", ctime(&t1));
                // printf("new %s \n", ctime(&t2));

                // printf("difftime %lf \n", difftime(t1, t2));

                if (difftime(t1, t2) < 0){
                    need_modify = 1;

                    write_header(archive_temp, md2);

                    // copy content of file
                    copy_file_content(src_file, archive_temp);
                }
                free(md2);
                break;
            }
        }

        if (need_modify == 0){
            write_header(archive_temp, &md);
        }

        // read (and write file content)
        char buffer[1024];
        int bufsize = 1024;

        int file_size = my_atoi(md.size);
        int num_bytes = file_size;
        if (num_bytes > bufsize){
            num_bytes = bufsize;
        }
        int total_bytes = 0;

        while ((bytes = read(archive, buffer, num_bytes))){
            if (need_modify == 0){
                if (write(archive_temp, &buffer, bytes) != (int)bytes){
                    printf("error while copying file contents\n");
                    return;
                }
            }

            // check if read all files
            total_bytes += bytes;
            if (total_bytes == file_size){
                break;
            }

            num_bytes = file_size - total_bytes;
            if (num_bytes > bufsize){
                num_bytes = bufsize;
            }
        }
    }
    close_archive(archive);
    close_archive(archive_temp);

    // delete archive
    // rename file
    unlink(tar_file);
    rename("temp.archive.tar", tar_file);
}

void extract_archive(char *archive_name){
    // args: archive name, name of file to write to
    int fd = open(archive_name, O_RDONLY);
    if (fd == -1){
        printf("error opening: %s for reading\n", archive_name);
        return;
    }

    ssize_t bytes;

    while (1){
        // read the meta header
        metadata md;
        bytes = read(fd, &md, sizeof(metadata));
        if (bytes != sizeof(metadata)){
            break;
        }

        int wr = open(md.name, O_WRONLY | O_CREAT);  // create files
        if (wr == -1){
            printf("error opening: %d\n", wr);
            return;
        }
        char buffer[1024];
        int bufsize = 1024;

        int file_size = my_atoi(md.size);
        int num_bytes = file_size;
        if (num_bytes > bufsize){
            num_bytes = bufsize;
        }
        int total_bytes = 0;

        while ((bytes = read(fd, buffer, num_bytes)))
        {
            if (write(wr, &buffer, bytes) != (int)bytes)
            {
                printf("error while copying file contents\n");
                return;
            }

            // check if read all files
            total_bytes += bytes;
            if (total_bytes == file_size){
                break;
            }

            num_bytes = file_size - total_bytes;
            if (num_bytes > bufsize){
                num_bytes = bufsize;
            }
        }

        // close file
        close(wr);
    }

    // printf("Debug: name: %s\n", md.name);

    close_archive(fd);
}

// copies content of file into archive
// arguments are file name and fd of archive
void copy_file_content(char *file_name, int ad){
    ssize_t bytes_read;
    char buffer[1024];
    size_t bufsize = (size_t)sizeof(buffer);

    int f = open(file_name, O_RDONLY);
    if (f == -1){
        printf("error opening %s for reading\n", file_name);
        return;
    }

    while ((bytes_read = read(f, buffer, bufsize)) > 0){
        // buffer[bytes_read] = '\0';
        if (write(ad, &buffer, bytes_read) != (int)bytes_read){
            printf("error copying file content\n");
            return;
        }
    }
    return;
}

/*list files in archive file*/
void list_files(char *archive_name){
    int fd = open(archive_name, O_RDONLY);
    if (fd == -1){
        printf("error open %s archive\n", archive_name);
        return;
    }

    metadata md;
    while (read(fd, &md, sizeof(metadata)) == sizeof(metadata)){
        write(1, md.name, my_strlen(md.name));
        write(1, "\n", 1);

        // read files
        int size = my_atoi(md.size);
        lseek(fd, size, SEEK_CUR);
    }

    close_archive(fd);
}

int check_options(int argc, char **argv, args *flags){
    int len;

    if (flags->c && flags->t){
        printf("-t and -c cannot be used simoultaneously\n");
        return 0;
    }
    if (flags->c && flags->x){
        printf("-c and -x cannot be used simoultaneously\n");
        return 0;
    }
    if (flags->c && flags->r){
        printf("-c and -r cannot be used simoultaneously\n");
        return 0;
    }
    if (flags->c && flags->u){
        printf("-c and -u cannot be used simoultaneously\n");
        return 0;
    }
    if (flags->u && flags->r){
        printf("-u and -r cannot be used simoultaneously\n");
        return 0;
    }
    if (flags->u && flags->x){
        printf("-u and -x cannot be used simoultaneously\n");
        return 0;
    }
    if (flags->u && !flags->f){
        printf("cannot update to stdout\n");
        return 0;
    }
    if (flags->u && flags->t){
        printf("-u and -u cannot be used simoultaneously\n");
        return 0;
    }
    if (flags->r && !flags->f){
        printf("cannot append to stdout\n");
        return 0;
    }

    // need archive file name
    // check file name ending with .tar
    if (flags->f){
        if (argc < 3){
            printf("no file or directory specified\n");
            return 0;
        }
        if (argc >= 3){
            for (int i = 3; i < argc; i++){
                if (!is_file(argv[i])){
                    printf("error: %s no such file or direcctory\n", argv[i]);
                    return 0;
                }
            }
        }
        else{
            len = my_strlen(argv[2]);
            //.tar ?
            if (len < 4 || argv[2][len - 1] != 'r' || argv[2][len - 2] != 'a' ||
                argv[2][len - 3] != 't' || argv[2][len - 4] != '.'){
                printf("No .tar found\n");
                return 0;
            }
        }
    }

    if (!flags->c && !flags->u && !flags->r && !flags->t && !flags->x){
        printf("flag is required\n");
        return 0;
    }

    return 1;
}

