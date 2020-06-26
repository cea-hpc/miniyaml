/* This file is part of the miniyaml Library
 * Copyright (C) 2019 Commissariat a l'energie atomique et aux energies
 *                    alternatives
 *
 * SPDX-License-Identifer: LGPL-3.0-or-later
 /

#ifndef BASE64_H
#define BASE64_H

#include <sys/types.h>

size_t
base64_encode(char *dest, const char *src, size_t n);

ssize_t
base64_decode(char *dest, const char *src, size_t n);

#endif
