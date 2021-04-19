#include "crfile.h"
#include "gamedata.h"

#include "crpat/crpat.h"

#include <stdio.h>
#include <errno.h>

typedef struct merge_context {
    gamedata *gd;
    int turn;
    crblock *block;
} merge_context;

static enum CR_Error handle_element(void *udata, const char *name,
    unsigned int keyc, int keyv[])
{
    return CR_ERROR_NONE;
}

static enum CR_Error handle_text(void *udata, const char *text)
{
    return CR_ERROR_NONE;
}

static enum CR_Error handle_number(void *udata, const char *name, long value)
{
    return CR_ERROR_NONE;
}

static enum CR_Error handle_location(void *udata, const char *name, const char *value)
{
    return CR_ERROR_NONE;
}

static enum CR_Error handle_property(void *udata, const char *name, const char *value)
{
    return CR_ERROR_NONE;
}

int crfile_merge(gamedata *gd, const char *filename)
{
    CR_Parser cp;
    merge_context ctx;
    int err;

    ctx.gd = gd;
    ctx.turn = gamedata_turn(gd);
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
