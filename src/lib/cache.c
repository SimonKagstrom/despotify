#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "util.h"
#include "cache.h"

char cache_directory[PATH_MAX];
char cache_filename[PATH_MAX];

bool cache_init(){
    char *ptr;

    if((ptr = getenv("XDG_CACHE_HOME")) != NULL){
        snprintf(cache_directory, PATH_MAX, "%s/despotify", ptr);
    }
    else{
        DSFYDEBUG("Error getting $XDG_CACHE_HOME. Trying $HOME...\n");

        if((ptr = getenv("HOME")) != NULL){
            snprintf(cache_directory, PATH_MAX, "%s/.cache/despotify", ptr);
        }
        else{
            DSFYDEBUG("Error getting $HOME.\n");

            return false;
        }
    }

    if(mkdir(cache_directory, 0755) != 0){
        if(errno == EEXIST){
            DSFYDEBUG("Cache directory already exists.\n");

            return true;
        }

        DSFYDEBUG("Error creating cache directory. errno = %d\n", errno);

        return false;
    }

    return true;
}

void cache_clear(){
    DIR *dir = opendir(cache_directory);
    struct dirent *dirp;

    if(!dir){
        DSFYDEBUG("Error opening cache directory.\n");

        return;
    }

    /* Loop over files. */
    while((dirp = readdir(dir)) != NULL){
        if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")){
            continue;
        }

        /* Build cache filename. */
        snprintf(cache_filename, PATH_MAX, "%s/%s", cache_directory, dirp->d_name);

        remove(cache_filename);
    }
}

bool cache_contains(unsigned char *id){
    /* Build cache filename. */
    snprintf(cache_filename, PATH_MAX, "%s/%s", cache_directory, id);

    /* Try to open file for reading. */
    FILE *file = fopen(cache_filename, "r");

    if(file){
        fclose(file);

        return true;
    }

    return false;
}

unsigned char *cache_load(unsigned char *id, unsigned int *size){
    unsigned char *data;
    unsigned long fsize;

    /* Build cache filename. */
    snprintf(cache_filename, PATH_MAX, "%s/%s", cache_directory, id);

    /* Try to open file for reading. */
    FILE *file = fopen(cache_filename, "r");

    if(!file){
        DSFYDEBUG("Error opening file in read-mode.\n");

        return NULL;
    }

    /* Determine file size. */
    fseek(file, 0, SEEK_END);
    fsize = ftell(file);
    fseek(file, 0L, SEEK_SET);

    /* Allocate memory. */
    data = (unsigned char *)malloc(fsize);

    if(!data){
        DSFYDEBUG("Error allocating memory for cache data.\n");

        return NULL;
    }

    /* Read data into memory. */
    if(fread(data, 1, fsize, file) != fsize){
        free(data);

        DSFYDEBUG("Error reading cache data.\n");

        return NULL;
    }

    fclose(file);

    if(size){
        *size = fsize;
    }

    return data;
}

void cache_remove(unsigned char *id){
    /* Build cache filename. */
    snprintf(cache_filename, PATH_MAX, "%s/%s", cache_directory, id);

    /* Remove cache file. */
    if(remove(cache_filename) != 0){
        DSFYDEBUG("Error removing cache file.\n");
    }
}

void cache_store(unsigned char *id, unsigned char *data, unsigned int size){
    /* Build cache filename. */
    snprintf(cache_filename, PATH_MAX, "%s/%s", cache_directory, id);

    /* Try to open file for writing. */
    FILE *file = fopen(cache_filename, "w");

    if(!file){
        DSFYDEBUG("Error opening file in write-mode.\n");

        return;
    }

    /* Write data to file. */
    if(fwrite(data, 1, size, file) != size){
        DSFYDEBUG("Error writing cache data.\n");

        return;
    }

    fclose(file);
}

unsigned int cache_get_meta_playlist_revision(){
    unsigned int revision;

    /* Build cache filename. */
    snprintf(cache_filename, PATH_MAX, "%s/meta_playlist_revision", cache_directory);

    /* Try to open file for reading. */
    FILE *file = fopen(cache_filename, "r");

    if(!file){
        DSFYDEBUG("Error opening file in read-mode.\n");

        return 0;
    }

    /* Write data to file. */
    if(fread(&revision, sizeof(unsigned int), 1, file) != 1){
        DSFYDEBUG("Error reading meta playlist revision.\n");

        return 0;
    }

    fclose(file);

    return revision;
}

void cache_set_meta_playlist_revision(unsigned int revision){
    /* Build cache filename. */
    snprintf(cache_filename, PATH_MAX, "%s/meta_playlist_revision", cache_directory);

    /* Try to open file for writing. */
    FILE *file = fopen(cache_filename, "w");

    if(!file){
        DSFYDEBUG("Error opening file in write-mode.\n");

        return;
    }

    /* Write data to file. */
    if(fwrite(&revision, sizeof(unsigned int), 1, file) != 1){
        DSFYDEBUG("Error writing meta playlist revision.\n");

        return;
    }

    fclose(file);
}

