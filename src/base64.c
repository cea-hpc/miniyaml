/* This file is part of the miniyaml Library
 * Copyright (C) 2019 Commissariat a l'energie atomique et aux energies
 *                    alternatives
 *
 * SPDX-License-Identifer: LGPL-3.0-or-later
 *
 * author: Quentin Bouget <quentin.bouget@cea.fr>
 */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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

/* 0       8       16      24         0     6     12    18    24
 * -------------------------          ------------------------- 
 * |       |       |       |          |     |     |     |     | 
 * -------------------------          ------------------------- 
 *
 *                                  /      /      |      \      \
 *
 *                                0       8       16      24      32
 *                                ---------------------------------
 *                                |       |       |       |       |
 *                                ---------------------------------
 */
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

#include <sys/types.h>

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

#if 0
#define CHUNK_SIZE 256

#include <stdio.h>

static ssize_t
b64_decode_4(char *dest, const char *data)
{
    if (indexof(data[0]) < 0 || indexof(data[1]) < 0 || indexof(data[2]) < 0
     || indexof(data[3]) < 0) {
        errno = EILSEQ;
        return -1;
    }

    dest[0] = indexof(data[0]) << 2 | indexof(data[1]) >> 4;
    dest[1] = indexof(data[1]) << 4 | indexof(data[2]) >> 2;
    dest[2] = indexof(data[2]) << 6 | indexof(data[3]);

    return 3;
}

static size_t
b64_decode_chunk(char *dest, const char *data)
{
    static_assert(CHUNK_SIZE % 4 == 0, "");

    for (size_t i = 0; i + 3 < CHUNK_SIZE; i += 4) {
        if (b64_decode_4(dest, data + i) < 0)
            return -1;
        dest += 3;
    }

    return 3 * CHUNK_SIZE / 4;
}

static size_t
b64_decode_n(char *dest, const char *src, size_t n)
{
    while (n > 0 && src[n - 1] == '=')
        n--;

    for (size_t i = 0; i + 3 < n; i += 4) {
        if (b64_decode_4(dest, src + i) < 0)
            return -1;
        dest += 3;
    }

    switch (n % 4) {
    case 0:
        return 3 * n / 4;
    case 1:
        errno = EILSEQ;
        return -1;
    case 2:
        if (indexof(src[n - 2]) < 0 || indexof(src[n - 1]) < 0) {
            errno = EILSEQ;
            return -1;
        }
        *dest = indexof(src[n - 2]) << 2 | indexof(src[n - 1]) >> 4;
        return 3 * (n - 2) / 4 + 1;
    case 3:
        if (indexof(src[n - 3]) < 0 || indexof(src[n - 2]) < 0
         || indexof(src[n - 1]) < 0) {
            errno = EILSEQ;
            return -1;
        }
        *dest++ = indexof(src[n - 3]) << 2 | indexof(src[n - 2]) >> 4;
        *dest = indexof(src[n - 2]) << 4 | indexof(src[n - 1]) >> 4;
        return 3 * (n - 3) / 4 + 2;
    }
}

ssize_t
base64_decode(char *dest, const char *src, size_t n)
{
    const char *end = src + n;
    char buffer[CHUNK_SIZE];
    size_t total = 0;

    while (true) {
        ssize_t decoded_bytes;
        size_t i = 0;

        /* Pack as many non-whitespace characters as possible */
        do {
            char *tmp = &buffer[i];
            const char *start;

            /* Discard any leading whitespace characters */
            while (src < end && isspace(*src))
                src++;
            start = src;

            /* Find as much non-whitespace characters as can fit in `buffer' */
            while (src < end && !isspace(*src) && i < CHUNK_SIZE)
                src++, i++;

            memcpy(tmp, start, src - start);
        } while (src < end && i < CHUNK_SIZE);

        if (src == end) {
            decoded_bytes = b64_decode_n(dest, buffer, i);
            if (decoded_bytes < 0)
                return -1;
            return total + decoded_bytes;
        }

        decoded_bytes = b64_decode_chunk(dest, buffer);
        if (decoded_bytes < 0)
            return -1;
        total += decoded_bytes;
        dest += decoded_bytes;
    }
}

#else

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
#endif
