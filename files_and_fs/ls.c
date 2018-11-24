#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <unistd.h>
#include <stdio.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdlib.h>

struct data {
	int size;
	char* line;
	int isdir;
	char* path;
};

struct data* create_line(DIR* dir, const struct dirent* data,
	const char* dir_name) {

	int fd = dirfd(dir);
	if (fd < 0)
		return NULL;

	const char* file_name = data->d_name;

	struct stat st;
	if (fstatat(fd, file_name, &st, 0)) {
		return NULL;
	}

	//mode
	char mode[10];
	if (S_ISDIR(st.st_mode)) { 
        mode[0] = 'd'; 
    } else if (S_ISCHR(st.st_mode)) {
        mode[0] = 'c'; 
    } else if (S_ISBLK(st.st_mode)) {
        mode[0] = 'b'; 
    } else if (S_ISLNK(st.st_mode)) {
        mode[0] = 'l';
    } else {
        mode[0] = '-';
    }
    char all_rights[] = "rwxrwxrwx";
    int mask = 1 << 8;
    for (int i = 1; i < 10; ++i) {
    	mode[i] = (st.st_mode & mask) ? all_rights[i - 1] : '-';
    	mask >>= 1;
    }

    //name and group
    struct passwd* p = getpwuid(st.st_uid);
    struct group* g = getgrgid(st.st_gid);
    char* name_;
    char* group_;
    char uid[8];
    char gid[8];
    if (p == NULL) {
    	sprintf(uid, "%i", st.st_uid);
    	name_ = uid;
    } else {
    	name_ = p->pw_name;
    }
    if (g == NULL) {
    	sprintf(gid, "%i", st.st_gid);
    	group_ = gid;
    } else {
    	group_ = g->gr_name;
    }

    //date
    static char date[15];
    strftime(date, 15, "%b %d %H:%M", localtime(&st.st_ctime));

    struct data* res_data = (struct data*)malloc(sizeof(struct data));
    if (res_data == NULL) {
    	return 0;
    }
    
    res_data->size = st.st_size;

    res_data->line = (char*)malloc(256);
    if (res_data->line == NULL) {
    	return NULL;
    }

    sprintf(res_data->line, "%s %ld %s %s %ld %s %s\n", mode, st.st_nlink,
    	name_, group_, st.st_size, date, file_name);

    //path
    char* path = malloc(strlen(dir_name) + strlen(file_name));
    if (path == NULL) {
    	return NULL;
    }
    sprintf(path, "%s/%s", dir_name, file_name);
    printf("%s\n", path);

    res_data->path = path;

    res_data->isdir = 0;
    if (S_ISDIR(st.st_mode) && strcmp(file_name, ".") && strcmp(file_name, "..")) {
    	res_data->isdir = 1;
    }

    return res_data;
} 

int compare_size(const void* a, const void* b) {
	struct data* l1 = *(struct data* const*)a;
	struct data* l2 = *(struct data* const*)b;
	return l1->size < l2->size;
}

void do_dir(char* dir_name) {

	if (strcmp(dir_name, ".")) {
        printf("%s:\n", dir_name);
    }

	DIR* dir = opendir(dir_name);
	if (dir == NULL) {
		error(2, errno, "Cannot open directory %s", dir_name);
	}

    struct data* data_array[256];
    struct dirent* dir_data = readdir(dir);
	if (errno == EBADF) {
        error(1, EBADF, "Invalid directory descriptor");
    }

    int count_data = 0;
    while (dir_data != NULL) {
    	data_array[count_data] = create_line(dir, dir_data, dir_name);
    	if (data_array != NULL) {
    		++count_data;
    	}
    	dir_data = readdir(dir);
		if (errno == EBADF) {
        	error(1, EBADF, "Invalid directory descriptor");
    	}
    }

    qsort(data_array, count_data, sizeof(struct data*), compare_size);

    for (int i = 0; i < count_data; ++i) {
    	if (data_array[i] != NULL && data_array[i]->isdir == 1) {
    		printf("%s\n", data_array[i]->line);
    	}
    }

    for (int i = 0; i < count_data; ++i) {
    	if (data_array[i] != NULL) {
    		printf("\n");
    		do_dir(data_array[i]->path);
    	}
    }

}


int main(int argc, char** argv) {
	if (argc == 1) {
		do_dir(".");
	}
	for (int i = 1; i < argc; ++i) {
		do_dir(argv[i]);
	}
	return 0;
}