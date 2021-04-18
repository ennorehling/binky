#pragma once

struct gamedata;

int crfile_import(struct gamedata *gd, const char *filename);
int crfile_export(struct gamedata *gd, const char *filename);
int crfile_merge(struct gamedata *gd, const char *filename);

