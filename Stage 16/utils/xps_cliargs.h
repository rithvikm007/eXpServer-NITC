#ifndef XPS_CLIARGS_H
#define XPS_CLIARGS_H

#include "../xps.h"

struct xps_cliargs_s {
  char *config_path;
};

xps_cliargs_t *xps_cliargs_create(int argc, char *argv[]);
void xps_cliargs_destroy(xps_cliargs_t *cilargs);

#endif