#include "xps_file.h"

void file_source_close_handler(void *ptr);
void file_source_handler(void *ptr);

xps_file_t *xps_file_create(xps_core_t *core, const char *file_path, int *error) {
    /*assert*/
    assert(core != NULL);
    assert(file_path != NULL);

    *error = E_FAIL;

    logger(LOG_DEBUG, "file_create()", "Requested file: %s", file_path);
    /*check if file is inside the public directory*/
    char *resolved_path = realpath(file_path, NULL);
    char *resolved_public = realpath("../public", NULL);

    if (resolved_path == NULL || resolved_public == NULL) {
        logger(LOG_ERROR, "xps_file_create()", "realpath() failed");
        /*free both path*/
        free(resolved_path);
        free(resolved_public);
        /*close file object*/
        return NULL;
    }

    size_t public_len = strlen(resolved_public);
    if (strncmp(resolved_path, resolved_public, public_len) != 0) {
        logger(LOG_WARNING, "xps_file_create()", "file requested is outside of public directory");
        *error = E_PERMISSION;
        /*free both path*/
        free(resolved_path);
        free(resolved_public);
        /*close file object*/
        return NULL;
    }

    /*free both path*/
    free(resolved_path);
    free(resolved_public);

    /*check if others have read permission*/
    struct stat file_stat;
    if (stat(file_path, &file_stat) != 0) {
        logger(LOG_ERROR, "xps_file_create()", "stat() failed");
        perror("Error message");
        /*close file object*/
        return NULL;
    }

    if (!(file_stat.st_mode & S_IROTH)) {
        logger(LOG_WARNING, "xps_file_create()", "others do not have read permission");
        *error = E_PERMISSION;
        /*close file object*/
        return NULL;
    }

    // Getting size of file from stat (already called above)
    long temp_size = file_stat.st_size;

    // Opening file
    FILE *file_struct = fopen(file_path, "rb");
    /*handle EACCES,ENOENT or any other error*/
    if (file_struct == NULL) {
        /*logs EACCES,ENOENT or any other error*/
        logger(LOG_ERROR, "xps_file_create()", "fopen() failed");
        return NULL;
    }

    const char *mime_type = xps_get_mime(file_path);

    /*Alloc memory for instance of xps_file_t*/
    xps_file_t *file = (xps_file_t *)malloc(sizeof(xps_file_t));
    if(file == NULL) {
        logger(LOG_ERROR, "xps_file_create()", "malloc() failed");
        fclose(file_struct);
        return NULL;
    }
    xps_pipe_source_t *source = xps_pipe_source_create((void *)file, file_source_handler, file_source_close_handler);
    /*if source is null, close file_struct and return*/
    if (source == NULL) {
        logger(LOG_ERROR, "xps_file_create()", "xps_pipe_source_create() failed");
        fclose(file_struct);
        free(file);
        return NULL;
    }

    // Init values
    source->ready = true;
    /*initialise the fields of file instance*/
    file->core = core;
    file->file_path = file_path;
    file->source = source;
    file->file_struct = file_struct;
    file->size = temp_size;
    file->mime_type = mime_type;
        
    *error = OK;

    logger(LOG_DEBUG, "xps_file_create()", "created file");

    return file;
}

void xps_file_destroy(xps_file_t *file) {
    /*assert*/
    assert(file != NULL);  

    fclose(file->file_struct);
    if(file->source != NULL) {
        xps_pipe_source_destroy(file->source);
        file->source = NULL;
    }
    free(file);

    logger(LOG_DEBUG, "xps_file_destroy()", "destroyed file struct");
}

void file_source_handler(void *ptr) {
    /*assert*/
    assert(ptr != NULL);

    xps_pipe_source_t *source = ptr;
    /*get file from source ptr*/
    xps_file_t *file = (xps_file_t *)source->ptr;

    /*create buffer and handle any error*/
    xps_buffer_t *buff = xps_buffer_create(DEFAULT_BUFFER_SIZE, 0, NULL);
    if (buff == NULL) {
        logger(LOG_ERROR, "file_source_handler()", "xps_buffer_create() failed");
        /*close file_struct and return*/
        return;
    }

    // Read from file
    size_t read_n = fread(buff->data, 1, buff->size, file->file_struct);
    buff->len = read_n;

    // Checking for read errors
    if (ferror(file->file_struct)) {
        /*deallocate buff, file_struct and return*/
        xps_buffer_destroy(buff);
        xps_file_destroy(file);
        return;
    }

    // If end of file reached
    if (read_n == 0 && feof(file->file_struct)) {
        /*deallocate buff, file_struct and return*/
        xps_buffer_destroy(buff);
        xps_file_destroy(file);
        return;
    }

    /*Write to pipe form buff*/
    if(xps_pipe_source_write(source, buff) != OK) {
        logger(LOG_ERROR, "file_source_handler()", "xps_pipe_source_write() failed");
        /*deallocate buff, file_struct and return*/
        xps_buffer_destroy(buff);
        xps_file_destroy(file);
        return;
    }
    /*deallocate buff*/
    xps_buffer_destroy(buff);
}

void file_source_close_handler(void *ptr) {
    /*assert*/
    assert(ptr != NULL);
    xps_pipe_source_t *source = ptr;
    /*get file from source ptr*/
    xps_file_t *file = (xps_file_t *)source->ptr;
	/*deallocate file object*/
    if(file != NULL) {
        xps_file_destroy(file);
    }
}