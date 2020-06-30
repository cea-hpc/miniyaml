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

/*----------------------------------------------------------------------------*
 |                           yaml_emit_mapping_*()                            |
 *----------------------------------------------------------------------------*/

START_TEST(yem_no_tag)
{
    const char EXPECTED_OUTPUT[] = "---\n"
                                   "~: ~\n"
                                   "...\n";
    unsigned char output[sizeof(EXPECTED_OUTPUT)] = {};
    size_t written = 0;

    yaml_emitter_set_output_string(&emitter, output, sizeof(output), &written);
    ck_assert(yaml_emit_stream_start(&emitter, YAML_UTF8_ENCODING));
    ck_assert(yaml_emit_document_start(&emitter));

    ck_assert(yaml_emit_mapping_start(&emitter, NULL));
    ck_assert(yaml_emit_null(&emitter));
    ck_assert(yaml_emit_null(&emitter));
    ck_assert(yaml_emit_mapping_end(&emitter));

    ck_assert(yaml_emit_document_end(&emitter));
    ck_assert(yaml_emit_stream_end(&emitter));
    ck_assert(yaml_emitter_flush(&emitter));

    ck_assert_uint_eq(written, sizeof(output) - 1);
    ck_assert_str_eq((char *)output, EXPECTED_OUTPUT);
}
END_TEST

START_TEST(yem_tagged)
{
    const char EXPECTED_OUTPUT[] = "--- !test\n"
                                   "~: ~\n"
                                   "...\n";
    unsigned char output[sizeof(EXPECTED_OUTPUT)] = {};
    size_t written = 0;

    yaml_emitter_set_output_string(&emitter, output, sizeof(output), &written);

    ck_assert(yaml_emit_stream_start(&emitter, YAML_UTF8_ENCODING));
    ck_assert(yaml_emit_document_start(&emitter));

    ck_assert(yaml_emit_mapping_start(&emitter, "!test"));
    ck_assert(yaml_emit_null(&emitter));
    ck_assert(yaml_emit_null(&emitter));
    ck_assert(yaml_emit_mapping_end(&emitter));

    ck_assert(yaml_emit_document_end(&emitter));
    ck_assert(yaml_emit_stream_end(&emitter));
    ck_assert(yaml_emitter_flush(&emitter));

    ck_assert_uint_eq(written, sizeof(output) - 1);
    ck_assert_str_eq((char *)output, EXPECTED_OUTPUT);
}
END_TEST

/*----------------------------------------------------------------------------*
 |                           yaml_emit_sequence_*()                           |
 *----------------------------------------------------------------------------*/

START_TEST(yese_no_tag)
{
    const char EXPECTED_OUTPUT[] = "---\n"
                                   "- ~\n"
                                   "...\n";
    unsigned char output[sizeof(EXPECTED_OUTPUT)] = {};
    size_t written = 0;

    yaml_emitter_set_output_string(&emitter, output, sizeof(output), &written);

    ck_assert(yaml_emit_stream_start(&emitter, YAML_UTF8_ENCODING));
    ck_assert(yaml_emit_document_start(&emitter));

    ck_assert(yaml_emit_sequence_start(&emitter, NULL));
    ck_assert(yaml_emit_null(&emitter));
    ck_assert(yaml_emit_sequence_end(&emitter));

    ck_assert(yaml_emit_document_end(&emitter));
    ck_assert(yaml_emit_stream_end(&emitter));
    ck_assert(yaml_emitter_flush(&emitter));

    ck_assert_uint_eq(written, sizeof(output) - 1);
    ck_assert_str_eq((char *)output, EXPECTED_OUTPUT);
}
END_TEST

START_TEST(yese_tagged)
{
    const char EXPECTED_OUTPUT[] = "--- !test\n"
                                   "- ~\n"
                                   "...\n";
    unsigned char output[sizeof(EXPECTED_OUTPUT)] = {};
    size_t written = 0;

    yaml_emitter_set_output_string(&emitter, output, sizeof(output), &written);

    ck_assert(yaml_emit_stream_start(&emitter, YAML_UTF8_ENCODING));
    ck_assert(yaml_emit_document_start(&emitter));

    ck_assert(yaml_emit_sequence_start(&emitter, "!test"));
    ck_assert(yaml_emit_null(&emitter));
    ck_assert(yaml_emit_sequence_end(&emitter));

    ck_assert(yaml_emit_document_end(&emitter));
    ck_assert(yaml_emit_stream_end(&emitter));
    ck_assert(yaml_emitter_flush(&emitter));

    ck_assert_uint_eq(written, sizeof(output) - 1);
    ck_assert_str_eq((char *)output, EXPECTED_OUTPUT);
}
END_TEST

/*----------------------------------------------------------------------------*
 |                             yaml_emit_scalar()                             |
 *----------------------------------------------------------------------------*/

START_TEST(yesc_no_tag_plain)
{
    const char EXPECTED_OUTPUT[] = "--- test\n"
                                   "...\n";
    unsigned char output[sizeof(EXPECTED_OUTPUT)] = {};
    size_t written = 0;

    yaml_emitter_set_output_string(&emitter, output, sizeof(output), &written);

    ck_assert(yaml_emit_stream_start(&emitter, YAML_UTF8_ENCODING));
    ck_assert(yaml_emit_document_start(&emitter));

    ck_assert(yaml_emit_scalar(&emitter, NULL, "test", 4,
                               YAML_PLAIN_SCALAR_STYLE));

    ck_assert(yaml_emit_document_end(&emitter));
    ck_assert(yaml_emit_stream_end(&emitter));
    ck_assert(yaml_emitter_flush(&emitter));

    ck_assert_uint_eq(written, sizeof(output) - 1);
    ck_assert_str_eq((char *)output, EXPECTED_OUTPUT);
}
END_TEST

START_TEST(yesc_no_tag_quoted)
{
    const char EXPECTED_OUTPUT[] = "--- \"test\"\n"
                                   "...\n";
    unsigned char output[sizeof(EXPECTED_OUTPUT)] = {};
    size_t written = 0;

    yaml_emitter_set_output_string(&emitter, output, sizeof(output), &written);

    ck_assert(yaml_emit_stream_start(&emitter, YAML_UTF8_ENCODING));
    ck_assert(yaml_emit_document_start(&emitter));

    ck_assert(yaml_emit_scalar(&emitter, NULL, "test", 4,
                               YAML_DOUBLE_QUOTED_SCALAR_STYLE));

    ck_assert(yaml_emit_document_end(&emitter));
    ck_assert(yaml_emit_stream_end(&emitter));
    ck_assert(yaml_emitter_flush(&emitter));

    ck_assert_uint_eq(written, sizeof(output) - 1);
    ck_assert_str_eq((char *)output, EXPECTED_OUTPUT);
}
END_TEST

START_TEST(yesc_tagged_plain)
{
    const char EXPECTED_OUTPUT[] = "--- !tag test\n"
                                   "...\n";
    unsigned char output[sizeof(EXPECTED_OUTPUT)] = {};
    size_t written = 0;

    yaml_emitter_set_output_string(&emitter, output, sizeof(output), &written);

    ck_assert(yaml_emit_stream_start(&emitter, YAML_UTF8_ENCODING));
    ck_assert(yaml_emit_document_start(&emitter));

    ck_assert(yaml_emit_scalar(&emitter, "!tag", "test", 4,
                               YAML_PLAIN_SCALAR_STYLE));

    ck_assert(yaml_emit_document_end(&emitter));
    ck_assert(yaml_emit_stream_end(&emitter));
    ck_assert(yaml_emitter_flush(&emitter));

    ck_assert_uint_eq(written, sizeof(output) - 1);
    ck_assert_str_eq((char *)output, EXPECTED_OUTPUT);
}
END_TEST

START_TEST(yesc_tagged_quoted)
{
    const char EXPECTED_OUTPUT[] = "--- !tag \"test\"\n"
                                   "...\n";
    unsigned char output[sizeof(EXPECTED_OUTPUT)] = {};
    size_t written = 0;

    yaml_emitter_set_output_string(&emitter, output, sizeof(output), &written);

    ck_assert(yaml_emit_stream_start(&emitter, YAML_UTF8_ENCODING));
    ck_assert(yaml_emit_document_start(&emitter));

    ck_assert(yaml_emit_scalar(&emitter, "!tag", "test", 4,
                               YAML_DOUBLE_QUOTED_SCALAR_STYLE));

    ck_assert(yaml_emit_document_end(&emitter));
    ck_assert(yaml_emit_stream_end(&emitter));
    ck_assert(yaml_emitter_flush(&emitter));

    ck_assert_uint_eq(written, sizeof(output) - 1);
    ck_assert_str_eq((char *)output, EXPECTED_OUTPUT);
}
END_TEST

/*----------------------------------------------------------------------------*
 |                              yaml_emit_null()                              |
 *----------------------------------------------------------------------------*/

START_TEST(yen_basic)
{
    const char EXPECTED_OUTPUT[] = "--- ~\n"
                                   "...\n";
    unsigned char output[sizeof(EXPECTED_OUTPUT)] = {};
    size_t written = 0;

    yaml_emitter_set_output_string(&emitter, output, sizeof(output), &written);

    ck_assert(yaml_emit_stream_start(&emitter, YAML_UTF8_ENCODING));
    ck_assert(yaml_emit_document_start(&emitter));

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

    tests = tcase_create("yaml_emit_mapping_*");
    tcase_add_checked_fixture(tests, emitter_init, emitter_exit);
    tcase_add_test(tests, yem_no_tag);
    tcase_add_test(tests, yem_tagged);

    suite_add_tcase(suite, tests);

    tests = tcase_create("yaml_emit_sequence_*");
    tcase_add_checked_fixture(tests, emitter_init, emitter_exit);
    tcase_add_test(tests, yese_no_tag);
    tcase_add_test(tests, yese_tagged);

    suite_add_tcase(suite, tests);

    tests = tcase_create("yaml_emit_scalar");
    tcase_add_checked_fixture(tests, emitter_init, emitter_exit);
    tcase_add_test(tests, yesc_no_tag_plain);
    tcase_add_test(tests, yesc_no_tag_quoted);
    tcase_add_test(tests, yesc_tagged_plain);
    tcase_add_test(tests, yesc_tagged_quoted);

    suite_add_tcase(suite, tests);

    tests = tcase_create("yaml_emit_null");
    tcase_add_checked_fixture(tests, emitter_init, emitter_exit);
    tcase_add_test(tests, yen_basic);

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
