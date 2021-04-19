#include "gamedata.h"

#include "stb/stb.h"
#include "stb/stb_ds.h"

#include <assert.h>
#include <limits.h>

int strings_put(stringtable *st, const char *str)
{
    ptrdiff_t len;
    string_index index;

    assert(str);
    index = stbds_shgets(st->map, str);
    if (index.key) {
        return index.value;
    }
    len = stbds_shlenu(st->map);
    if (len < INT_MAX) {
        int pos = (int)len;
        char *value = stb_p_strdup(str);
        if (value) {
            stbds_shput(st->map, value, pos);
            return pos;
        }
        return -2;
    }
    return -1;
}

const char *strings_get(stringtable *st, int index)
{
    assert(index >=0 && index < stbds_shlenu(st->map));
    return st->map[index].key;
}

const char *strings_find(stringtable *st, const char *str)
{
    string_index index;

    assert(str);
    index = stbds_shgets(st->map, str);
    if (index.key) {
        const char *value = strings_get(st, index.value);
        return index.key;
    }
    return NULL;
}

void strings_free(stringtable *st)
{
    stbds_shfree(st->map);
    st->map = NULL;
}

void gamedata_free(gamedata *gd)
{
    stbds_arrfree(gd->blocks);
    gd->strings = NULL;
    gd->blocks = NULL;
}

int gamedata_turn(gamedata *gd)
{
    if (gd->blocks) {
        crblock *block = gd->blocks[0];
        unsigned int i;
        size_t len;
        const char *key;

        key = strings_find(gd->strings, "Runde");
        assert(key);
        assert(block->type == CROBJECT);
        len = stbds_arrlenu(block->data.attributes);
        for (i = 0; i != len; ++i) {
            if (block->data.attributes[i].key == key) {
                return block->data.attributes[i].value.number;
            }
        }
    }
    return 0;
}
