#include "gamedata.h"
#include "crfile.h"

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
    gamedata gd = { 0 };
    const char *infile, *outfile;
    int err;

    if (argc < 2) return -1;

    infile = argv[1];
    err = crfile_import(&gd, infile);
    if (err > 0) {
        perror(infile);
    }
    if (err != 0) {
        return err;
    }
    if (argc < 3) return 0;

    outfile = argv[2];
    return crfile_export(&gd, outfile);
}
