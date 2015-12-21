/*  Embedis - Embedded Dictionary Server
    Copyright (C) 2015 PatternAgents, LLC

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
  ******************************************************************************
  *
  * @file        embedis.h
  * @copyright   PatternAgents, LLC
  * @brief       The main include file for the Embedis Dictionary Server
  *
  ******************************************************************************
  */

#include <stddef.h>

#ifndef EMBEDIS_H
#define EMBEDIS_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup embedis embedis
  * @{
  */

/*-----------------------------------------------------------------------------*/
/* Exported Types                                                              */
/*-----------------------------------------------------------------------------*/
/** @defgroup embedis_Exported_Types
  * @{
  */

/**
  * Close the Doxygen embedis__Exported_Types group.
  * @}
  */


/*-----------------------------------------------------------------------------*/
/* Exported Macros                                                             */
/*-----------------------------------------------------------------------------*/
/** @defgroup embedis_Exported_Macros
  * @{
  */

/**
  * Close the Doxygen embedis_Exported_Macros group.
  * @}
  */

/*-----------------------------------------------------------------------------*/
/* Exported Variable Declarations                                              */
/*-----------------------------------------------------------------------------*/
/** @defgroup embedis_Exported_Variables
  * @{
  */

extern const char* EMBEDIS_OK;               /**< Embedis OK Flag */
extern const char* EMBEDIS_UNKNOWN_COMMAND;  /**< Embedis Unknown Command Flag */
extern const char* EMBEDIS_SYNTAX_ERROR;     /**< Embedis Syntax Error Flag */
extern const char* EMBEDIS_BUFFER_OVERFLOW;  /**< Embedis Buffer Overflow Flag */
extern const char* EMBEDIS_ARGS_ERROR;       /**< Embedis Argument Error Flag */
extern const char* EMBEDIS_STORAGE_OVERFLOW; /**< Embedis Storage Overflow Flag */

typedef struct embedis_state embedis_state;  /**< Embedis CLI State */

typedef struct embedis_dictionary_commands {
    void (*const select)(embedis_state* state);
    void (*const keys)(embedis_state* state);
    void (*const get)(embedis_state* state);
    void (*const set)(embedis_state* state);
    void (*const del)(embedis_state* state);
} embedis_dictionary_commands;

typedef struct embedis_dictionary {
    const char* const name;
    const embedis_dictionary_commands* commands;
    void* const context;
} embedis_dictionary;

typedef struct embedis_dictionary_key {
    const char* const name;
    const short int id;
} embedis_dictionary_key;

typedef struct embedis_rw_key  {
    const char* const name;
    void (*const read)(embedis_state* state);
    void (*const write)(embedis_state* state);
} embedis_rw_key;

typedef struct embedis_command {
    const char* const name;
    void (*const call)(embedis_state* state);
} embedis_command;

typedef struct embedis_ram_access {
    size_t (*const size)();
    char (*const fetch)(size_t pos);
    void (*const store)(size_t pos, char value);
    void (*const commit)();
} embedis_ram_access;

typedef struct embedis_protocol {
    char* const buf;
    const size_t buf_length;
    const size_t argv_length;
    size_t pos;
    char mode;
} embedis_protocol;

struct embedis_state {
    void (*const output)(char data);
    char** const argv;
    size_t argc;
    const embedis_dictionary* dictionary;
    embedis_state* prev;
    embedis_protocol protocol;
    unsigned char num;
};

/** Platform Globals */

/**
  * Close the Doxygen embedis_Exported_Variables group.
  * @}
  */

/*-----------------------------------------------------------------------------*/
/* Exported Function Declarations                                              */
/*-----------------------------------------------------------------------------*/
/** @defgroup embedis_Exported_Functions
  * @{
  */
// protocol.c

embedis_state* embedis_state_last(embedis_state*);
void embedis_reset(embedis_state *state);
void embedis_in(embedis_state *state, char data);

int embedis_strcmp(const char* s1, const char* s2);
int embedis_stricmp(const char* s1, const char* s2);

void embedis_emit_newline(embedis_state* state);
void embedis_emit_integer(embedis_state* state, int i);
void embedis_emit_size(embedis_state* state, char kind, size_t length);
void embedis_response_error(embedis_state* state, const char* message);
void embedis_response_simple(embedis_state* state, const char* message);
void embedis_response_string(embedis_state* state, const char* message, size_t length);
void embedis_response_null(embedis_state* state);

// commands.c

void embedis_COMMANDS(embedis_state* state);

void embedis_SELECT(embedis_state* state);
void embedis_KEYS(embedis_state* state);
void embedis_GET(embedis_state* state);
void embedis_SET(embedis_state* state);
void embedis_DEL(embedis_state* state);

extern const embedis_dictionary_commands embedis_rom_commands;
void embedis_rom_SELECT(embedis_state* state);
void embedis_rom_KEYS(embedis_state* state);
void embedis_rom_GET(embedis_state* state);
void embedis_rom_SET(embedis_state* state);
void embedis_rom_DEL(embedis_state* state);

extern const embedis_dictionary_commands embedis_ram_commands;
void embedis_ram_SELECT(embedis_state* state);
void embedis_ram_KEYS(embedis_state* state);
void embedis_ram_GET(embedis_state* state);
void embedis_ram_SET(embedis_state* state);
void embedis_ram_DEL(embedis_state* state);

void embedis_READ(embedis_state* state);
void embedis_WRITE(embedis_state* state);

void embedis_PUBLISH(embedis_state* state);
void embedis_SUBSCRIBE(embedis_state* state);
void embedis_UNSUBSCRIBE(embedis_state* state);

int embedis_publish(const char* channel, const char* message, size_t length);

// Application must provide implementations of these

extern const embedis_dictionary embedis_dictionaries[];
extern const embedis_command embedis_commands[];
extern const embedis_dictionary_key embedis_dictionary_keys[];
extern const embedis_rw_key embedis_rw_keys[];
extern char const * const embedis_pubsub_channels[];
extern unsigned char embedis_pubsub_subscriptions[];


// Creates an instance of embedis_state named embedis_state_{num}.
// You can have 256 of these but only num 0-7 support the SUBSCRIBE command.
#define EMBEDIS_STATE_INSTANCE(num, output, buflen, argslen) \
char embedis_state_##num##_buf[buflen]; \
char* embedis_state_##num##_argv[argslen+1] = {embedis_state_##num##_buf}; \
embedis_state embedis_state_##num = { \
    output, \
    embedis_state_##num##_argv, \
    0, \
    &embedis_dictionaries[0], \
    embedis_state_last(&embedis_state_##num), \
    { \
        embedis_state_##num##_buf, \
        buflen, \
        argslen, \
        0, \
        0 \
    }, \
    num \
};


/**
  * Close the Doxygen embedis_Exported_Functions group.
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*EMBEDIS_H*/
/*-----------------------------------------------------------------------------*/
/**
  * Close the Doxygen embedis group.
  *    @}
*/
