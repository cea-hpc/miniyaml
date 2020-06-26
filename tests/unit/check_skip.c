/* This file is part of rbh-fsevents
 * Copyright (C) 2019 Commissariat a l'energie atomique et aux energies
 *                    alternatives
 *
 * SPDX-License-Identifer: LGPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <check.h>

#include <miniyaml.h>

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

static yaml_parser_t parser;

static void
parser_init(void)
{
    ck_assert_int_ne(yaml_parser_initialize(&parser), 0);
    yaml_parser_set_encoding(&parser, YAML_UTF8_ENCODING);
}

static void
parser_exit(void)
{
    yaml_parser_delete(&parser);
}

static void
skip_stream_start(yaml_parser_t *parser)
{
    yaml_event_t event;

    ck_assert_int_ne(yaml_parser_parse(parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);
}

/*----------------------------------------------------------------------------*
 |                             yaml_parser_skip()                             |
 *----------------------------------------------------------------------------*/

static const yaml_event_type_t yps_noop_event_types[] = {
    YAML_NO_EVENT,
    YAML_STREAM_END_EVENT,
    YAML_ALIAS_EVENT,
    YAML_SCALAR_EVENT,
    YAML_SEQUENCE_END_EVENT,
    YAML_MAPPING_END_EVENT,
};

START_TEST(yps_noop)
{
    const unsigned char INPUT[] = "";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    ck_assert_int_ne(yaml_parser_skip(&parser, yps_noop_event_types[_i]), 0);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);
}
END_TEST

START_TEST(yps_stream_empty)
{
    const unsigned char INPUT[] = "";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    skip_stream_start(&parser);
    ck_assert_int_ne(yaml_parser_skip(&parser, YAML_STREAM_START_EVENT), 0);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_NO_EVENT);
    yaml_event_delete(&event);
}
END_TEST

START_TEST(yps_stream_not_empty)
{
    const unsigned char INPUT[] = "---\n"
                                  "~\n"
                                  "...";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    skip_stream_start(&parser);
    ck_assert_int_ne(yaml_parser_skip(&parser, YAML_STREAM_START_EVENT), 0);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_NO_EVENT);
    yaml_event_delete(&event);
}
END_TEST

START_TEST(yps_one_document)
{
    const unsigned char INPUT[] = "---\n"
                                  "...";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    skip_stream_start(&parser);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_skip(&parser, YAML_DOCUMENT_START_EVENT), 0);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_STREAM_END_EVENT);
    yaml_event_delete(&event);
}
END_TEST

START_TEST(yps_two_documents)
{
    const unsigned char INPUT[] = "---\n"
                                  "...\n"
                                  "---\n"
                                  "...";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    skip_stream_start(&parser);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_skip(&parser, YAML_DOCUMENT_START_EVENT), 0);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);
}
END_TEST

START_TEST(yps_mapping_basic)
{
    const unsigned char INPUT[] = "---\n"
                                  "{}\n"
                                  "...";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    skip_stream_start(&parser);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_MAPPING_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_skip(&parser, YAML_MAPPING_START_EVENT), 0);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_END_EVENT);
    yaml_event_delete(&event);
}
END_TEST

START_TEST(yps_nested_mappings_outer)
{
    const unsigned char INPUT[] = "---\n"
                                  "{~: {}}\n"
                                  "...";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    skip_stream_start(&parser);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_MAPPING_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_skip(&parser, YAML_MAPPING_START_EVENT), 0);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_END_EVENT);
    yaml_event_delete(&event);
}
END_TEST

START_TEST(yps_nested_mappings_inner)
{
    const unsigned char INPUT[] = "---\n"
                                  "{~: {}}\n"
                                  "...";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    skip_stream_start(&parser);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_MAPPING_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_SCALAR_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_MAPPING_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_skip(&parser, YAML_MAPPING_START_EVENT), 0);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_MAPPING_END_EVENT);
    yaml_event_delete(&event);
}
END_TEST

START_TEST(yps_sequence_basic)
{
    const unsigned char INPUT[] = "---\n"
                                  "[]\n"
                                  "...";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    skip_stream_start(&parser);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_SEQUENCE_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_skip(&parser, YAML_SEQUENCE_START_EVENT), 0);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_END_EVENT);
    yaml_event_delete(&event);
}
END_TEST

START_TEST(yps_nested_sequences_outer)
{
    const unsigned char INPUT[] = "---\n"
                                  "[[]]\n"
                                  "...";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    skip_stream_start(&parser);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_SEQUENCE_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_skip(&parser, YAML_SEQUENCE_START_EVENT), 0);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_END_EVENT);
    yaml_event_delete(&event);
}
END_TEST

START_TEST(yps_nested_sequences_inner)
{
    const unsigned char INPUT[] = "---\n"
                                  "[[]]\n"
                                  "...";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    skip_stream_start(&parser);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_SEQUENCE_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_SEQUENCE_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_skip(&parser, YAML_SEQUENCE_START_EVENT), 0);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_SEQUENCE_END_EVENT);
    yaml_event_delete(&event);
}
END_TEST

START_TEST(yps_mapping_in_sequence)
{
    const unsigned char INPUT[] = "---\n"
                                  "[{}]\n"
                                  "...";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    skip_stream_start(&parser);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_SEQUENCE_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_skip(&parser, YAML_SEQUENCE_START_EVENT), 0);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_END_EVENT);
    yaml_event_delete(&event);
}
END_TEST

START_TEST(yps_sequence_in_mapping)
{
    const unsigned char INPUT[] = "---\n"
                                  "{[]: []}\n"
                                  "...";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    skip_stream_start(&parser);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_MAPPING_START_EVENT);
    yaml_event_delete(&event);

    ck_assert_int_ne(yaml_parser_skip(&parser, YAML_MAPPING_START_EVENT), 0);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_DOCUMENT_END_EVENT);
    yaml_event_delete(&event);
}
END_TEST

START_TEST(yps_error)
{
    const unsigned char INPUT[] = ":";

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);
    skip_stream_start(&parser);

    ck_assert_int_eq(yaml_parser_skip(&parser, YAML_STREAM_START_EVENT), 0);
}
END_TEST

/*----------------------------------------------------------------------------*
 |                          yaml_parser_skip_next()                           |
 *----------------------------------------------------------------------------*/

START_TEST(ypsn_basic)
{
    const unsigned char INPUT[] = "";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    ck_assert_int_ne(yaml_parser_skip_next(&parser), 0);

    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    ck_assert_int_eq(event.type, YAML_NO_EVENT);
    yaml_event_delete(&event);
}
END_TEST

START_TEST(ypsn_error)
{
    const unsigned char INPUT[] = ":";
    yaml_event_t event;

    yaml_parser_set_input_string(&parser, INPUT, sizeof(INPUT) - 1);

    /* YAML_STREAM_START_EVENT */
    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    yaml_event_delete(&event);
    /* YAML_DOCUMENT_START_EVENT */
    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    yaml_event_delete(&event);
    /* YAML_MAPPING_START_EVENT */
    ck_assert_int_ne(yaml_parser_parse(&parser, &event), 0);
    yaml_event_delete(&event);

    ck_assert_int_eq(yaml_parser_skip_next(&parser), 0);
}
END_TEST

static Suite *
unit_suite(void)
{
    Suite *suite;
    TCase *tests;

    suite = suite_create("skip");

    tests = tcase_create("yaml_parser_skip");
    tcase_add_checked_fixture(tests, parser_init, parser_exit);
    tcase_add_loop_test(tests, yps_noop, 0, ARRAY_SIZE(yps_noop_event_types));
    tcase_add_test(tests, yps_stream_empty);
    tcase_add_test(tests, yps_stream_not_empty);
    tcase_add_test(tests, yps_one_document);
    tcase_add_test(tests, yps_two_documents);
    tcase_add_test(tests, yps_mapping_basic);
    tcase_add_test(tests, yps_nested_mappings_outer);
    tcase_add_test(tests, yps_nested_mappings_inner);
    tcase_add_test(tests, yps_sequence_basic);
    tcase_add_test(tests, yps_nested_sequences_outer);
    tcase_add_test(tests, yps_nested_sequences_inner);
    tcase_add_test(tests, yps_mapping_in_sequence);
    tcase_add_test(tests, yps_sequence_in_mapping);
    tcase_add_test(tests, yps_error);

    suite_add_tcase(suite, tests);

    tests = tcase_create("yaml_parser_skip_next");
    tcase_add_checked_fixture(tests, parser_init, parser_exit);
    tcase_add_test(tests, ypsn_basic);
    tcase_add_test(tests, ypsn_error);

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
