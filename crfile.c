#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "crfile.h"
#include "gamedata.h"

#include "crpat/crpat.h"

#include "stb/stb.h"
#include "stb/stb_ds.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct parser_context {
    gamedata *gd;
    crblock *block;
} parser_context;

static enum CR_Error handle_element(void *udata, const char *name,
    unsigned int keyc, int keyv[])
{
    parser_context *ctx = (parser_context *)udata;
    crblock *block;

    block = malloc(sizeof(crblock));
    if (block) {
        crblock **block_p = stbds_arraddnptr(ctx->gd->blocks, 1);
        ctx->block = *block_p = block;
        block->iname = strings_put(&ctx->gd->strings, name);
        block->type = CROBJECT;
        block->nkeys = keyc;
        block->data.attributes = NULL;
        if (keyc > 0) {
            memcpy(block->keys, keyv, sizeof(keyv[0]) * keyc);
        }
        return CR_ERROR_NONE;
    }
    return CR_ERROR_NO_MEMORY;
}

static attribute *attr_add(parser_context *ctx, const char *name, enum value_t type)
{
    int index = strings_put(&ctx->gd->strings, name);
    if (index >= 0) {
        crblock *block = ctx->block;
        attribute *attr = stbds_arraddnptr(block->data.attributes, 1);
        if (attr) {
            attr->type = type;
            attr->ikey = index;
        }
        return attr;
    }
    return NULL;
}

static enum CR_Error handle_text(void *udata, const char *text) {
    parser_context *ctx = (parser_context *)udata;
    char *value = stb_p_strdup(text);

    if (value) {
        crblock *block = ctx->block;
        char **ins = stbds_arraddnptr(block->data.strings, 1);

        block->type = CRSTRINGS;
        *ins = value;
        return CR_ERROR_NONE;
    }
    return CR_ERROR_NO_MEMORY;
}

static enum CR_Error handle_number(void *udata, const char *name, long value)
{
    parser_context *ctx = (parser_context *)udata;

    if (ctx->block) {
        crblock *block = ctx->block;
        attribute *attr;

        if (block->type != CROBJECT) {
            return CR_ERROR_SYNTAX;
        }

        attr = attr_add(ctx, name, VALUE_NUMBER);
        if (!attr) {
            return CR_ERROR_NO_MEMORY;
        }
        attr->value.number = value;
        return CR_ERROR_NONE;
    }
    return CR_ERROR_GRAMMAR;
}

static enum CR_Error handle_location(void *udata, const char *name, const char *value)
{
    parser_context *ctx = (parser_context *)udata;

    if (ctx->block) {
        crblock *block = ctx->block;
        attribute *attr;
        short x = 0, y = 0, z = 0;
        int n;

        if (block->type != CROBJECT) {
            return CR_ERROR_SYNTAX;
        }

        n = sscanf(value, "%hd %hd %hd", &x, &y, &z);
        if (n < 2) {
            return CR_ERROR_SYNTAX;
        }

        attr = attr_add(ctx, name, VALUE_LOCATION);
        if (!attr) {
            return CR_ERROR_NO_MEMORY;
        }
        attr->value.location[0] = x;
        attr->value.location[1] = y;
        attr->value.location[2] = z;
        return CR_ERROR_NONE;
    }
    return CR_ERROR_GRAMMAR;
}

static enum CR_Error handle_property(void *udata, const char *name, const char *value)
{
    parser_context *ctx = (parser_context *)udata;
    
    char *string = stb_p_strdup(value);
    if (!string) {
        return CR_ERROR_NO_MEMORY;
    }

    if (ctx->block) {
        crblock *block = ctx->block;
        attribute *attr;

        if (block->type != CROBJECT) {
            return CR_ERROR_SYNTAX;
        }

        attr = attr_add(ctx, name, VALUE_STRING);
        if (!attr) {
            return CR_ERROR_NO_MEMORY;
        }
        attr->value.string = string;
        return CR_ERROR_NONE;
    }
    return CR_ERROR_GRAMMAR;
}

int crfile_import(gamedata *gd, const char *filename)
{
    CR_Parser cp;
    parser_context ctx;
    FILE *F = fopen(filename, "rt");
    unsigned char buf[2048];
    int done = 0, err = 0;
    size_t len;
    const char *line = (const char *)buf;

    if (F == NULL) {
        return EINVAL;
    }

    len = fread(buf, 1, sizeof(buf), F);
    if (len >= 3 && buf[0] == 0xef) {
        /* skip BOM */
        len -= 3;
        line += 3;
        printf("BOM found\n");
    }
    cp = CR_ParserCreate();
    CR_SetElementHandler(cp, handle_element);
    CR_SetPropertyHandler(cp, handle_property);
    CR_SetNumberHandler(cp, handle_number);
    CR_SetTextHandler(cp, handle_text);
    CR_SetLocationHandler(cp, handle_location);
    ctx.gd = gd;
    CR_SetUserData(cp, (void *)&ctx);

    while (!done) {
        if (ferror(F)) {
            fprintf(stderr,
                "read error at line %d of %s: %s\n",
                CR_GetCurrentLineNumber(cp),
                filename, strerror(errno));
            err = errno;
            break;
        }
        done = feof(F);
        if (CR_Parse(cp, line, len, done) == CR_STATUS_ERROR) {
            fprintf(stderr,
                "parse error at line %d of %s: %s\n",
                CR_GetCurrentLineNumber(cp),
                filename, CR_ErrorString(CR_GetErrorCode(cp)));
            err = -1;
            break;
        }
        len = fread(buf, 1, sizeof(buf), F);
        line = (const char *)buf;
    }
    CR_ParserFree(cp);
    fclose(F);
    return err;
}

int crfile_export(gamedata *gd, const char *filename)
{
    FILE *F = fopen(filename, "wt");
    unsigned int i;
    size_t nblocks;

    if (F == NULL) {
        return EINVAL;
    }

    nblocks = stbds_arrlenu(gd->blocks);
    for (i = 0; i != nblocks; ++i) {
        unsigned int k;
        crblock *block = gd->blocks[i];
        fprintf(F, "%s", block_name(gd, block));
        for (k = 0; k != block->nkeys; ++k) {
            fprintf(F, " %d", block->keys[k]);
        }
        fputc('\n', F);
        if (block->type == CRSTRINGS) {
            size_t len = stbds_arrlenu(block->data.strings);
            for (k = 0; k != len; ++k) {
                char *str = block->data.strings[k];
                fprintf(F, "\"%s\"\n", str);
            }
        }
        else if (block->type == CROBJECT) {
            size_t len = stbds_arrlenu(block->data.attributes);
            unsigned int a;
            for (a = 0; a != len; ++a) {
                attribute *attr = block->data.attributes + a;
                const char *key = strings_get(&gd->strings, attr->ikey);
                if (attr->type == VALUE_STRING) {
                    const char *val = attr->value.string;
                    fprintf(F, "\"%s\";%s\n", val, key);
                }
                else if (attr->type == VALUE_NUMBER) {
                    fprintf(F, "%d;%s\n", attr->value.number, key);
                }
                else if (attr->type == VALUE_LOCATION) {
                    if (attr->value.location[2]) {
                        fprintf(F, "%d %d %d;%s\n",
                            attr->value.location[0],
                            attr->value.location[1],
                            attr->value.location[2],
                            key);
                    }
                    else {
                        /* hack: skills are stored as location */
                        fprintf(F, "%d %d;%s\n",
                            attr->value.location[0],
                            attr->value.location[1],
                            key);
                    }
                }
            }
        }
    }
    
    fclose(F);
    return 0;
}
