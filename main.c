#include "gamedata.h"
#include "crfile.h"

#undef STB_DEFINE
#include "strings.h"

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"

#include <stdio.h>

static void game_dump(gamedata *gd)
{
    size_t len = stbds_shlenu(gd->strings->map);
    int i;
    for (i = 0; i != len; ++i) {
        const char *str = strings_get(gd->strings, i);
        puts(str);
    }
    printf("%zu strings\n", len);
}

int main(int argc, char *argv[]) {
    stringtable st = { 0 };
    gamedata gd = { &st, NULL };
    const char *infile, *outfile;
    int err;

    if (argc < 2) return -1;

    infile = argv[1];
    err = crfile_import(&gd, infile);
    if (err > 0) {
        perror(infile);
    }
    if (err != 0) {
        goto error_exit;
    }
    game_dump(&gd);
    if (argc < 3) return 0;

    outfile = argv[2];
    err = crfile_merge(&gd, outfile);
    if (err > 0) {
        perror(outfile);
    }
error_exit:
    strings_free(&st);
    gamedata_free(&gd);
    return err;
}
