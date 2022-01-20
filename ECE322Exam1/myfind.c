#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <string.h> 
#include <dirent.h> 
#include <pwd.h> 
#include <grp.h> 
#include <time.h> 
#include <sys/types.h> 
#include <sys/stat.h>

//Phase 3: Write a C Program to search for files

int printFInfo(char *path, char *fname) {

	char *filename = NULL;
	struct stat finfo;
	struct passwd *pw = NULL;
	struct group *gr = NULL;

	filename = (char *)malloc(sizeof(char)*strlen(path)+strlen(fname)+2);
	sprintf(filename,"%s/%s", path, fname);

	if (lstat(filename, &finfo) == 0){

        char *dname = (char *)malloc(sizeof(char)*strlen(fname)+2);
        char ch = "regular";
        if (S_ISDIR(finfo.st_mode)){
            
            ch = "directory";
        }
        else if (S_ISREG(finfo.st_mode)){
            if ((finfo.st_mode & S_IXUSR) == S_IXUSR){
              
                ch = "executable";
            }
        }
        else if(S_ISLNK(finfo.st_mode)){
            
            ch= "link";
        }
        else if(S_ISFIFO(finfo.st_mode)){
           
            ch= "fifo";
        }
        sprintf(dname,"%s%c", fname, ch);
    
        printf("*****************************************************************************\n");
        printf("path: ");
        printf("\t%s", path);
        printf("/");
        printf("%s", fname);
        printf("\n");
        printf("type: ");
        printf("%s", ch);
        printf("\n");
        printf("size: ");
        printf("%ld ", finfo.st_size);
        printf("bytes");
        printf("\n");
        printf("Blks: ");
        printf("\t%ld", finfo.st_blocks);
        printf(" blocks");
        printf("\n");
        printf("BLSz: ");
        printf("\t%ld", finfo.st_blksize);
        printf(" bytes");
        printf("\n");
        printf("owner: ");
        pw = getpwuid(finfo.st_uid);
        gr = getgrgid(finfo.st_gid);
        printf("\t%s", pw->pw_name);
        printf("\n");
        printf("grup: ");
        printf("\t%s", gr->gr_name);
        printf("\n");
        printf("AccT: ");
        printf("\t%s", ctime(&finfo.st_atime));
        printf("ModT: ");
        printf("\t%s", ctime(&finfo.st_mtime));//for rabbit: st_mtim    ctime(&finfo.st_mtim.tv_sec)); //
        printf("ChgT: ");
        printf("\t%s", ctime(&finfo.st_ctime));
        printf("*****************************************************************************\n\n");
        //printf("%ld", finfo.st_mtime);
        //printf("\n");
        //printf("\n");
        free(dname);
	}
	free(filename);
	return 0;
}

int searching(char *path, char *keyword){
DIR *dirp=NULL;
struct dirent *d=NULL;
//char*path=strdup(folder);
dirp=opendir(path);
d = readdir(dirp);
    if(dirp==NULL){
        fprintf(stderr,"Bad path: %s\n", path);
        exit(0);
		return -1;
    }
    else{
        int counter = 0;
        while((d=readdir(dirp))!= NULL){
            char *filename = NULL;
            struct stat finfo;
            filename = (char *)malloc(sizeof(char)*strlen(path)+strlen(d->d_name)+2);
            
            char *temp=NULL;
            temp = (char *)malloc(sizeof(char)*strlen(path)+strlen(d->d_name)+2);
            sprintf(temp,"%s/%s", path, d->d_name);
            lstat(temp, &finfo);
            if(S_ISDIR(finfo.st_mode)){
                if(d->d_name[0]!='.'){
                    printf("%s \n",temp);
                    searching(temp,keyword);
                }
            }
            else{
                if(strstr(d->d_name,keyword)!=NULL){
                    printFInfo(path,d->d_name);
                }
            }
        }
    }
return 0;
}

int main(int argc, char *argv[]){

    char * path;
    char * filename;
    if(argc<3){
        //fprintf(stderr,"usage: %s <path_to_search> <filename>\n",argv[0]);
        fprintf(stderr,"usage: %s <path_to_search> <partial_or_whole_filename_to_find>\n",argv[0]);
        exit(0);
        return -1;
    }
    path=strdup(argv[1]);
    filename=strdup(argv[2]);
    searching(path,filename);

    return 0;
}