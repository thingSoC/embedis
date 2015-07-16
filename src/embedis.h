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
  * @author      Dave Turnbull
  * @version     0.0.1
  * @date        2015-06-29
  * @copyright   PatternAgents, LLC
  * @brief       The main include file for the Embedis Dictionary Server
  *
  ******************************************************************************
  */

/// @file

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
/* Exported Constants                                                          */
/*-----------------------------------------------------------------------------*/
/** @defgroup embedis_Exported_Constants
  * @{
  */

#ifndef EMBEDIS_COMMAND_BUF_SIZE
#define EMBEDIS_COMMAND_BUF_SIZE (128)
#endif

#ifndef EMBEDIS_COMMAND_MAX_ARGS
#define EMBEDIS_COMMAND_MAX_ARGS (8)
#endif


/**
  * Close the Doxygen embedis_Exported_Constants group.
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
    void (*select)(embedis_state* state);
    void (*keys)(embedis_state* state);
    void (*get)(embedis_state* state);
    void (*set)(embedis_state* state);
    void (*del)(embedis_state* state);
} embedis_dictionary_commands;

typedef struct embedis_dictionary {
    const char* name;
    const embedis_dictionary_commands* commands;
    void* context;
} embedis_dictionary;

typedef struct embedis_dictionary_key {
    const char* name;
    short int id;
} embedis_dictionary_key;

typedef struct embedis_rw_key  {
    const char* name;
    void (*read)(embedis_state* state);
    void (*write)(embedis_state* state);
} embedis_rw_key;

typedef struct embedis_command {
    const char* name;
    void (*call)(embedis_state* state);
} embedis_command;

typedef struct embedis_ram_access {
    size_t (*size)();
    char (*fetch)(size_t pos);
    void (*store)(size_t pos, char value);
} embedis_ram_access;

struct embedis_state {
    const char* argv[EMBEDIS_COMMAND_MAX_ARGS+1];
    const embedis_dictionary* dictionary;
    size_t argc;
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

void embedis_init();
void embedis_reset();
void embedis_in(char data);

int embedis_strcmp(const char* s1, const char* s2);
int embedis_stricmp(const char* s1, const char* s2);

void embedis_emit_newline();
void embedis_emit_integer(int i);
void embedis_emit_size(char kind, size_t length);
void embedis_response_error(const char* message);
void embedis_response_simple(const char* message);
void embedis_response_string(const char* message, size_t length);
void embedis_response_null();

// commands.c

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

// Application must provide implementations of these

void embedis_out(char data);

extern const embedis_dictionary embedis_dictionaries[];
extern const embedis_command embedis_commands[];
extern const embedis_dictionary_key embedis_dictionary_keys[];
extern const embedis_rw_key embedis_rw_keys[];

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
