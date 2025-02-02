#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "rahmensprogramm.h"


void print_usage(const char *prog_name) {
    printf("Usage: %s [options]\n", prog_name);
    printf("  -c, --cycles <num>          Number of cycles to simulate\n");
    printf("  -t, --tf <file>            Path to trace file\n");
    printf("  -n, --num-cache-levels <num> Number of cache levels (1-3)\n");
    printf("  -s, --cacheline-size <num>  Cache line size in bytes\n");
    printf("  -l, --num-lines-l1 <num>    Number of lines in L1 cache\n");
    printf("  -m, --num-lines-l2 <num>    Number of lines in L2 cache\n");
    printf("  -o, --num-lines-l3 <num>    Number of lines in L3 cache\n");
    printf("  -x, --latency-cache-l1 <num> L1 cache latency\n");
    printf("  -y, --latency-cache-l2 <num> L2 cache latency\n");
    printf("  -z, --latency-cache-l3 <num> L3 cache latency\n");
    printf("  -p, --mapping-strategy <num> Cache mapping strategy\n");
    printf("  -h, --help                 Show this help message\n");
}

int parse_args(int argc, char *argv[], CacheConfig *cacheConfig) {
    int opt;
    static struct option long_options[] = {
        {"cycles", required_argument, 0, 'c'},
        {"tf", required_argument, 0, 't'},
        {"num-cache-levels", required_argument, 0, 'n'},
        {"cacheline-size", required_argument, 0, 's'},
        {"num-lines-l1", required_argument, 0, 'l'},
        {"num-lines-l2", required_argument, 0, 'm'},
        {"num-lines-l3", required_argument, 0, 'o'},
        {"latency-cache-l1", required_argument, 0, 'x'},
        {"latency-cache-l2", required_argument, 0, 'y'},
        {"latency-cache-l3", required_argument, 0, 'z'},
        {"mapping-strategy", required_argument, 0, 'p'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "c:t:n:s:l:m:o:x:y:z:p:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'c': cacheConfig->cycles = atoi(optarg); break;
            case 't': cacheConfig->tracefile = optarg; break;
            case 'n': cacheConfig->numCacheLevels = atoi(optarg); break;
            case 's': cacheConfig->cachelineSize = atoi(optarg); break;
            case 'l': cacheConfig->numLinesL1 = atoi(optarg); break;
            case 'm': cacheConfig->numLinesL2 = atoi(optarg); break;
            case 'o': cacheConfig->numLinesL3 = atoi(optarg); break;
            case 'x': cacheConfig->latencyCacheL1 = atoi(optarg); break;
            case 'y': cacheConfig->latencyCacheL2 = atoi(optarg); break;
            case 'z': cacheConfig->latencyCacheL3 = atoi(optarg); break;
            case 'p': cacheConfig->mappingStrategy = atoi(optarg); break;
            case 'h': print_usage(argv[0]); exit(EXIT_SUCCESS);
            default: print_usage(argv[0]); return -1;
        }
    }
    return 0;
}