#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "args.h"
#include "compile.h"

struct Args args;

static char const*         prog_name;
static const char*         opt_short = "o:";
static const struct option opt_long[] = {
    { "output",                 required_argument, NULL,                'o' },
    { "verbose",                no_argument,       &args.verbose,        1  },
    { NULL,                     0,                 NULL,                 0  }
};

static void usage(void) {
    fprintf(
        stderr,
        "usage:\t%s\n"
        "\t%s [options] <file>\n",
        prog_name,
        prog_name
    );
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
    prog_name = argv[0];
    static char ch;
    while ((ch = getopt_long(argc, argv, opt_short, opt_long, NULL)) != -1) {
        switch (ch) {
        case 'o':
            args.output = optarg;
            break;
        case 0:
            break;
        default:
            usage();
        }
    }

    argc -= optind;
    argv += optind;

    if (argc != 1) {
        usage();
    }
 
    compile(argv[0]);

    return EXIT_SUCCESS;
}
