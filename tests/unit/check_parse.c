/* This file is part of MiniYAML
 * Copyright (C) 2019 Commissariat a l'energie atomique et aux energies
 *                    alternatives
 *
 * SPDX-License-Identifer: LGPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <check.h>
#include <errno.h>

#include <miniyaml.h>

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

static yaml_parser_t parser;

static void
parser_init(void)
{
    ck_assert(yaml_parser_initialize(&parser));
}

static void
parser_exit(void)
{
    yaml_parser_delete(&parser);
}

/* Testing yaml_mapping_tag() and yaml_scalar_*() is of very little value...
 * But since the code is already written. One never knows, it might catch a
 * incompatible change in libyaml.
 */

/*----------------------------------------------------------------------------*
 |                             yaml_mapping_tag()                             |
 *----------------------------------------------------------------------------*/

START_TEST(ymt_basic)
{
    const unsigned char INPUT[] = "!test {}";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_MAPPING_START_EVENT);

    ck_assert_str_eq(yaml_mapping_tag(&event), "!test");

    yaml_event_delete(&event);
}
END_TEST

START_TEST(ymt_no_tag)
{
    const unsigned char INPUT[] = "{}";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_MAPPING_START_EVENT);

    ck_assert_ptr_null(yaml_mapping_tag(&event));

    yaml_event_delete(&event);
}
END_TEST

/*----------------------------------------------------------------------------*
 |                              yaml_scalar_*()                               |
 *----------------------------------------------------------------------------*/

START_TEST(ys_no_tag)
{
    const unsigned char INPUT[] = "abcdefgh";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert_str_eq(yaml_scalar_value(&event), "abcdefgh");
    ck_assert_uint_eq(yaml_scalar_length(&event), 8);
    ck_assert_ptr_null(yaml_scalar_tag(&event));

    yaml_event_delete(&event);
}
END_TEST

START_TEST(ys_tagged)
{
    const unsigned char INPUT[] = "!test 'abcdefgh'";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert_str_eq(yaml_scalar_value(&event), "abcdefgh");
    ck_assert_uint_eq(yaml_scalar_length(&event), 8);
    ck_assert_str_eq(yaml_scalar_tag(&event), "!test");

    yaml_event_delete(&event);
}
END_TEST

START_TEST(ys_plain)
{
    const unsigned char INPUT[] = "!test abcdefgh";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert_str_eq(yaml_scalar_value(&event), "abcdefgh");
    ck_assert_uint_eq(yaml_scalar_length(&event), 8);
    ck_assert(yaml_scalar_is_plain(&event));

    yaml_event_delete(&event);
}
END_TEST

START_TEST(ys_single_quoted)
{
    const unsigned char INPUT[] = "'abcdefgh'";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert_str_eq(yaml_scalar_value(&event), "abcdefgh");
    ck_assert_uint_eq(yaml_scalar_length(&event), 8);
    ck_assert_int_eq(yaml_scalar_style(&event),
                     YAML_SINGLE_QUOTED_SCALAR_STYLE);

    yaml_event_delete(&event);
}
END_TEST

START_TEST(ys_double_quoted)
{
    const unsigned char INPUT[] = "\"abcdefgh\"";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert_str_eq(yaml_scalar_value(&event), "abcdefgh");
    ck_assert_uint_eq(yaml_scalar_length(&event), 8);
    ck_assert_int_eq(yaml_scalar_style(&event),
                     YAML_DOUBLE_QUOTED_SCALAR_STYLE);

    yaml_event_delete(&event);
}
END_TEST

/*----------------------------------------------------------------------------*
 |                             yaml_parse_null()                              |
 *----------------------------------------------------------------------------*/

static const char *VALID_NULLS[] = {
    "---",
    "~",
    "null",
    "Null",
    "NULL",
    "!!null",
    "!!null \"whatever\"",
};

START_TEST(ypn_valid)
{
    const char *INPUT = VALID_NULLS[_i];
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_null(&event));

    yaml_event_delete(&event);
}
END_TEST

static const char *INVALID_NULLS[] = {
    /* Not plain */
    "\"\"",
    "\"~\"",
    "'null'",
    /* Typos */
    "0",
    "~~",
    "nill",
    "Nill",
    "Nul",
    "NUL",
    /* Bad tag */
    "!!nul",
    "!!binary MDEy",
};

START_TEST(ypn_invalid)
{
    const char *INPUT = INVALID_NULLS[_i];
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_null(&event));
    ck_assert_int_eq(errno, EINVAL);

    yaml_event_delete(&event);
}
END_TEST

/*----------------------------------------------------------------------------*
 |                            yaml_parse_boolean()                            |
 *----------------------------------------------------------------------------*/

static const char *TRUES[] = {
    "y",
    "Y",
    "yes",
    "Yes",
    "YES",
    "true",
    "True",
    "TRUE",
    "on",
    "On",
    "ON",
    "!!bool 'y'",
};

START_TEST(ypbo_true)
{
    const char *INPUT = TRUES[_i];
    yaml_event_t event;
    bool b;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_boolean(&event, &b));
    ck_assert(b);

    yaml_event_delete(&event);
}
END_TEST

static const char *FALSES[] = {
    "n",
    "N",
    "no",
    "No",
    "NO",
    "false",
    "False",
    "FALSE",
    "off",
    "Off",
    "OFF",
    "!!bool \"n\"",
};

START_TEST(ypbo_false)
{
    const char *INPUT = FALSES[_i];
    yaml_event_t event;
    bool b;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_boolean(&event, &b));
    ck_assert(!b);

    yaml_event_delete(&event);
}
END_TEST

static const char *INVALID_BOOLEANS[] = {
    /* Not plain */
    "\"y\"",
    "'n'",
    /* Typos */
    "0",
    "FALS",
    "Fals",
    "Folse",
    "fals",
    "NOO",
    "Noo",
    "Ni",
    "noo",
    "ni",
    "OF",
    "Of",
    "ONN",
    "Onn",
    "Oy",
    "of",
    "onn",
    "oy",
    "TRU",
    "Tru",
    "Ttrue",
    "ttrue",
    "YE",
    "Ye",
    "Yas",
    "ye",
    "yas",
    /* Bad tag */
    "!!boolean y",
    "!!null",
};

START_TEST(ypbo_invalid)
{
    const char *INPUT = INVALID_BOOLEANS[_i];
    yaml_event_t event;
    bool b;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_boolean(&event, &b));
    ck_assert_int_eq(errno, EINVAL);

    yaml_event_delete(&event);
}
END_TEST

/*----------------------------------------------------------------------------*
 |                            yaml_parse_integer()                            |
 *----------------------------------------------------------------------------*/

static const char *ZEROS[] = {
    "0",
    "00",
    "0x0",
    "!!int 0",
};

START_TEST(ypi_zero)
{
    const char *INPUT = ZEROS[_i];
    yaml_event_t event;
    intmax_t i;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_integer(&event, &i));
    ck_assert_int_eq(i, 0);

    yaml_event_delete(&event);
}
END_TEST

static const char *SIXTEENS[] = {
    "+16",
    "020",
    "0x10",
    "!!int '020'",
};

START_TEST(ypi_sixteen)
{
    const char *INPUT = SIXTEENS[_i];
    yaml_event_t event;
    intmax_t i;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_integer(&event, &i));
    ck_assert_int_eq(i, 16);

    yaml_event_delete(&event);
}
END_TEST

static const char *MINUS_SIXTEENS[] = {
    "-16",
    "-020",
    "-0x10",
    "!!int \"-0x10\"",
};

START_TEST(ypi_minus_sixteen)
{
    const char *INPUT = MINUS_SIXTEENS[_i];
    yaml_event_t event;
    intmax_t i;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_integer(&event, &i));
    ck_assert_int_eq(i, -16);

    yaml_event_delete(&event);
}
END_TEST

START_TEST(ypi_min_base10)
{
    yaml_event_t event;
    char *input;
    intmax_t i;
    int size;

    size = asprintf(&input, "%"PRIiMAX, INTMAX_MIN);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_integer(&event, &i));
    ck_assert_int_eq(i, INTMAX_MIN);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypi_min_base8)
{
    yaml_event_t event;
    char *input;
    intmax_t i;
    int size;

    size = asprintf(&input, "-0%"PRIoMAX, INTMAX_MIN);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_integer(&event, &i));
    ck_assert_int_eq(i, INTMAX_MIN);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypi_min_base16)
{
    yaml_event_t event;
    char *input;
    intmax_t i;
    int size;

    size = asprintf(&input, "-0x%"PRIxMAX, INTMAX_MIN);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_integer(&event, &i));
    ck_assert_int_eq(i, INTMAX_MIN);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypi_max_base10)
{
    yaml_event_t event;
    char *input;
    intmax_t i;
    int size;

    size = asprintf(&input, "%"PRIiMAX, INTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_integer(&event, &i));
    ck_assert_int_eq(i, INTMAX_MAX);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypi_max_base8)
{
    yaml_event_t event;
    char *input;
    intmax_t i;
    int size;

    size = asprintf(&input, "0%"PRIoMAX, INTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_integer(&event, &i));
    ck_assert_int_eq(i, INTMAX_MAX);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypi_max_base16)
{
    yaml_event_t event;
    char *input;
    intmax_t i;
    int size;

    size = asprintf(&input, "0x%"PRIxMAX, INTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_integer(&event, &i));
    ck_assert_int_eq(i, INTMAX_MAX);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

static const char *INVALID_INTEGERS[] = {
    /* No tag, not plain */
    "'0'",
    "\"0\"",
    /* Bad tag */
    "!!integer 0",
    "!!i 1",
    "!!bool 0",
    /* Empty */
    "!!int",
    /* Bad base 10 */
    "a",
    /* Bad base 8 */
    "09",
    /* Bad base 16 */
    "0x",
    "0xg",
    "f",
    /* Not a number */
    "~",
    "test",
    /* Not only a number */
    "0test",
    "0 test",
    /* Not only one number */
    "0 1 2 3",
};

START_TEST(ypi_invalid)
{
    const char *INPUT = INVALID_INTEGERS[_i];
    yaml_event_t event;
    intmax_t i;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_integer(&event, &i));
    ck_assert_int_eq(errno, EINVAL);

    yaml_event_delete(&event);
}
END_TEST

START_TEST(ypi_too_little_base10)
{
    yaml_event_t event;
    char *input;
    intmax_t i;
    int size;

    size = asprintf(&input, "%"PRIiMAX"0", INTMAX_MIN);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_integer(&event, &i));
    ck_assert_int_eq(errno, ERANGE);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypi_too_little_base8)
{
    yaml_event_t event;
    char *input;
    intmax_t i;
    int size;

    size = asprintf(&input, "-0%"PRIoMAX"0", INTMAX_MIN);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_integer(&event, &i));
    ck_assert_int_eq(errno, ERANGE);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypi_too_little_base16)
{
    yaml_event_t event;
    char *input;
    intmax_t i;
    int size;

    size = asprintf(&input, "-0x%"PRIxMAX"0", INTMAX_MIN);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_integer(&event, &i));
    ck_assert_int_eq(errno, ERANGE);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypi_too_big_base10)
{
    yaml_event_t event;
    char *input;
    intmax_t i;
    int size;

    size = asprintf(&input, "%"PRIiMAX"0", INTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_integer(&event, &i));
    ck_assert_int_eq(errno, ERANGE);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypi_too_big_base8)
{
    yaml_event_t event;
    char *input;
    intmax_t i;
    int size;

    size = asprintf(&input, "0%"PRIoMAX"0", INTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_integer(&event, &i));
    ck_assert_int_eq(errno, ERANGE);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypi_too_big_base16)
{
    yaml_event_t event;
    char *input;
    intmax_t i;
    int size;

    size = asprintf(&input, "0x%"PRIxMAX"0", INTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_integer(&event, &i));
    ck_assert_int_eq(errno, ERANGE);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

/*----------------------------------------------------------------------------*
 |                       yaml_parse_unsigned_integer()                        |
 *----------------------------------------------------------------------------*/

START_TEST(ypui_zero)
{
    const char *INPUT = ZEROS[_i];
    yaml_event_t event;
    uintmax_t u;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_unsigned_integer(&event, &u));
    ck_assert_uint_eq(u, 0);

    yaml_event_delete(&event);
}
END_TEST

START_TEST(ypui_sixteen)
{
    const char *INPUT = SIXTEENS[_i];
    yaml_event_t event;
    uintmax_t u;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_unsigned_integer(&event, &u));
    ck_assert_uint_eq(u, 16);

    yaml_event_delete(&event);
}
END_TEST

START_TEST(ypui_minus_sixteen)
{
    const char *INPUT = MINUS_SIXTEENS[_i];
    yaml_event_t event;
    uintmax_t u;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_unsigned_integer(&event, &u));
    ck_assert_uint_eq(u, -16);

    yaml_event_delete(&event);
}
END_TEST

START_TEST(ypui_min_base10)
{
    yaml_event_t event;
    char *input;
    uintmax_t u;
    int size;

    size = asprintf(&input, "-%"PRIuMAX, UINTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_unsigned_integer(&event, &u));
    ck_assert_uint_eq(u, 1);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypui_min_base8)
{
    yaml_event_t event;
    char *input;
    uintmax_t u;
    int size;

    size = asprintf(&input, "-0%"PRIoMAX, UINTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_unsigned_integer(&event, &u));
    ck_assert_uint_eq(u, 1);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypui_min_base16)
{
    yaml_event_t event;
    char *input;
    uintmax_t u;
    int size;

    size = asprintf(&input, "-0x%"PRIxMAX, UINTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_unsigned_integer(&event, &u));
    ck_assert_uint_eq(u, 1);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypui_max_base10)
{
    yaml_event_t event;
    char *input;
    uintmax_t u;
    int size;

    size = asprintf(&input, "%"PRIuMAX, UINTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_unsigned_integer(&event, &u));
    ck_assert_uint_eq(u, UINTMAX_MAX);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypui_max_base8)
{
    yaml_event_t event;
    char *input;
    uintmax_t u;
    int size;

    size = asprintf(&input, "0%"PRIoMAX, UINTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_unsigned_integer(&event, &u));
    ck_assert_uint_eq(u, UINTMAX_MAX);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypui_max_base16)
{
    yaml_event_t event;
    char *input;
    uintmax_t u;
    int size;

    size = asprintf(&input, "0x%"PRIxMAX, UINTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_unsigned_integer(&event, &u));
    ck_assert_uint_eq(u, UINTMAX_MAX);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

static const char *INVALID_UNSIGNED_INTEGERS[] = {
    /* No tag, not plain */
    "'0'",
    "\"0\"",
    /* Bad tag */
    "!!unsigned 0",
    "!!uint 1",
    "!!bool 0",
    /* Empty */
    "!!int",
    /* Bad base 10 */
    "a",
    /* Bad base 8 */
    "09",
    /* Bad base 16 */
    "0x",
    "f",
    /* Not a number */
    "~",
    "test",
    /* Not only a number */
    "0test",
    "0 test",
    /* Not only one number */
    "0 1 2 3",
};

START_TEST(ypui_invalid)
{
    const char *INPUT = INVALID_UNSIGNED_INTEGERS[_i];
    yaml_event_t event;
    uintmax_t u;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_unsigned_integer(&event, &u));
    ck_assert_int_eq(errno, EINVAL);

    yaml_event_delete(&event);
}
END_TEST

START_TEST(ypui_too_little_base10)
{
    yaml_event_t event;
    char *input;
    uintmax_t u;
    int size;

    size = asprintf(&input, "-%"PRIuMAX"0", UINTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_unsigned_integer(&event, &u));
    ck_assert_int_eq(errno, ERANGE);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypui_too_little_base8)
{
    yaml_event_t event;
    char *input;
    uintmax_t u;
    int size;

    size = asprintf(&input, "-0%"PRIoMAX"0", UINTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_unsigned_integer(&event, &u));
    ck_assert_int_eq(errno, ERANGE);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypui_too_little_base16)
{
    yaml_event_t event;
    char *input;
    uintmax_t u;
    int size;

    size = asprintf(&input, "-0x%"PRIxMAX"0", UINTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_unsigned_integer(&event, &u));
    ck_assert_int_eq(errno, ERANGE);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypui_too_big_base10)
{
    yaml_event_t event;
    char *input;
    uintmax_t u;
    int size;

    size = asprintf(&input, "%"PRIuMAX"0", UINTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_unsigned_integer(&event, &u));
    ck_assert_int_eq(errno, ERANGE);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypui_too_big_base8)
{
    yaml_event_t event;
    char *input;
    uintmax_t u;
    int size;

    size = asprintf(&input, "0%"PRIoMAX"0", UINTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_unsigned_integer(&event, &u));
    ck_assert_int_eq(errno, ERANGE);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

START_TEST(ypui_too_big_base16)
{
    yaml_event_t event;
    char *input;
    uintmax_t u;
    int size;

    size = asprintf(&input, "0x%"PRIxMAX"0", UINTMAX_MAX);
    ck_assert_int_ge(size, 0);

    yaml_parser_set_input_string(&parser, (const unsigned char *)input, size);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_unsigned_integer(&event, &u));
    ck_assert_int_eq(errno, ERANGE);

    yaml_event_delete(&event);
    free(input);
}
END_TEST

/*----------------------------------------------------------------------------*
 |                            yaml_parse_string()                             |
 *----------------------------------------------------------------------------*/

static const char *ABCDEFGS[] = {
    "abcdefg",
    "'abcdefg'",
    "\"abcdefg\"",
    "!!str abcdefg",
    "!!str 'abcdefg'",
    "!!str \"abcdefg\"",
};

START_TEST(yps_abcdefg)
{
    const char *INPUT = ABCDEFGS[_i];
    yaml_event_t event;
    const char *string;
    size_t size;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_string(&event, &string, &size));
    ck_assert_uint_eq(size, strlen(ABCDEFGS[0]));
    ck_assert_str_eq(string, ABCDEFGS[0]);

    yaml_event_delete(&event);
}
END_TEST

/* Any plain scalar can be a string */
static const char *VALID_STRINGS[] = {
    "~", /* Null */
    "y", /* Boolean */
    "0", /* Integer */
};

START_TEST(yps_valid_string)
{
    const char *INPUT = VALID_STRINGS[_i];
    yaml_event_t event;
    const char *string;
    size_t size;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_string(&event, &string, &size));
    ck_assert_uint_eq(size, strlen(INPUT));
    ck_assert_str_eq(string, INPUT);

    yaml_event_delete(&event);
}
END_TEST

START_TEST(yps_no_size)
{
    const unsigned char INPUT[] = "test";
    yaml_event_t event;
    const char *string;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_string(&event, &string, NULL));
    ck_assert_str_eq(string, (char *)INPUT);

    yaml_event_delete(&event);
}
END_TEST

static const char *INVALID_STRINGS[] = {
    /* Bad tag */
    "!!string test",
    "!!s test",
    "!!int test",
};

START_TEST(yps_invalid_string)
{
    const char *INPUT = INVALID_STRINGS[_i];
    yaml_event_t event;
    const char *string;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_string(&event, &string, NULL));
    ck_assert_int_eq(errno, EINVAL);

    yaml_event_delete(&event);
}
END_TEST

/*----------------------------------------------------------------------------*
 |                            yaml_parse_binary()                             |
 *----------------------------------------------------------------------------*/

static const char *BINARY_ABCDEFGS[] = {
    "YWJjZGVmZw==",
    "!!binary YWJjZGVmZw==",
    "!!binary 'YWJjZGVmZw=='",
    "!!binary \"YWJjZGVmZw==\"",
};

START_TEST(ypbi_abcdefg)
{
    const char *INPUT = BINARY_ABCDEFGS[_i];
    char binary[sizeof(ABCDEFGS[0]) - 1];
    yaml_event_t event;
    size_t size;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    ck_assert(yaml_parse_binary(&event, binary, &size));
    ck_assert_uint_eq(size, sizeof(binary));
    ck_assert_mem_eq(binary, ABCDEFGS[0], sizeof(binary));

    yaml_event_delete(&event);
}
END_TEST

static const char *INVALID_BINARIES[] = {
    /* Not plain */
    "'AAAA'",
    /* Bad tag */
    "!!bin AAAA",
    "!!str AAAA",
};

START_TEST(ypbi_invalid)
{
    const char *INPUT = INVALID_BINARIES[_i];
    yaml_event_t event;
    char binary[256];
    size_t size;

    yaml_parser_set_input_string(&parser, (const unsigned char *)INPUT,
                                 strlen(INPUT));

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_binary(&event, binary, &size));
    ck_assert_int_eq(errno, EINVAL);

    yaml_event_delete(&event);
}
END_TEST

START_TEST(ypbi_decoding_error)
{
    const unsigned char INPUT[] = "0";
    yaml_event_t event;
    char binary[256];
    size_t size;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert(yaml_parser_parse(&parser, &event));
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);

    errno = 0;
    ck_assert(!yaml_parse_binary(&event, binary, &size));
    ck_assert_int_eq(errno, EILSEQ);

    yaml_event_delete(&event);
}
END_TEST

static Suite *
unit_suite(void)
{
    Suite *suite;
    TCase *tests;

    suite = suite_create("parse");

    tests = tcase_create("yaml_mapping_tag");
    tcase_add_checked_fixture(tests, parser_init, parser_exit);
    tcase_add_test(tests, ymt_basic);
    tcase_add_test(tests, ymt_no_tag);

    suite_add_tcase(suite, tests);

    tests = tcase_create("yaml_scalar_*");
    tcase_add_checked_fixture(tests, parser_init, parser_exit);
    tcase_add_test(tests, ys_no_tag);
    tcase_add_test(tests, ys_tagged);
    tcase_add_test(tests, ys_plain);
    tcase_add_test(tests, ys_single_quoted);
    tcase_add_test(tests, ys_double_quoted);

    suite_add_tcase(suite, tests);

    tests = tcase_create("yaml_parse_null");
    tcase_add_checked_fixture(tests, parser_init, parser_exit);
    tcase_add_loop_test(tests, ypn_valid, 0, ARRAY_SIZE(VALID_NULLS));
    tcase_add_loop_test(tests, ypn_invalid, 0, ARRAY_SIZE(INVALID_NULLS));

    suite_add_tcase(suite, tests);

    tests = tcase_create("yaml_parse_boolean");
    tcase_add_checked_fixture(tests, parser_init, parser_exit);
    tcase_add_loop_test(tests, ypbo_true, 0, ARRAY_SIZE(TRUES));
    tcase_add_loop_test(tests, ypbo_false, 0, ARRAY_SIZE(FALSES));
    tcase_add_loop_test(tests, ypbo_invalid, 0, ARRAY_SIZE(INVALID_BOOLEANS));

    suite_add_tcase(suite, tests);

    tests = tcase_create("yaml_parse_integer");
    tcase_add_checked_fixture(tests, parser_init, parser_exit);
    tcase_add_loop_test(tests, ypi_zero, 0, ARRAY_SIZE(ZEROS));
    tcase_add_loop_test(tests, ypi_sixteen, 0, ARRAY_SIZE(SIXTEENS));
    tcase_add_loop_test(tests, ypi_minus_sixteen, 0,
                        ARRAY_SIZE(MINUS_SIXTEENS));
    tcase_add_test(tests, ypi_min_base10);
    tcase_add_test(tests, ypi_min_base8);
    tcase_add_test(tests, ypi_min_base16);
    tcase_add_test(tests, ypi_max_base10);
    tcase_add_test(tests, ypi_max_base8);
    tcase_add_test(tests, ypi_max_base16);
    tcase_add_loop_test(tests, ypi_invalid, 0, ARRAY_SIZE(INVALID_INTEGERS));
    tcase_add_test(tests, ypi_too_little_base10);
    tcase_add_test(tests, ypi_too_little_base8);
    tcase_add_test(tests, ypi_too_little_base16);
    tcase_add_test(tests, ypi_too_big_base10);
    tcase_add_test(tests, ypi_too_big_base8);
    tcase_add_test(tests, ypi_too_big_base16);

    suite_add_tcase(suite, tests);

    tests = tcase_create("yaml_parse_unsigned_integer");
    tcase_add_checked_fixture(tests, parser_init, parser_exit);
    tcase_add_loop_test(tests, ypui_zero, 0, ARRAY_SIZE(ZEROS));
    tcase_add_loop_test(tests, ypui_sixteen, 0, ARRAY_SIZE(SIXTEENS));
    tcase_add_loop_test(tests, ypui_minus_sixteen, 0,
                        ARRAY_SIZE(MINUS_SIXTEENS));
    tcase_add_test(tests, ypui_min_base10);
    tcase_add_test(tests, ypui_min_base8);
    tcase_add_test(tests, ypui_min_base16);
    tcase_add_test(tests, ypui_max_base10);
    tcase_add_test(tests, ypui_max_base8);
    tcase_add_test(tests, ypui_max_base16);
    tcase_add_loop_test(tests, ypui_invalid, 0,
                        ARRAY_SIZE(INVALID_UNSIGNED_INTEGERS));
    tcase_add_test(tests, ypui_too_little_base10);
    tcase_add_test(tests, ypui_too_little_base8);
    tcase_add_test(tests, ypui_too_little_base16);
    tcase_add_test(tests, ypui_too_big_base10);
    tcase_add_test(tests, ypui_too_big_base8);
    tcase_add_test(tests, ypui_too_big_base16);

    suite_add_tcase(suite, tests);

    tests = tcase_create("yaml_parse_string");
    tcase_add_checked_fixture(tests, parser_init, parser_exit);
    tcase_add_loop_test(tests, yps_abcdefg, 0, ARRAY_SIZE(ABCDEFGS));
    tcase_add_loop_test(tests, yps_valid_string, 0, ARRAY_SIZE(VALID_STRINGS));
    tcase_add_test(tests, yps_no_size);
    tcase_add_loop_test(tests, yps_invalid_string, 0,
                        ARRAY_SIZE(INVALID_STRINGS));

    suite_add_tcase(suite, tests);

    tests = tcase_create("yaml_parse_binary");
    tcase_add_checked_fixture(tests, parser_init, parser_exit);
    tcase_add_loop_test(tests, ypbi_abcdefg, 0, ARRAY_SIZE(BINARY_ABCDEFGS));
    tcase_add_loop_test(tests, ypbi_invalid, 0, ARRAY_SIZE(INVALID_BINARIES));
    tcase_add_test(tests, ypbi_decoding_error);

    suite_add_tcase(suite, tests);

    return suite;
}

int
main(void)
{
    int number_failed;
    SRunner *runner;
    Suite *suite;

    suite = unit_suite();
    runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
