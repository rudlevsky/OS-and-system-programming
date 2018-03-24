#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

// structure for files
typedef struct {
    char way[PATH_MAX];
    char name[FILENAME_MAX];
    int  size;
} st_file;

char util_name[FILENAME_MAX];
st_file *records;
int records_len = 0;

int (*cmpfunc)(st_file file1, st_file file2);

int cmpname(st_file file1, st_file file2) {
    return (strcmp(file1.name, file2.name) > 0);
}

int cmpsize(st_file file1, st_file file2) {
    return ((file1.size - file2.size) < 0);
}

// function for adding '/'
char* sls_add(char* way) {
    if (way[strlen(way) - 1] != '/')
        strcat(way, "/");
    return way;
}

// open directories 
int dirpass(const char *dir_name) {
    DIR *directory;

    if (!(directory = opendir(dir_name))) {
        fprintf(stderr, "%s: %s: %s\n", util_name, dir_name, strerror(errno));
        return -1;
    }

    struct dirent *dir_item;
    while((dir_item = readdir(directory)) != NULL) {
        char next_item[PATH_MAX];
        strcpy(next_item, dir_name);
        strcat(next_item, dir_item->d_name);
        if(strcmp(".", dir_item->d_name) == 0 ||
            strcmp("..", dir_item->d_name) == 0)
            continue;

        struct stat statbuf;
        lstat(next_item, &statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
            dirpass(sls_add(next_item));
        } else if(S_ISREG(statbuf.st_mode)) {
            st_file file_tmp;
            strcpy(file_tmp.name, dir_item->d_name);
            strcpy(file_tmp.way, next_item);
            file_tmp.size = (int) statbuf.st_size;
            records = realloc(records, (++records_len)*sizeof(st_file));
            records[records_len - 1] = file_tmp;
        }
    }
    closedir(directory);
}

void flistsort(int sort_opt){
    if (sort_opt == 1)
        cmpfunc = cmpname;
    else
        cmpfunc = cmpsize;

    int i, j;
    for (i = 0; i < records_len; i++) {
        for (j = 0; j < records_len - i - 1; j++) {
            if (cmpfunc(records[j], records[j+1])) {
                st_file file_tmp = records[j];
                records[j] = records[j+1];
                records[j+1] = file_tmp;
            }
        }
    }
}

int main(int argc, char const *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "%s\n","Wrong number of arguments");
        return -1;
    }

    char dist_path[PATH_MAX];
    realpath(argv[3], dist_path);
    sls_add(dist_path);

    char sort_opt = atoi(argv[2]);
    if (sort_opt != 1 && sort_opt != 2) {
        fprintf(stderr,"%s\n","Wrong sort option (Must be: 1 or 2)");
        return -1;
    }

    char initdir[PATH_MAX];
    strcpy(initdir, argv[1]);
    dirpass(sls_add(initdir));

    flistsort(sort_opt);

    if (mkdir(dist_path, 0777) == -1 && errno != EEXIST) {
        fprintf(stderr, "%s: %s\n", dist_path, strerror(errno));
        return -1;
    }

    // add numbers
    int i, j;
    for(i = 0; i < records_len; i++) {
       
        char add_index[5];
        sprintf(add_index,"%d",i);
        strcat(records[i].name,add_index);
       
        char dist_file[PATH_MAX + 1];
        strcpy(dist_file, dist_path);
        strcat(dist_file, records[i].name);

        if (symlink(records[i].way, dist_file) == -1) 
           fprintf(stderr, "%s: %s\n",dist_file, strerror(errno));
    }
    return 0;
}
