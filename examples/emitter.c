#include <error.h>

#include "miniyaml.h"

#define PERSON_TAG "!person"

struct person {
    char name[128];
    unsigned int age;
};

static void __attribute__((noreturn))
emitter_error(yaml_emitter_t *emitter)
{
    error(EXIT_FAILURE, 0, "emitter error: %s\n", emitter->problem);
    __builtin_unreachable();
}

static bool
emit_person(yaml_emitter_t *emitter, const struct person *person)
{
    return yaml_emit_document_start(emitter)
        && yaml_emit_mapping_start(emitter, PERSON_TAG)
        && YAML_EMIT_STRING(emitter, "name")
        && YAML_EMIT_STRING(emitter, person->name)
        && YAML_EMIT_STRING(emitter, "age")
        && yaml_emit_unsigned_integer(emitter, person->age)
        && yaml_emit_mapping_end(emitter)
        && yaml_emit_document_end(emitter);
}

int
main()
{
    const struct person PERSON = {
        .name = "test",
        .age = 32,
    };
    yaml_emitter_t emitter;

    if (!yaml_emitter_initialize(&emitter))
        error(EXIT_FAILURE, 0, "yaml_emitter_initialize");

    yaml_emitter_set_output_file(&emitter, stdout);
    yaml_emitter_set_unicode(&emitter, true);

    if (!yaml_emit_stream_start(&emitter, YAML_UTF8_ENCODING))
        emitter_error(&emitter);

    if (!emit_person(&emitter, &PERSON))
        emitter_error(&emitter);

    /* XXX: Emit more documents here if you want to */

    if (!yaml_emit_stream_end(&emitter))
        emitter_error(&emitter);

    yaml_emitter_delete(&emitter);
    return EXIT_SUCCESS;
}
