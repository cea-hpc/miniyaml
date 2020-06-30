/* this file is part of MiniYAML
 * copyright (c) 2019 commissariat a l'energie atomique et aux energies
 *                    alternatives
 *
 * spdx-license-identifer: lgpl-3.0-or-later
 */

#include <errno.h>
#include <error.h>
#include <limits.h>

#include "miniyaml.h"

#define PERSON_TAG "!person"

struct person {
    char name[128];
    unsigned int age;
};

struct fields_seen {
    bool name:1;
    bool age:1;
};

static void __attribute__((noreturn))
parser_error(yaml_parser_t *parser)
{
    error(EXIT_FAILURE, 0, "parser error: %s", parser->problem);
    __builtin_unreachable();
}

enum person_field {
    PF_UNKNOWN,
    PF_NAME,
    PF_AGE,
};

static enum person_field __attribute__((pure))
person_field_tokenize(const char *key)
{
    switch (*key++) {
    case 'a': /* age */
        if (strcmp(key, "ge"))
            break;
        return PF_AGE;
    case 'n': /* name */
        if (strcmp(key, "ame"))
            break;
        return PF_NAME;
    }

    return PF_UNKNOWN;
}

static void
parse_person_field(yaml_parser_t *parser, struct person *person,
                   const char *key, struct fields_seen *seen)
{
    yaml_event_t event;
    const char *name;
    intmax_t age;
    bool success;

    if (!yaml_parser_parse(parser, &event))
        parser_error(parser);

    switch (person_field_tokenize(key)) {
    case PF_UNKNOWN:
        error(0, 0, "unknown mapping key '%s'", key);
        goto skip;
    case PF_NAME:
        success = yaml_parse_string(&event, &name, NULL);
        if (success) {
            int rc;

            rc = snprintf(person->name, sizeof(person->name), "%s", name);
            if (rc >= 0 && rc < sizeof(person->name)) {
                seen->name = true;
            } else {
                errno = EOVERFLOW;
                success = false;
            }
        }
        break;
    case PF_AGE:
        success = yaml_parse_integer(&event, &age);
        if (success) {
            if (age >= 0 && age <= UINT_MAX) {
                person->age = age;
                seen->age = true;
            } else {
                success = false;
                errno = ERANGE;
            }
        }

    }

    if (!success)
        error(0, errno, "%s, l.%zu:%zu:", key, event.start_mark.line,
              event.start_mark.column);

skip:
    /* It is only safe to skip on success because no key should successfully
     * map to a mapping or a sequence.
     */
    if (!yaml_parser_skip(parser, event.type))
        parser_error(parser);
    yaml_event_delete(&event);
}

static bool
parse_person_mapping(yaml_parser_t *parser, struct person *person)
{
    struct fields_seen seen = {};
    bool end = false;

    do {
        yaml_event_t event;
        const char *key;

        if (!yaml_parser_parse(parser, &event))
            parser_error(parser);

        switch (event.type) {
        case YAML_MAPPING_END_EVENT:
            end = true;
            break;
        case YAML_SCALAR_EVENT:
            /* Only keys that are strings are allowed/expected here */
            if (yaml_parse_string(&event, &key, NULL)) {
                parse_person_field(parser, person, key, &seen);
                break;
            }

            /* Otherwise, skip it */
            __attribute__((fallthrough));
        default:
            /* Ignore this key/value */
            if (!yaml_parser_skip(parser, event.type))
                parser_error(parser);
            if (!yaml_parser_skip_next(parser))
                parser_error(parser);
        }

        yaml_event_delete(&event);
    } while (!end);

    return seen.name && seen.age;
}

static bool
parse_person_document(yaml_parser_t *parser, struct person *person)
{
    yaml_event_t event;
    const char *tag;
    bool success;

    if (!yaml_parser_parse(parser, &event))
        parser_error(parser);

    if (event.type != YAML_MAPPING_START_EVENT)
        goto skip;

    tag = yaml_mapping_tag(&event);
    if (tag && strcmp(tag, PERSON_TAG))
        goto skip;

    yaml_event_delete(&event);

    success = parse_person_mapping(parser, person);
    if (tag && !success)
        /* We know for sure this should have been a person mapping */
        error(0, 0, "incomplete person mapping");

    if (!yaml_parser_parse(parser, &event))
        parser_error(parser);
    assert(event.type == YAML_DOCUMENT_END_EVENT);

    return success;

skip:
    yaml_event_delete(&event);
    if (!yaml_parser_skip(parser, YAML_DOCUMENT_START_EVENT))
        parser_error(parser);
    return false;
}

int
main()
{
    struct person person;
    yaml_parser_t parser;
    yaml_event_t event;
    bool end = false;

    if (!yaml_parser_initialize(&parser))
        error(EXIT_FAILURE, 0, "yaml_parser_initialize");

    yaml_parser_set_input_file(&parser, stdin);
    yaml_parser_set_encoding(&parser, YAML_UTF8_ENCODING);

    if (!yaml_parser_parse(&parser, &event))
        parser_error(&parser);

    assert(event.type == YAML_STREAM_START_EVENT);
    yaml_event_delete(&event);

    do {
        if (!yaml_parser_parse(&parser, &event))
            parser_error(&parser);

        switch (event.type) {
        case YAML_DOCUMENT_START_EVENT:
            if (parse_person_document(&parser, &person))
                printf("person = {.name = %s, .age = %d}\n", person.name,
                       person.age);
            break;
        case YAML_STREAM_END_EVENT:
            end = true;
            break;
        default:
            assert(false);
            __builtin_unreachable();
        }

        yaml_event_delete(&event);
    } while (!end);

    yaml_parser_delete(&parser);
    return EXIT_SUCCESS;
}
