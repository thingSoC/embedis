/*  Embedis - Embedded Dictionary Server
    Copyright (C) 2015 Pattern Agents, LLC

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

/// @file

#include <stddef.h>

#ifndef EMBEDIS_H
#define EMBEDIS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EMBEDIS_COMMAND_BUF_SIZE
#define EMBEDIS_COMMAND_BUF_SIZE (128)
#endif

#ifndef EMBEDIS_COMMAND_MAX_ARGS
#define EMBEDIS_COMMAND_MAX_ARGS (8)
#endif

extern const char* EMBEDIS_OK;
extern const char* EMBEDIS_UNKNOWN_COMMAND;
extern const char* EMBEDIS_SYNTAX_ERROR;
extern const char* EMBEDIS_BUFFER_OVERFLOW;
extern const char* EMBEDIS_ARGS_ERROR;
extern const char* EMBEDIS_STORAGE_OVERFLOW;

void embedis_response_newline();
void embedis_response_error(const char* message);
void embedis_response_simple(const char* message);
void embedis_response_string_length(size_t length);
void embedis_response_string(const char* message, size_t length);
void embedis_response_null();

void embedis_reset();
void embedis_in(char data);
void embedis_out(char data);

size_t embedis_nvram_size();
size_t embedis_nvram_fetch(size_t pos);
void embedis_nvram_store(size_t pos, char value);

void embedis_GET(int argc, const char* argv[]);
void embedis_SET(int argc, const char* argv[]);
void embedis_DEL(int argc, const char* argv[]);
void embedis_dict_error(int argc, const char* argv[], const void* id);
void embedis_dict_rom(int argc, const char* argv[], const void* id);
void embedis_dict_GET(int argc, const char* argv[], const void* id);
void embedis_dict_SET(int argc, const char* argv[], const void* id);
void embedis_dict_DEL(int argc, const char* argv[], const void* id);


typedef struct embedis_command {
    const char* name;
    void (*call)(int argc, const char* argv[]);
} embedis_command;

typedef struct embedis_dict_handler {
    void (*get)(int argc, const char* argv[], const void* id);
    void (*set)(int argc, const char* argv[], const void* id);
    void (*del)(int argc, const char* argv[], const void* id);
} embedis_dict_handler;

typedef struct embedis_dict_config {
    const char* name;
    const void* id;
    const embedis_dict_handler* handle;
} embedis_dict_config;

extern const embedis_command embedis_commands[];
extern const embedis_dict_config embedis_dict_keys[];

#ifdef __cplusplus
}
#endif

#endif /*EMBEDIS_H*/
