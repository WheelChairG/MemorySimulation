#ifndef RAHMENSPROGRAMM_H
#define RAHMENSPROGRAMM_H

#include <stdint.h>
#include "cacheconfig.hpp"

void print_usage(const char *prog_name);

int parse_args(int argc, char *argv[], CacheConfig *config);

void validateCacheConfig(CacheConfig *config);

void setDefault(CacheConfig *config);

#endif
