/* This file is part of MiniYAML
 * Copyright (C) 2019 Commissariat a l'energie atomique et aux energies
 *                    alternatives
 *
 * SPDX-License-Identifer: LGPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include <check.h>

#include "../../src/base64.h"

/*----------------------------------------------------------------------------*
 |                              base64_encode()                               |
 *----------------------------------------------------------------------------*/

START_TEST(be_nothing)
{
    char c;

    ck_assert_uint_eq(base64_encode(&c, NULL, 0), 0);
    ck_assert_int_eq(c, 0);
}
END_TEST

static const char CHARS[256] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
    0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
    0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
};

static const char BASE64[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/',
};

START_TEST(be_c0)
{
    char expected_output[5] = {0, BASE64[0], '=', '='};
    char encoded[5];

    for (size_t i = 0; i < 64; i++) {
        ck_assert_uint_eq(base64_encode(encoded, &CHARS[i * 4], 1), 4);

        expected_output[0] = BASE64[i];
        ck_assert_str_eq(encoded, expected_output);
    }
}
END_TEST

START_TEST(be_c1)
{
    char expected_output[5] = {BASE64[0], 0, BASE64[0], '='};
    char encoded[5];
    char input[2];

    for (size_t i = 0; i < 4; i++) {
        input[0] = CHARS[i];
        for (size_t j = 0; j < 16; j++) {
            input[1] = CHARS[j * 16];

            ck_assert_uint_eq(base64_encode(encoded, input, sizeof(input)), 4);

            expected_output[1] = BASE64[i * 16 + j];
            ck_assert_str_eq(encoded, expected_output);
        }
    }
}
END_TEST

START_TEST(be_c2)
{
    char expected_output[5] = {BASE64[0], BASE64[0], 0, BASE64[0]};
    char encoded[5];
    char input[3];

    input[0] = CHARS[0];

    for (size_t i = 0; i < 16; i++) {
        input[1] = CHARS[i];
        for (size_t j = 0; j < 4; j++) {
            input[2] = CHARS[j * 64];

            ck_assert_uint_eq(base64_encode(encoded, input, sizeof(input)), 4);

            expected_output[2] = BASE64[i * 4 + j];
            ck_assert_str_eq(encoded, expected_output);
        }
    }
}
END_TEST

START_TEST(be_c3)
{
    char expected_output[5] = {BASE64[0], BASE64[0], BASE64[0]};
    char encoded[5];
    char input[3];

    input[0] = CHARS[0];
    input[1] = CHARS[0];

    for (size_t i = 0; i < 64; i++) {
        input[2] = CHARS[i];

        ck_assert_uint_eq(base64_encode(encoded, input, sizeof(input)), 4);

        expected_output[3] = BASE64[i];
        ck_assert_str_eq(encoded, expected_output);
    }
}
END_TEST

START_TEST(be_every_char)
{
    const char EXPECTED_OUTPUT[] =
        "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4v"
        "MDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5f"
        "YGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6P"
        "kJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/"
        "wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v"
        "8PHy8/T19vf4+fr7/P3+/w==";
    char encoded[sizeof(EXPECTED_OUTPUT)];

    ck_assert_uint_eq(base64_encode(encoded, CHARS, sizeof(CHARS)),
                      sizeof(encoded) - 1);
    ck_assert_str_eq(encoded, EXPECTED_OUTPUT);
}
END_TEST

/*----------------------------------------------------------------------------*
 |                              base64_decode()                               |
 *----------------------------------------------------------------------------*/

START_TEST(bd_nothing)
{
    ck_assert_int_eq(base64_decode(NULL, NULL, 0), 0);
}
END_TEST

START_TEST(bd_leading_spaces)
{
    char input[] = {' ', '\f', '\n', '\r', '\t', '\v'};

    ck_assert_int_eq(base64_decode(NULL, input, sizeof(input)), 0);
}
END_TEST

START_TEST(bd_c0)
{
    char input[4] = {0, 0, '=', '='};
    char decoded;

    for (size_t i = 0; i < 64; i++) {
        input[0] = BASE64[i];
        for (size_t j = 0; j < 4; j++) {
            input[1] = BASE64[j << 4];

            for (size_t k = 2; k <= sizeof(input); k++) {
                ck_assert_int_eq(base64_decode(&decoded, input, k), 1);
                ck_assert_int_eq(decoded, CHARS[i * 4 + j]);
            }
        }
    }
}
END_TEST

START_TEST(bd_spaces_in_c0)
{
    char input[] = {'A', ' ', '\f', '\n', '\r', '\t', '\v', 'A'};
    char decoded = CHAR_MAX;

    ck_assert_int_eq(base64_decode(&decoded, input, sizeof(input)), 1);
    ck_assert_int_eq(decoded, 0);
}
END_TEST

START_TEST(bd_spaces_in_c1)
{
    char input[] = {'A', 'A', ' ', '\f', '\n', '\r', '\t', '\v', 'A'};
    char decoded[2] = {CHAR_MAX, CHAR_MAX};

    ck_assert_int_eq(base64_decode(decoded, input, sizeof(input)), 2);
    ck_assert_int_eq(decoded[0], 0);
    ck_assert_int_eq(decoded[1], 0);
}
END_TEST

START_TEST(bd_c1)
{
    char input[4] = {BASE64[0], 0, 0, '='};
    char decoded[2];

    for (size_t i = 0; i < 16; i++) {
        input[1] = BASE64[i];
        for (size_t j = 0; j < 16; j++) {
            input[2] = BASE64[j << 2];

            for (size_t k = 3; k <= sizeof(input); k++) {
                ck_assert_int_eq(base64_decode(decoded, input, k), 2);
                ck_assert_int_eq(decoded[0], 0);
                ck_assert_int_eq(decoded[1], CHARS[i * 16 + j]);
            }
        }
    }
}
END_TEST

START_TEST(bd_spaces_in_c2)
{
    char input[] = {'A', 'A', 'A', ' ', '\f', '\n', '\r', '\t', '\v', 'A'};
    char decoded[3] = {CHAR_MAX, CHAR_MAX, CHAR_MAX};

    ck_assert_int_eq(base64_decode(decoded, input, sizeof(input)), 3);
    ck_assert_int_eq(decoded[0], 0);
    ck_assert_int_eq(decoded[1], 0);
    ck_assert_int_eq(decoded[2], 0);
}
END_TEST

START_TEST(bd_c2)
{
    char input[4] = {BASE64[0], BASE64[0], 0, 0};
    char decoded[3];

    for (size_t i = 0; i < 4; i++) {
        input[2] = BASE64[i];
        for (size_t j = 0; j < 64; j++) {
            input[3] = BASE64[j];

            ck_assert_int_eq(base64_decode(decoded, input, 4), 3);
            ck_assert_int_eq(decoded[0], 0);
            ck_assert_int_eq(decoded[1], 0);
            ck_assert_int_eq(decoded[2], CHARS[i * 64 + j]);
        }
    }
}
END_TEST

START_TEST(bd_trailing_spaces)
{
    char input[] = {'A', 'A', '=', '=', ' ', '\f', '\n', '\r', '\t', '\v'};
    char decoded;

    ck_assert_int_eq(base64_decode(&decoded, input, sizeof(input)), 1);
    ck_assert_int_eq(decoded, 0);
}
END_TEST

START_TEST(bd_every_char)
{
    const char INPUT[] =
        "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4v"
        "MDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5f"
        "YGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6P"
        "kJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/"
        "wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v"
        "8PHy8/T19vf4+fr7/P3+/w==";
    char decoded[sizeof(CHARS)];

    ck_assert_int_eq(base64_decode(decoded, INPUT, sizeof(INPUT) - 1),
                     sizeof(decoded));
    ck_assert_mem_eq(decoded, CHARS, sizeof(CHARS));
}
END_TEST

START_TEST(bd_c0_encoding_error)
{
    char input = '.';

    errno = 0;
    ck_assert_int_eq(base64_decode(NULL, &input, 1), -1);
    ck_assert_int_eq(errno, EILSEQ);
}
END_TEST

START_TEST(bd_too_short)
{
    char input = 'A';
    char decoded;

    errno = 0;
    ck_assert_int_eq(base64_decode(&decoded, &input, 1), -1);
    ck_assert_int_eq(errno, EILSEQ);
}
END_TEST

START_TEST(bd_c0_encoding_error_bis)
{
    char input[2] = "A=";
    char decoded;

    errno = 0;
    ck_assert_int_eq(base64_decode(&decoded, input, 2), -1);
    ck_assert_int_eq(errno, EILSEQ);
}
END_TEST

START_TEST(bd_c1_encoding_error)
{
    char input[3] = "AA.";
    char decoded[2];

    errno = 0;
    ck_assert_int_eq(base64_decode(decoded, input, 3), -1);
    ck_assert_int_eq(errno, EILSEQ);
}
END_TEST

START_TEST(bd_c2_encoding_error)
{
    char input[4] = "AAA.";
    char decoded[3];

    errno = 0;
    ck_assert_int_eq(base64_decode(decoded, input, 4), -1);
    ck_assert_int_eq(errno, EILSEQ);
}
END_TEST

START_TEST(bd_trailing_encoding_error)
{
    char input[] = {'A', 'A', '=', '=', '.'};
    char decoded;

    errno = 0;
    ck_assert_int_eq(base64_decode(&decoded, input, sizeof(input)), -1);
    ck_assert_int_eq(errno, EILSEQ);
}
END_TEST

static Suite *
unit_suite(void)
{
    Suite *suite;
    TCase *tests;

    suite = suite_create("base64");

    tests = tcase_create("base64_encode");
    tcase_add_test(tests, be_nothing);
    tcase_add_test(tests, be_c0);
    tcase_add_test(tests, be_c1);
    tcase_add_test(tests, be_c2);
    tcase_add_test(tests, be_c3);
    tcase_add_test(tests, be_every_char);

    suite_add_tcase(suite, tests);

    tests = tcase_create("base64_decode");
    tcase_add_test(tests, bd_nothing);
    tcase_add_test(tests, bd_leading_spaces);
    tcase_add_test(tests, bd_c0);
    tcase_add_test(tests, bd_spaces_in_c0);
    tcase_add_test(tests, bd_spaces_in_c1);
    tcase_add_test(tests, bd_c1);
    tcase_add_test(tests, bd_spaces_in_c2);
    tcase_add_test(tests, bd_c2);
    tcase_add_test(tests, bd_trailing_spaces);
    tcase_add_test(tests, bd_every_char);
    tcase_add_test(tests, bd_c0_encoding_error);
    tcase_add_test(tests, bd_too_short);
    tcase_add_test(tests, bd_c0_encoding_error_bis);
    tcase_add_test(tests, bd_c1_encoding_error);
    tcase_add_test(tests, bd_c2_encoding_error);
    tcase_add_test(tests, bd_trailing_encoding_error);

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
