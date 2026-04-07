#pragma once

int argcmp(const char *s1, const char *s2);

int get_parent_path(const char *path, char *res);

void write_n(const int fd, const char *buf, const int n);

int add_postfix(const char *src, const int src_sz, char *dest, const int dest_capacity, const char *postfix, const int postfix_sz);