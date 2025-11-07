#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <getopt.h>
#include "utils.h"

#define LN_VERSION "1.0"

#define HELPSTR "\
Usage: ln [OPTION]... [-T] TARGET LINK_NAME\n\
  or:  ln [OPTION]... TARGET\n\
  or:  ln [OPTION]... TARGET... DIRECTORY\n\
  or:  ln [OPTION]... -t DIRECTORY TARGET..."

/*** Options ***/
#define OPT_SYMBOLIC 's'
#define OPT_FORCE 'f'
#define OPT_TARGETDIR 't'
#define OPT_HELP 'h'
#define OPT_VERBOSE 'v'
#define OPT_RELATIVE 'r'

#define OPTSTR "sft:hvr"

int opt_symbolic = 0;
int opt_force = 0;
int opt_targetdir = 0;
int opt_relative = 0;
int opt_verbose = 0;
int opt_help = 0;
int opt_version = 0;

char oarg_targetdir[BUFSIZE];

void process_flags(int argc, char *argv[]) {

    int opt = 0;

    while ((opt = getopt(argc, argv, OPTSTR)) != -1) {
        switch (opt) {
            case OPT_SYMBOLIC:
                opt_symbolic = 1;
                break;
            case OPT_FORCE:
                opt_force = 1;
                break;
            case OPT_TARGETDIR:
                opt_targetdir = 1;
                snprintf(oarg_targetdir, sizeof(oarg_targetdir), "%s", optarg);
                break;
            case OPT_VERBOSE:
                opt_verbose = 1;
                break;
            case OPT_HELP:
                opt_help = 1;
                break;
            default:
                fprintf(stderr, "%s: Unknown option '%c'\n", argv[0], opt);
                exit(EXIT_FAILURE);
        }
    }
}

void process_flags_long(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"help", no_argument, 0, OPT_HELP},
        {"target-directory", required_argument, 0, OPT_TARGETDIR},
        {"relative", no_argument, 0,  OPT_RELATIVE},
        {"symbolic", no_argument, 0,  OPT_SYMBOLIC},
        {"force", no_argument, 0,  OPT_FORCE},
        {"verbose", no_argument, 0,  OPT_VERBOSE},
        {"version", no_argument, &opt_version, 1}
    };
    while (1) {

        int option_index = 0;
        int c = getopt_long(argc, argv, OPTSTR, long_options, &option_index);

        switch (c) {
            case 0:
                printf("using long argument \'%s\' ", long_options[option_index].name);
                if (optarg)
                    printf("with optarg %s", optarg);
                printf("\n");
                break;
            case OPT_SYMBOLIC:
                opt_symbolic = 1;
                break;
            case OPT_FORCE:
                opt_force = 1;
                break;
            case OPT_TARGETDIR:
                opt_targetdir = 1;
                snprintf(oarg_targetdir, sizeof(oarg_targetdir), "%s", optarg);
                break;
            case OPT_RELATIVE:
                opt_relative = 1;
                break;
            case OPT_VERBOSE:
                opt_verbose = 1;
                break;
            case OPT_HELP:
                opt_help = 1;
                break;
            default:
                break;
                // exit(EXIT_FAILURE);
        }

        if (c == -1) break;
    }
}

int get_target_count(int argc) {

    int scount = argc - optind;
    if (!opt_targetdir) scount -= 1;

    return scount;
}

int get_last_target(int argc) {

    int last_target = argc - 1;
    if (!opt_targetdir) last_target -= 1;

    if (last_target == 0) return 1;

    return last_target;
}

int main(int argc, char *argv[]) {

    // process_flags(argc, argv);
    process_flags_long(argc, argv);

    if (opt_help) {
        printf("%s\n", HELPSTR);
        exit(EXIT_SUCCESS);
    }

    if (opt_version) {
        printf("%s\n", LN_VERSION);
        exit(EXIT_SUCCESS);
    }

    if (optind >= argc) {
        printf("%s\n", HELPSTR);
        exit(EXIT_FAILURE);
    }

    int target_count = get_target_count(argc);
    int first_target = optind;
    int last_target = get_last_target(argc);



    return 0;
}
