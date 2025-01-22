#include "rahmensprogramm.h"
#include <stddef.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>

void print_usage(const char *prog_name){
    fprintf(stderr, "Usage: %s [options] <inputfiles> \n", prog_name);
    fprintf(stderr, "-c --cycles <num>        number of cycles, default 1000\n");
    fprintf(stderr, "-t --tf <filename>       path of trace file\n");
    fprintf(stderr, "-h --help                help messages\ns");
    fprintf(stderr, "--num-cache-levels       number of cache levels(min. 1, max. 3)\n");
    fprintf(stderr, "--cacheline-size         size of cachelines in Bytes\n");
    fprintf(stderr, "--num-lines-l1           number of cache lines in L1-Cache\n");
    fprintf(stderr, "--num-lines-l2           number of cache lines in L2-Cache\n");
    fprintf(stderr, "--num-lines-l3           number of cache lines in L3-Cache\n");
    fprintf(stderr, "--latency-cache-l1       latency of L1-Caches in clockcycles\n");
    fprintf(stderr, "--latency-cache-l2       latency of L2-Caches in clockcycles\n");
    fprintf(stderr, "--latency-cache-l3       latency of L3-Caches in clockcycles\n");
    fprintf(stderr, "--mapping-strategy       chose mapping strategy: 0 = direct-mapped, 1 = fully associative, 2 = set-associative\n");
}

int parse_args(int argc, char *argv[], CacheConfig *cacheConfig){

    if(cacheConfig == NULL){
        cacheConfig -> cycles = 1000;
        cacheConfig -> numCacheLevels = 3;
        cacheConfig -> cachelineSize = 64;
        cacheConfig -> numLinesL1 = 1024;
        cacheConfig -> numLinesL2 = 2048;
        cacheConfig -> numLinesL3 = 4096;
        cacheConfig -> latencyCacheL1 = 5;
        cacheConfig -> latencyCacheL2 = 10;
        cacheConfig -> latencyCacheL3 = 20;
        cacheConfig -> mappingStrategy = 0;
    }

    static struct option long_options[] = {
        {"cycles", required_argument, 0, 'c'},
        {"tf", required_argument, 0, 't'},
        {"help", no_argument, 0, 'h'},
        {"num-cache-levels", required_argument, 0, 0},
        {"cacheline-size", required_argument, 0, 0},
        {"num-lines-l1", required_argument, 0, 0},
        {"num-lines-l2", required_argument, 0, 0},
        {"num-lines-l3", required_argument, 0, 0},
        {"latency-cache-l1", required_argument, 0, 0},
        {"latency-cache-l2", required_argument, 0, 0},
        {"latency-cache-l3", required_argument, 0, 0},
        {"mapping-strategie", required_argument, 0, 0},
        {0, 0, 0, 0}
    };

    int opt;

    while((opt = getopt_long(argc, argv, "c:t:h", long_options, NULL)) != -1){
        switch(opt){
            case 'c':
            cacheConfig -> cycles = atoi(optarg);
            break;
            case 't':
            cacheConfig -> tracefile = optarg;
            break;
            case 'h':
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
            default:
            print_usage(argv[0]);
            return 1;
        }
    }
     
    return 0;
}