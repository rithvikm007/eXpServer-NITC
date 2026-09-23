#ifndef XPS_DIRECTORY
#define XPS_DIRECTORY

#include "../xps.h"

/**
 * @brief Generates a directory listing HTML page for directory browsing.
 *
 * This function generates an HTML page displaying the contents of a directory
 * for browsing.
 *
 * @param dir_path The path to the directory to be browsed.
 * @param pathname The pathname to be displayed in the HTML page.
 * @return An xps_buffer_t pointer containing the HTML page, or NULL on failure.
 */
xps_buffer_t *xps_directory_browsing(const char *dir_path, const char *pathname);

#endif