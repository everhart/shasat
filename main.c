#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "./sha1sat/sha1sat.h"
#include "./sha2sat/sha256sat.h"

int main(int argc, char **argv) {
    int res = 0;
    
    char opt = 0;
    int help = 0;
    int kind = 0;
    char * output = NULL;
    char * digest = NULL;
    size_t msize = 0;

    const char * short_options = "o:d:m:";
    const struct option long_options[4] = {
        { "help", no_argument, &help, 1 },
        { "sha1", no_argument, &kind, 1 },
        { "sha256", no_argument, &kind, 256 },
        { "sha224", no_argument, &kind, 224 }
    };
   
    while (
        (opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1
    ) {
        switch (opt) {
        case 'o':
            output = optarg;
            break;
        case 'd':
            digest = optarg;
            break;
        case 'm':
            msize = atoi(optarg);
            break;
        default:
            break;
        }        
    }
 
    FILE * stream = fopen(output, "w+");

    switch (msize) {
    case 1:
        res = sha1sat(stream, msize, digest);
    case 256:
        res = sha256sat(stream, msize, digest);
    case 224:
        res = sha224sat(stream, msize, digest);
    }
    
    if (res < 0) {
        printf(
            "Fatal error occured when attempting to reduce SHA%d to SAT.\n",
            kind
        );
        exit(EXIT_FAILURE);
    }
    
    printf(
        "Successfully reduced SHA%d to SAT.\n"
        "Resultant Boolean expression outputted to %s.\n", 
        kind, output
    );

	return 0;
}
