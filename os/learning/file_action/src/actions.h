#pragma once

int touch_dir(const char *dir_name);
int ls_dir(const char *dir_name);
int rm_dir(const char *dir_name);

int touch_file(const char *file_name);
int cat_file(const char *file_name);
int rm_file(const char *file_name);

int touch_slink(const char *slink_file_name);
int cat_slink(const char *slink_name);
int cat_slink_file(const char *slink_name);
int rm_slink(const char *slink_name);

int touch_hlink(const char *hlink_name);
int ch_perm(const char *hlink_name);
int cat_perm(const char *hlink_name);
int rm_hlink(const char *hlink_name);