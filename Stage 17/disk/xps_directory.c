#include "../xps.h"

xps_buffer_t * xps_directory_browsing(const char * dir_path, const char * pathname) {
    /* validate parameters */
    assert(dir_path != NULL);
    assert(pathname != NULL);

    // Buffer for HTTP message
    size_t buff_size = 8192; // Initial buffer size
    char * buff = malloc(buff_size);
    if (buff == NULL) {
        logger(LOG_ERROR, "xps_directory_browsing()", "malloc() failed for 'buff'");
        return NULL;
    }
    buff[0] = '\0'; // Initialize buffer to an empty string

    //Here we will append the html file into this buff starting from the heading and basic styling
    snprintf(buff, buff_size, "<html><head lang='en'><meta http-equiv='Content-Type' "
        "content='text/html; "
        "charset=UTF-8'><meta name='viewport' content='width=device-width, "
        "initial-scale=1.0'><title>Directory: "
        "%s</title><style>body{font-family: monospace; "
        "font-size: 15px;}td {padding: 1.5px 6px; padding-right: 20px;} "
        "h1{font-family: serif; "
        "margin: 0;} h3{font-family: serif;margin: 12px 0px; "
        "background-color: rgba(0,0,0,0.1); "
        "padding: 4px 0px;}</style></head><body><h1>eXpServer</h1><h3>Index "
        "of&nbsp;%s</h3><hr><table>",
        pathname, pathname);

    // Open a directory stream using opendir function
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        logger(LOG_ERROR, "xps_directory_browsing()", "opendir() failed");
        free(buff);
        return NULL;
    }

    struct dirent * dir_entry;

    while ((dir_entry = readdir(dir)) != NULL) {
        //skip the first two entries such as . and .. in list directory from dir_entry->d_dname
        if (strcmp(dir_entry -> d_name, ".") == 0 || strcmp(dir_entry -> d_name, "..") == 0)
            continue;

        char full_path[1024];
        /* copy the dir_path and dir_entry->d_name with formatted with a / into full_path */
        //HINT: you can use snprintf
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, dir_entry -> d_name);

        //open the file in the full_path
        int file_fd = open(full_path, O_RDONLY);
        if (file_fd == -1) {
            logger(LOG_ERROR, "xps_directory_browsing()", "failed to open file");
            continue;
        }

        struct stat file_stat;

        //get information about the file
        if (fstat(file_fd, & file_stat) == -1) {
            logger(LOG_ERROR, "xps_directory_browsing()", "fstat()");
            close(file_fd);
            continue;
        }

        //check if file is regular file or if the file is a direcory
        if (S_ISREG(file_stat.st_mode) || S_ISDIR(file_stat.st_mode)) {
            char *is_dir = S_ISDIR(file_stat.st_mode) ? "/" : "";

            char *temp_pathname = str_create(pathname);
            if (strlen(temp_pathname) > 0 && temp_pathname[strlen(temp_pathname) - 1] == '/')
                temp_pathname[strlen(temp_pathname) - 1] = '\0';

            // char time_buff[20];
            // strftime(time_buff, sizeof(time_buff), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));

            if (S_ISREG(file_stat.st_mode)) // IS_FILE
                sprintf(buff + strlen(buff),
                    "<tr><td><a "
                    "href='%s/%s'>%s%s</a></td></tr>\n",
                    temp_pathname, dir_entry -> d_name, dir_entry -> d_name, is_dir);
            else
                sprintf(
                    buff + strlen(buff),
                    "<tr><td><a href='%s/%s'>%s%s</a></td><td></td></tr>\n",
                    temp_pathname, dir_entry -> d_name, dir_entry -> d_name, is_dir);
            free(temp_pathname);
        }

        /*close the file*/
        close(file_fd);

    }

    closedir(dir);
    sprintf(buff + strlen(buff), "</table></body></html>");
    xps_buffer_t * directory_browsing = xps_buffer_create(strlen(buff), strlen(buff), (u_char *)buff);

    if (directory_browsing == NULL) {
        logger(LOG_ERROR, "xps_directory_browsing()", "xps_buffer_create() returned NULL");
        free(buff);
        return NULL;
    }

    return directory_browsing;
}