#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "crfile.h"
#include "gamedata.h"

#include "crpat/crpat.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int crfile_import(gamedata *gd, const char *filename)
{
    CR_Parser cp;
    FILE *F = fopen(filename, "rt");
    char buf[2048];
    int done = 0, err = 0;

    if (F == NULL) {
        return EINVAL;
    }
    cp = CR_ParserCreate();

    while (!done) {
        size_t len = fread(buf, 1, sizeof(buf), F);
        if (ferror(F)) {
            fprintf(stderr,
                "read error at line %d of %s: %s\n",
                CR_GetCurrentLineNumber(cp),
                filename, strerror(errno));
            err = errno;
            break;
        }
        done = feof(F);
        if (CR_Parse(cp, buf, len, done) == CR_STATUS_ERROR) {
            fprintf(stderr,
                "parse error at line %d of %s: %s\n",
                CR_GetCurrentLineNumber(cp),
                filename, CR_ErrorString(CR_GetErrorCode(cp)));
            err = -1;
            break;
        }
    }
    CR_ParserFree(cp);
    fclose(F);
    return err;
}

int crfile_export(gamedata *gd, const char *filename)
{
    FILE *F = fopen(filename, "wt");
    if (F == NULL) {
        return EINVAL;
    }
    fputs("VERSION 67\n\"UTF-8\";charset\n", F);
    fclose(F);
    return 0;
}
