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
    "!!Null",
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

START_TEST(ypb_true)
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

START_TEST(ypb_false)
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
};

START_TEST(ypb_invalid)
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
    tcase_add_loop_test(tests, ypb_true, 0, ARRAY_SIZE(TRUES));
    tcase_add_loop_test(tests, ypb_false, 0, ARRAY_SIZE(FALSES));
    tcase_add_loop_test(tests, ypb_invalid, 0, ARRAY_SIZE(INVALID_BOOLEANS));

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
