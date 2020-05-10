/* This file is part of the miniyaml Library
 * Copyright (C) 2019 Commissariat a l'energie atomique et aux energies
 *                    alternatives
 *
 * SPDX-License-Identifer: LGPL-3.0-or-later
 *
 * author: Quentin Bouget <quentin.bouget@cea.fr>
 */

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "base64.h"
#include "miniyaml.h"
    
bool
yaml_parser_skip(yaml_parser_t *parser, yaml_event_type_t last)
{
    size_t depth = 0;

    switch (last) {
    case YAML_DOCUMENT_START_EVENT:
    case YAML_MAPPING_START_EVENT:
    case YAML_SEQUENCE_START_EVENT:
        depth++;
        break;
    default:
        break;
    }

    while (depth) {
        yaml_event_t event;

        if (!yaml_parser_parse(parser, &event))
            return false;

        switch (event.type) {
        case YAML_MAPPING_START_EVENT:
        case YAML_SEQUENCE_START_EVENT:
            depth++;
            break;
        case YAML_DOCUMENT_END_EVENT:
        case YAML_MAPPING_END_EVENT:
        case YAML_SEQUENCE_END_EVENT:
            depth--;
            break;
        default:
            break;
        }

        yaml_event_delete(&event);
    }

    return true;
}

enum yaml_type {
    /* Collection types */
    YT_MAP,
    YT_OMAP,
    YT_PAIRS,
    YT_SET,
    YT_SEQ,

    /* Scalar types */
    YT_BINARY,
    YT_BOOL,
    YT_FLOAT,
    YT_INT,
    YT_MERGE,
    YT_NULL,
    YT_STR,
    YT_TIMESTAMP,
    YT_VALUE,
    YT_YAML,
};

static enum yaml_type __attribute__((pure))
_yaml_tag2type(const char *tag)
{
    switch (*tag++) {
    case 'b': /* bool, binary */
        switch (*tag++) {
        case 'i': /* binary */
            if (strcmp(tag, "nary"))
                break;
            return YT_BINARY;
        case 'o': /* bool */
            if (strcmp(tag, "ol"))
                break;
            return YT_BOOL;
        }
        break;
    case 'f': /* float */
        if (strcmp(tag, "loat"))
            break;
        return YT_FLOAT;
    case 'i': /* int */
        if (strcmp(tag, "nt"))
            break;
        return YT_INT;
    case 'm': /* map, merge */
        switch (*tag++) {
        case 'a': /* map */
            if (strcmp(tag, "p"))
                break;
            return YT_MAP;
        case 'e': /* merge */
            if (strcmp(tag, "rge"))
                break;
            return YT_MERGE;
        }
        break;
    case 'n': /* null */
        if (strcmp(tag, "ull"))
            break;
        return YT_NULL;
    case 'o': /* omap */
        if (strcmp(tag, "map"))
            break;
        return YT_OMAP;
    case 'p': /* pairs */
        if (strcmp(tag, "airs"))
            break;
        return YT_PAIRS;
    case 's': /* seq, set, str */
        switch (*tag++) {
        case 'e': /* seq, set */
            switch (*tag++) {
            case 'q': /* seq */
                if (*tag != '\0')
                    break;
                return YT_SEQ;
            case 't': /* set */
                if (*tag != '\0')
                    break;
                return YT_SET;
            }
            break;
        case 't': /* str */
            if (strcmp(tag, "r"))
                break;
            return YT_STR;
        }
        break;
    case 't': /* timestamp */
        if (strcmp(tag, "imestamp"))
            break;
        return YT_TIMESTAMP;
    case 'v': /* value */
        if (strcmp(tag, "alue"))
            break;
        return YT_VALUE;
    case 'y': /* yaml */
        if (strcmp(tag, "aml"))
            break;
        return YT_YAML;
    }

    errno = EINVAL;
    return -1;
}

static enum yaml_type
yaml_tag2type(const char *tag)
{
    switch (*tag++) {
    case 't': /* tag URI */
        if (strncmp(tag, "ag:", 3))
            break;
        tag += 3;

        switch (*tag++) {
        case 'y': /* yaml.org,2002: */
            if (strncmp(tag, "aml.org,2002:", 13))
                break;
            return _yaml_tag2type(tag + 13);
        }
        break;
    }

    /* Local tag */
    errno = ENOTSUP;
    return -1;
}

bool
yaml_parse_null(const yaml_event_t *event)
{
    const char *value = yaml_scalar_value(event);
    const char *tag = yaml_scalar_tag(event);

    assert(event->type == YAML_SCALAR_EVENT);

    if (tag && yaml_tag2type(tag) == YT_NULL)
        return true;
    if (!yaml_scalar_is_plain(event))
        goto out_einval;

    switch (*value++) {
    case '\0': /* (empty) */
        return true;
    case '~': /* ~ */
        return *value == '\0';
    case 'n': /* null */
        if (strcmp(value, "ull"))
            break;
        return true;
    case 'N': /* Null, NULL */
        switch (*value++) {
        case 'u': /* Null */
            return strcmp(value, "ll") == 0;
        case 'U': /* NULL */
            return strcmp(value, "LL") == 0;
        }
        break;
    }

out_einval:
    errno = EINVAL;
    return false;
}

bool
yaml_parse_integer(const yaml_event_t *event, intmax_t *i)
{
    const char *value = yaml_scalar_value(event);
    const char *tag = yaml_scalar_tag(event);
    int save_errno;
    char *end;

    assert(event->type == YAML_SCALAR_EVENT);

    if (tag ? yaml_tag2type(tag) != YT_INT : !yaml_scalar_is_plain(event)) {
        errno = EINVAL;
        return false;
    }

    save_errno = errno;
    errno = 0;
    *i = strtoimax(value, &end, 0);
    if ((*i == INTMAX_MIN || *i == INTMAX_MAX) && errno == ERANGE)
        return false;
    if (*end != '\0') {
        errno = EINVAL;
        return false;
    }

    errno = save_errno;
    return true;
}

bool
yaml_parse_unsigned_integer(const yaml_event_t *event, uintmax_t *u)
{
    const char *value = yaml_scalar_value(event);
    const char *tag = yaml_scalar_tag(event);
    int save_errno;
    char *end;

    assert(event->type == YAML_SCALAR_EVENT);

    if (tag ? yaml_tag2type(tag) != YT_INT : !yaml_scalar_is_plain(event)) {
        errno = EINVAL;
        return false;
    }

    save_errno = errno;
    errno = 0;
    *u = strtoumax(value, &end, 0);
    if ((*u == UINTMAX_MAX) && errno == ERANGE)
        return false;
    if (*end != '\0') {
        errno = EINVAL;
        return false;
    }

    errno = save_errno;
    return true;
}

bool
yaml_parse_string(const yaml_event_t *event, const char **string,
                  size_t *length)
{
    const char *tag = yaml_scalar_tag(event);

    assert(event->type == YAML_SCALAR_EVENT);

    if (tag && yaml_tag2type(tag) != YT_STR) {
        errno = EINVAL;
        return false;
    }

    *string = yaml_scalar_value(event);
    if (length)
        *length = event->data.scalar.length;
    return true;
}

#ifndef YAML_BINARY_TAG
# define YAML_BINARY_TAG "tag:yaml.org,2002:binary"
#endif

static bool
_yaml_emit_binary(yaml_emitter_t *emitter, const char *b64, size_t size)
{
    return yaml_emit_scalar(emitter, YAML_BINARY_TAG, b64, size,
                            YAML_ANY_SCALAR_STYLE);
}

bool
yaml_emit_binary(yaml_emitter_t *emitter, const char *data, size_t size)
{
    char onstack[256];
    char *buffer = onstack;
    bool success;
    
    /* Encoding in base64 increases the size by a factor of 4/3
     *                     vvvv      vvvvvvv
     */
    if (sizeof(onstack) < (size + 2) * 4 / 3 + 1) {
        /*                      ^^^          ^^^
         * Account for padding bytes   and   the terminating null byte
         */
        buffer = malloc((size + 2) * 4 / 3 + 1);
        if (buffer == NULL)
            return false;
    }

    size = base64_encode(buffer, data, size);
    success = _yaml_emit_binary(emitter, buffer, size);
    if (buffer != onstack)
        free(buffer);
    return success;
}

bool
yaml_parse_binary(const yaml_event_t *event, char *buffer, size_t *size)
{
    const char *value = yaml_scalar_value(event);
    size_t length = yaml_scalar_length(event);
    const char *tag = yaml_scalar_tag(event);
    ssize_t rc;

    assert(event->type == YAML_SCALAR_EVENT);

    if (tag ? yaml_tag2type(tag) != YT_BINARY : !yaml_scalar_is_plain(event)) {
        errno = EINVAL;
        return false;
    }

     rc = base64_decode(buffer, value, length);
     if (rc != -1)
         *size = rc;
     return rc != -1;
}
