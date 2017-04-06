#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>

#include "./sha1sat/sha1sat.h"
#include "./sha2sat/sha256sat.h"

int main(int argc, char **argv) {
    int res = 0;

    char opt = 0;
    FILE * stream = NULL;
    int help = 0;
    int kind = 0;
    char * output = NULL;
    char * digest = NULL;
    ssize_t msize = -1;

    const char * short_options = "ho:d:m:";
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
        case 'h':
            help = 1;
            break;
        case 'o':
            output = optarg;
            stream = fopen(output, "w+");
            if (stream == NULL) {
                printf(
                    "Error: Failed to open or create a file at %s for writing. %s\n",
                    output, strerror(errno)
                 );
            }

            break;
        case 'd':
            digest = optarg;
            break;
        case 'm':
            msize = (size_t)atoi(optarg);
            break;
        default:
            break;
        }        
    }
 
    if (help) {
        printf(
            "Usage: ./shasat [options]\n"
            "Options:\n"
            "  %-24s %s\n"
            "  %-24s %s\n"
            "  %-24s %s\n"
            "  %-24s %s\n"
            "  %-24s %s\n"
            "  %-24s %s\n"
            "  %-24s %s\n",
            "--help",       
            "Display this information.",
            "--sha1",       
            "Use SHA1 as the hash function.",
            "--sha256",     
            "Use SHA256 as the hash function.",
            "--sha224",     
            "Use SHA224 as the hash function.",
            "-o <file>",   
            "Save the output Boolean expression in dimacs cnf form to <file>.",
            "-d <digest>",
            "Use <digest> as the output digest.",
            "-m <message_size>",
            "Use <message_size> as the size of the unknown input message."
        );

        return 0;
    }

    if (kind == 0) {
        printf("Error: No hash function specified.\n");
        exit(EXIT_FAILURE);
    }

    if (output == NULL) {
        printf("Error: No output file specified.\n");
        exit(EXIT_FAILURE);
    }
    
    if (digest == NULL) {
        printf("Error: No digest specified.\n");
        exit(EXIT_FAILURE);
    }

    if (msize == -1) {
        printf("Error: No message size specified.\n");
        exit(EXIT_FAILURE);
    }

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
            "Error: Could not reduce SHA%d to SAT using the specified arguments.\n",
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
