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

#include <miniyaml.h>

static yaml_emitter_t emitter;

static void
emitter_init(void)
{
    ck_assert(yaml_emitter_initialize(&emitter));
}

static void
emitter_exit(void)
{
    yaml_emitter_delete(&emitter);
}

/*----------------------------------------------------------------------------*
 |                            yaml_emit_stream_*()                            |
 *----------------------------------------------------------------------------*/

START_TEST(yestre_basic)
{
    const char EXPECTED_OUTPUT[] = "";
    unsigned char output[sizeof(EXPECTED_OUTPUT)] = {};
    size_t written = 0;

    yaml_emitter_set_output_string(&emitter, output, sizeof(output), &written);

    ck_assert(yaml_emit_stream_start(&emitter, YAML_UTF8_ENCODING));
    ck_assert(yaml_emit_stream_end(&emitter));

    ck_assert(yaml_emitter_flush(&emitter));

    ck_assert_uint_eq(written, sizeof(EXPECTED_OUTPUT) - 1);
    ck_assert_str_eq((char *)output, EXPECTED_OUTPUT);
}
END_TEST

/*----------------------------------------------------------------------------*
 |                           yaml_emit_document_*()                           |
 *----------------------------------------------------------------------------*/

START_TEST(yed_basic)
{
    const char EXPECTED_OUTPUT[] = "--- ~\n"
                                   "...\n";
    unsigned char output[sizeof(EXPECTED_OUTPUT)] = {};
    size_t written = 0;

    yaml_emitter_set_output_string(&emitter, output, sizeof(output), &written);

    ck_assert(yaml_emit_stream_start(&emitter, YAML_UTF8_ENCODING));

    ck_assert(yaml_emit_document_start(&emitter));
    /* Empty documents are not allowed */
    ck_assert(yaml_emit_null(&emitter));
    ck_assert(yaml_emit_document_end(&emitter));

    ck_assert(yaml_emit_stream_end(&emitter));
    ck_assert(yaml_emitter_flush(&emitter));

    ck_assert_uint_eq(written, sizeof(output) - 1);
    ck_assert_str_eq((char *)output, EXPECTED_OUTPUT);
}
END_TEST

static Suite *
unit_suite(void)
{
    Suite *suite;
    TCase *tests;

    suite = suite_create("emit");

    tests = tcase_create("yaml_emit_stream_*");
    tcase_add_checked_fixture(tests, emitter_init, emitter_exit);
    tcase_add_test(tests, yestre_basic);

    suite_add_tcase(suite, tests);

    tests = tcase_create("yaml_emit_document_*");
    tcase_add_checked_fixture(tests, emitter_init, emitter_exit);
    tcase_add_test(tests, yed_basic);

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
