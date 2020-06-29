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

/* Testing yaml_mapping_tag() is of very little value...
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
