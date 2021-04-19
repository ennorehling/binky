#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "crfile.h"
#include "gamedata.h"

#include "crpat/crpat.h"

#include "stb/stb.h"
#include "stb/stb_ds.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct import_context {
    gamedata *gd;
    crblock *block;
} import_context;

static bool ignored_block(const char *name) {
    /* sorted (!) array of ignored blocks: */
    const char *names[] = {
        "MESSAGETYPE",
        "TRANSLATION",
        NULL
    };
    int i;

    /* FIXME: linear search is slow */
    for (i = 0; names[i]; ++i) {
        int d = strcmp(names[i], name);
        if (d == 0) {
            return true;
        }
        else if (d > 0) {
            break;
        }
    }
    return false;
}

static enum CR_Error handle_element(void *udata, const char *name,
    unsigned int keyc, int keyv[])
{
    import_context *ctx = (import_context *)udata;
    crblock *block;
    stringtable *st = ctx->gd->strings;

    if (ignored_block(name)) {
        /* ignore these blocks */
        ctx->block = NULL;
        return CR_ERROR_NONE;
    }

    block = malloc(sizeof(crblock));
    if (block) {
        crblock **block_p = stbds_arraddnptr(ctx->gd->blocks, 1);
        int i;
        i = strings_put(st, name);
        block->name = strings_get(st, i);
        block->type = CROBJECT;
        block->nkeys = keyc;
        block->data.attributes = NULL;
        ctx->block = *block_p = block;
        if (keyc > 0) {
            memcpy(block->keys, keyv, sizeof(keyv[0]) * keyc);
        }
        return CR_ERROR_NONE;
    }
    return CR_ERROR_NO_MEMORY;
}

static attribute *attr_add(import_context *ctx, const char *name, enum value_t type)
{
    stringtable *st = ctx->gd->strings;
    int index = strings_put(st, name);
    if (index >= 0) {
        crblock *block = ctx->block;
        attribute *attr = stbds_arraddnptr(block->data.attributes, 1);
        if (attr) {
            attr->type = type;
            attr->key = strings_get(st, index);
        }
        return attr;
    }
    return NULL;
}

static enum CR_Error handle_text(void *udata, const char *text) {
    import_context *ctx = (import_context *)udata;
    crblock *block = ctx->block;

    if (block) {
        char *value = stb_p_strdup(text);

        if (value) {
            char **ins = stbds_arraddnptr(block->data.strings, 1);

            block->type = CRSTRINGS;
            *ins = value;
            return CR_ERROR_NONE;
        }
        return CR_ERROR_NO_MEMORY;
    }
    return CR_ERROR_NONE;
}

static enum CR_Error handle_number(void *udata, const char *name, long value)
{
    import_context *ctx = (import_context *)udata;

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
    }
    return CR_ERROR_NONE;
}

static enum CR_Error handle_location(void *udata, const char *name, const char *value)
{
    import_context *ctx = (import_context *)udata;

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
    }
    return CR_ERROR_NONE;
}

static enum CR_Error handle_property(void *udata, const char *name, const char *value)
{
    import_context *ctx = (import_context *)udata;
    
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
    }
    return CR_ERROR_NONE;
}

int crfile_import(gamedata *gd, const char *filename)
{
    CR_Parser cp;
    int err;
    import_context ctx;

    ctx.gd = gd;
    cp = CR_ParserCreate();
    CR_SetLocationHandler(cp, handle_location);
    CR_SetNumberHandler(cp, handle_number);
    CR_SetPropertyHandler(cp, handle_property);
    CR_SetTextHandler(cp, handle_text);
    CR_SetElementHandler(cp, handle_element);
    CR_SetUserData(cp, (void *)&ctx);

    err = CR_ReadFile(cp, filename);

    CR_ParserFree(cp);
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
        fprintf(F, "%s", block->name);
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
                const char *key = attr->key;
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
