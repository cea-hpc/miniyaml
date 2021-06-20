/* This file is part of MiniYAML
 * Copyright (C) 2019 Commissariat a l'energie atomique et aux energies
 *                    alternatives
 *
 * SPDX-License-Identifer: LGPL-3.0-or-later
 */

#ifndef MINIYAML_H
#define MINIYAML_H

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>

#include <sys/types.h>

#include <yaml.h>

/*----------------------------------------------------------------------------*
 |                                   parser                                   |
 *----------------------------------------------------------------------------*/

/**
 * Skip an event or a series of event
 *
 * @param parser    the parser for which to skip events
 * @param type      the type of the last event yielded by the parser
 *
 * @return          false if there was a parsing error while skipping events,
 *                  true otherwise
 *
 * This function is useful to consume entire documents/mappings/sequences.
 *
 * Pass it the corresponding YAML_{DOCUMENT,MAPPING,SEQUENCE}_START_EVENT
 * type and it will parse every event until it finds the matching ending event
 * or encounters a parsing error.
 *
 * Note that mappings and/or sequences nested in the document/mapping/sequence
 * being skipped will be skipped as well.
 *
 * Pass any other event and this function is a noop.
 */
bool
yaml_parser_skip(yaml_parser_t *parser, yaml_event_type_t type);

/**
 * Skip the next event or series of event
 *
 * @param parser    the parse for which to skip events
 *
 * @return          false if there was a parsing error while skipping events,
 *                  true otherwise
 *
 * This function is useful when you encounter a key in a mapping you did not
 * expect and want to skip the value associated to it.
 */
static inline bool
yaml_parser_skip_next(yaml_parser_t *parser)
{
    yaml_event_t event;
    bool success;

    if (!yaml_parser_parse(parser, &event))
        return false;

    success = yaml_parser_skip(parser, event.type);
    yaml_event_delete(&event);
    return success;
}

/*----------------------------------------------------------------------------*
 |                                   event                                    |
 *----------------------------------------------------------------------------*/

    /*--------------------------------------------------------------------*
     |                               stream                               |
     *--------------------------------------------------------------------*/

/**
 * Emit a YAML_STREAM_START_EVENT
 *
 * @param emitter   emitter to use
 * @param encoding  the encoding of the stream
 *
 * @return          true on success, false otherwise
 */
static inline bool
yaml_emit_stream_start(yaml_emitter_t *emitter, yaml_encoding_t encoding)
{
    yaml_event_t event;

    return yaml_stream_start_event_initialize(&event, encoding)
        && yaml_emitter_emit(emitter, &event);
}

/**
 * Emit a YAML_STREAM_END_EVENT
 *
 * @param emitter   emitter to use
 *
 * @return          true on success, false otherwise
 */
static inline bool
yaml_emit_stream_end(yaml_emitter_t *emitter)
{
    yaml_event_t event;

    return yaml_stream_end_event_initialize(&event)
        && yaml_emitter_emit(emitter, &event);
}

    /*--------------------------------------------------------------------*
     |                              document                              |
     *--------------------------------------------------------------------*/

/**
 * Emit a YAML_DOCUMENT_START_EVENT
 *
 * @param emitter   emitter to use
 *
 * @return          true on success, false otherwise
 */
static inline bool
yaml_emit_document_start(yaml_emitter_t *emitter)
{
    yaml_event_t event;

    return yaml_document_start_event_initialize(&event, NULL, NULL, NULL, false)
        && yaml_emitter_emit(emitter, &event);
}

/**
 * Emit a YAML_DOCUMENT_END_EVENT
 *
 * @param emitter   emitter to use
 *
 * @return          true on success, false otherwise
 */
static inline bool
yaml_emit_document_end(yaml_emitter_t *emitter)
{
    yaml_event_t event;

    return yaml_document_end_event_initialize(&event, false)
        && yaml_emitter_emit(emitter, &event);
}

    /*--------------------------------------------------------------------*
     |                              mapping                               |
     *--------------------------------------------------------------------*/

/**
 * Return the tag of a mapping start event
 *
 * @param event     a mapping start event
 *
 * @return          the tag of \p event
 */
static inline const char *
yaml_mapping_tag(const yaml_event_t *event)
{
    assert(event->type == YAML_MAPPING_START_EVENT);
    return (char *)event->data.mapping_start.tag;
}

/**
 * Emit a YAML_MAPPING_START_EVENT
 *
 * @param emitter   emitter to use
 * @param tag       the tag to use (may be NULL)
 *
 * @return          true on success, false otherwise
 */
static inline bool
yaml_emit_mapping_start(yaml_emitter_t *emitter, const char *tag)
{
    yaml_event_t event;

    return yaml_mapping_start_event_initialize(&event, NULL, (yaml_char_t *)tag,
                                               false, YAML_ANY_MAPPING_STYLE)
        && yaml_emitter_emit(emitter, &event);
}

/**
 * Emit a YAML_MAPPING_END_EVENT
 *
 * @param emitter   emitter to use
 *
 * @return          true on success, false otherwise
 */
static inline bool
yaml_emit_mapping_end(yaml_emitter_t *emitter)
{
    yaml_event_t event;

    return yaml_mapping_end_event_initialize(&event)
        && yaml_emitter_emit(emitter, &event);
}

    /*--------------------------------------------------------------------*
     |                              sequence                              |
     *--------------------------------------------------------------------*/

/**
 * Emit a YAML_SEQUENCE_START_EVENT
 *
 * @param emitter   emitter to use
 * @param tag       the tag to use (may be NULL)
 *
 * @return          true on success, false otherwise
 */
static inline bool
yaml_emit_sequence_start(yaml_emitter_t *emitter, const char *tag)
{
    yaml_event_t event;

    return yaml_sequence_start_event_initialize(&event, NULL,
                                                (yaml_char_t *)tag, false,
                                                YAML_ANY_SEQUENCE_STYLE)
        && yaml_emitter_emit(emitter, &event);
}

/**
 * Emit a YAML_SEQUENCE_END_EVENT
 *
 * @param emitter   emitter to use
 *
 * @return          true on success, false otherwise
 */
static inline bool
yaml_emit_sequence_end(yaml_emitter_t *emitter)
{
    yaml_event_t event;

    return yaml_sequence_end_event_initialize(&event)
        && yaml_emitter_emit(emitter, &event);
}

    /*--------------------------------------------------------------------*
     |                               scalar                               |
     *--------------------------------------------------------------------*/

/**
 * Return the value of a scalar event
 *
 * @param event     a scalar event
 *
 * @return          the value of \p event
 */
static inline const char *
yaml_scalar_value(const yaml_event_t *event)
{
    assert(event->type == YAML_SCALAR_EVENT);
    return (char *)event->data.scalar.value;
}

/**
 * Return the length of a scalar event's value
 *
 * @param event     a scalar event
 *
 * @return          the length of \p event's value
 */
static inline size_t
yaml_scalar_length(const yaml_event_t *event)
{
    assert(event->type == YAML_SCALAR_EVENT);
    return event->data.scalar.length;
}

/**
 * Return the tag of a scalar event (may be NULL)
 *
 * @param event     a scalar event
 *
 * @return          the tag of \p event
 */
static inline const char *
yaml_scalar_tag(const yaml_event_t *event)
{
    assert(event->type == YAML_SCALAR_EVENT);
    return (char *)event->data.scalar.tag;
}

/**
 * Return the style of a scalar event
 *
 * @param event     a scalar event
 *
 * @return          the style of \p event
 */
static inline yaml_scalar_style_t
yaml_scalar_style(const yaml_event_t *event)
{
    assert(event->type == YAML_SCALAR_EVENT);
    return event->data.scalar.style;
}

/**
 * Return whether a scalar event uses the plain scalar style
 *
 * @param event     a scalar event
 *
 * @return          true if \p event uses the plain scalar style, false
 *                  otherwise
 */
static inline bool
yaml_scalar_is_plain(const yaml_event_t *event)
{
    assert(event->type == YAML_SCALAR_EVENT);
    return yaml_scalar_style(event) == YAML_PLAIN_SCALAR_STYLE;
}

/**
 * Emit a YAML_SCALAR_EVENT
 *
 * @param emitter   the emitter to use
 * @param tag       the tag to use
 * @param data      the data to use
 * @param size      the number of bytes to use from \p data
 * @param style     the style to use
 *
 * @return          true on success, false otherwise
 */
static inline bool
yaml_emit_scalar(yaml_emitter_t *emitter, const char *tag, const char *data,
                 size_t size, yaml_scalar_style_t style)
{
    yaml_event_t event;

    return yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)tag,
                                        (yaml_char_t *)data, size, tag == NULL,
                                        tag == NULL, style)
        && yaml_emitter_emit(emitter, &event);
}

        /*------------------------------------------------------------*
         |                            null                            |
         *------------------------------------------------------------*/

/**
 * Emit a null scalar
 *
 * @param emitter   the emitter to use
 *
 * @return          true on success, false otherwise
 */
static inline bool
yaml_emit_null(yaml_emitter_t *emitter)
{
    return yaml_emit_scalar(emitter, NULL, "~", 1, YAML_ANY_SCALAR_STYLE);
}

/**
 * Parse a scalar event as a null scalar
 *
 * @param event     a scalar event
 *
 * @return          true if \p event was successfully parsed as a null scalar,
 *                  false otherwise and errno is set appropriately
 *
 * @error EINVAL    \p event is not parsable as a null scalar
 */
bool
yaml_parse_null(const yaml_event_t *event);

        /*------------------------------------------------------------*
         |                          boolean                           |
         *------------------------------------------------------------*/

/**
 * Emit a boolean
 *
 * @param emitter   the emitter to use
 * @param b         the boolean to emit
 *
 * @return          true on success, false otherwise
 */
static inline bool
yaml_emit_boolean(yaml_emitter_t *emitter, bool b)
{
    return yaml_emit_scalar(emitter, NULL, b ? "y" : "n", 1,
                            YAML_PLAIN_SCALAR_STYLE);
}

/** Parse a scalar event as a boolean
 *
 * @param event     a scalar event
 * @param b         a pointer to a bool; on success, it is set to the value
 *                  \p event represents
 *
 * @return          true if \p event was successfully parsed as a boolean, false
 *                  otherwise and errno is set appropriately
 *
 * @error EINVAL    \p event is not parsable as a boolean
 */
bool
yaml_parse_boolean(const yaml_event_t *event, bool *b);

        /*------------------------------------------------------------*
         |                      (signed) integer                      |
         *------------------------------------------------------------*/

/**
 * Emit a signed integer
 *
 * @param emitter   the emitter to use
 * @param i         the signed integer to emit
 *
 * @return          true on success, false otherwise
 */
static inline bool
yaml_emit_integer(yaml_emitter_t *emitter, intmax_t i)
{
    char buffer[sizeof(intmax_t) * 4];
    int n;

    n = snprintf(buffer, sizeof(buffer), "%"PRIiMAX, i);
    return yaml_emit_scalar(emitter, NULL, buffer, n, YAML_PLAIN_SCALAR_STYLE);
}

/**
 * Parse a scalar event as a signed integer
 *
 * @param event     a scalar event
 * @param i         a pointer to an intmax_t; on success, it is set to the value
 *                  \p event represents
 *
 * @return          true if \p event was successfully parsed as a signed
 *                  integer,  false otherwise and errno is set appropriately
 *
 * @error EINVAL    \p event is not parsable as a signed integer
 * @error ERANGE    the value \p event represents does not fit in an intmaxt_t
 */
bool
yaml_parse_integer(const yaml_event_t *event, intmax_t *i);

        /*------------------------------------------------------------*
         |                      unsigned integer                      |
         *------------------------------------------------------------*/

/**
 * Emit an unsigned integer
 *
 * @param emitter   the emitter to use
 * @param u         the unsigned integer to emit
 *
 * @return          true on success, false otherwise
 */
static inline bool
yaml_emit_unsigned_integer(yaml_emitter_t *emitter, uintmax_t u)
{
    char buffer[sizeof(uintmax_t) * 4];
    int n;

    n = snprintf(buffer, sizeof(buffer), "%"PRIuMAX, u);
    return yaml_emit_scalar(emitter, NULL, buffer, n, YAML_PLAIN_SCALAR_STYLE);
}

/**
 * Parse a scalar event as an unsigned integer
 *
 * @param event     a scalar event
 * @param u         a pointer to an uintmax_t; on success, it is set to the
 *                  value \p event represents
 *
 * @return          true if \p event was successfully parsed as an unsigned
 *                  integer, false otherwise and errno is set appropriately
 *
 * @error EINVAL    \p event is not parsable as an unsigned integer
 * @error ERANGE    the value \p event represents does not fit in an uintmaxt_t
 *
 * Note that negative integers will implicitely be converted to their signed
 * equivalent, just like if you cast a negative intmax_t into an uintmax_t
 * (eg. -UINTMAX_MAX --> 1).
 * Integers strictly smaller than -UINTMAX_MAX will fail to be parsed and errno
 * will be set to ERANGE.
 */
bool
yaml_parse_unsigned_integer(const yaml_event_t *event, uintmax_t *u);

        /*------------------------------------------------------------*
         |                           string                           |
         *------------------------------------------------------------*/

/**
 * Emit a string
 *
 * @param emitter   the emitter to use
 * @param string    the string to emit
 * @param length    the number of bytes in \p string to emit (not including the
 *                  null terminating byte)
 *
 * @return          true on success, false otherwise
 *
 * To avoid having to specify \p length as \c strlen(string) yourself, you may
 * use the YAML_EMIT_STRING() macro.
 */
static inline bool
yaml_emit_string(yaml_emitter_t *emitter, const char *string, size_t length)
{
    return yaml_emit_scalar(emitter, NULL, string, length,
                            YAML_DOUBLE_QUOTED_SCALAR_STYLE);
}

#define YAML_EMIT_STRING(emitter, string) \
    yaml_emit_string(emitter, string, strlen(string))

/**
 * Parse a scalar event as a string
 *
 * @param event     a scalar event
 * @param string    a pointer to a const char *; on success, it is set to the
 *                  value \p event represents
 *
 * @return          true if \p event was successfully parsed as a string, false
 *                  otherwise and errno is set appropriately
 *
 * @error EINVAL    \p event is not parsable as a string
 */
bool
yaml_parse_string(const yaml_event_t *event, const char **string,
                  size_t *length);

        /*------------------------------------------------------------*
         |                           binary                           |
         *------------------------------------------------------------*/

/**
 * Emit binary data
 *
 * @param emitter   the emitter to use
 * @param data      the binary data to emit
 * @param size      the number of bytes in \p data to emit
 *
 * @return          true on success, false otherwise
 */
bool
yaml_emit_binary(yaml_emitter_t *emitter, const char *data, size_t size);

/**
 * Parse a scalar event as binary data
 *
 * @param event     a scalar event
 * @param data      a pointer to a buffer big enough to store the binary data
 *                  \p event represents; on success, it is filled with this data
 *
 * @return          true if \p event was successfully parsed as binary data,
 *                  false otherwise and errno is set appropriately
 *
 * @error EINVAL    \p event is not parsable as binary data
 * @error EILSEQ    the binary data in \p event is incorrectly encoded (which
 *                  may indicate that \p event is not, in fact, parsable as
 *                  binary data)
 *
 * Binary data in yaml is encoded into Base64. The number of bytes \p event may
 * represent is capped by: L * 4 / 3, where L is the length of \p event itself
 * (as returned by yaml_scalar_length()).
 */
bool
yaml_parse_binary(const yaml_event_t *event, char *data, size_t *size);

#endif
