/* This file is part of the miniyaml Library
 * Copyright (C) 2019 Commissariat a l'energie atomique et aux energies
 *                    alternatives
 *
 * SPDX-License-Identifer: LGPL-3.0-or-later
 */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/types.h>

/* Base64 encoding/decoding implementation following RFC4648 */

static const char table[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/',
};

size_t
base64_encode(char *dest, const char *src, size_t n)
{
    const uint8_t *data = (const unsigned char *)src;
    size_t i, j = 0;

    for (i = 0; i + 2 < n; i += 3) {
        dest[j++] = table[data[i] >> 2];
        dest[j++] = table[(data[i] & 0x3) << 4 | data[i + 1] >> 4];
        dest[j++] = table[(data[i + 1] & 0xf) << 2 | data[i + 2] >> 6];
        dest[j++] = table[data[i + 2] & 0x3f];
    }

    switch (n % 3) {
    case 0:
        break;
    case 1:
        dest[j++] = table[data[i] >> 2];
        dest[j++] = table[(data[i] & 0x3) << 4];
        dest[j++] = '=';
        dest[j++] = '=';
        break;
    case 2:
        dest[j++] = table[data[i] >> 2];
        dest[j++] = table[(data[i] & 0x3) << 4 | data[i + 1] >> 4];
        dest[j++] = table[(data[i + 1] & 0xf) << 2];
        dest[j++] = '=';
        break;
    }

    dest[j] = '\0';
    return j;
}

static int_fast8_t __attribute__((const))
indexof(char c)
{
    switch (c) {
    case 'A' ... 'Z':
        return c - 'A';
    case 'a' ... 'z':
        return c - 'a' + 26;
    case '0' ... '9':
        return c - '0' + 52;
    case '+':
        return 62;
    case '/':
        return 63;
    }

    errno = EINVAL;
    return -1;
}

/* Whitespace characters are ignored. The encoded data may be padded with any
 * number of '=' characters (but only after the encoded data).
 *
 * XXX: it might be better to treat extranous padding characters as errors
 */

ssize_t
base64_decode(char *dest, const char *src, size_t n)
{
    const char *end = src + n;
    uint8_t *data = (uint8_t *)dest;
    size_t i = 0;

    while (true) {
        const char *previous;

        while (src < end && isspace(*src))
            src++;

        if (src == end)
            return i;
        if (indexof(*src) < 0)
            goto out_eilseq;

        data[i] = indexof(*src++) << 2;

        while (src < end && isspace(*src))
            src++;

        if (src == end || indexof(*src) < 0)
            goto out_eilseq;

        data[i++] |= indexof(*src) >> 4;
        previous = src++;

        while (src < end && isspace(*src))
            src++;

        if (src == end)
            return i;
        if (indexof(*src) < 0) {
            if (*src == '=')
                break;
            goto out_eilseq;
        }

        data[i++] = indexof(*previous) << 4 | indexof(*src) >> 2;
        previous = src++;

        while (src < end && isspace(*src))
            src++;

        if (src == end)
            break;
        if (indexof(*src) < 0) {
            if (*src == '=')
                break;
            goto out_eilseq;
        }
        data[i++] = indexof(*previous) << 6 | indexof(*src++);
    }

    while (src < end && *src == '=')
        src++;
    while (src < end && isspace(*src))
        src++;
    if (src == end)
        return i;

out_eilseq:
    errno = EILSEQ;
    return -1;
}
