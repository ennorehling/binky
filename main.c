#include "gamedata.h"
#include "crfile.h"

#undef STB_DEFINE
#include "stb/stb.h"

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

#include <stdio.h>

void game_dump(const gamedata *gd)
{
    size_t len = stbds_arrlenu(gd->strings.strings);
    int i;
    for (i = 0; i != len; ++i) {
        const char *str = strings_get(&gd->strings, i);
        puts(str);
    }
    printf("%zu strings\n", len);
}

int main(int argc, char *argv[]) {
    gamedata gd = { 0 };
    const char *infile, *outfile;
    int err;

    if (argc < 2) return -1;

    infile = argv[1];
    err = crfile_import(&gd, infile);
    // game_dump(&gd);
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
