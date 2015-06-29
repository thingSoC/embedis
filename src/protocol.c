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

#include "embedis.h"

const char* EMBEDIS_OK = "OK";
const char* EMBEDIS_UNKNOWN_COMMAND = "unknown command";
const char* EMBEDIS_SYNTAX_ERROR = "syntax error";
const char* EMBEDIS_BUFFER_OVERFLOW = "buffer overflow";
const char* EMBEDIS_ARGS_ERROR = "bad argument count";
const char* EMBEDIS_STORAGE_OVERFLOW = "storage overflow";

// Foo
void embedis_response_newline() {
    embedis_out('\r');
    embedis_out('\n');
}

void embedis_response_error(const char* message) {
    const char* errstr = "-ERROR";
    if (message == EMBEDIS_OK) {
        embedis_response_simple(message);
        return;
    }
    while(*errstr) {
        embedis_out(*errstr);
        errstr++;
    }
    if (message) {
        embedis_out(' ');
        while(*message) {
            embedis_out(*message);
            message++;
        }
    }
    embedis_response_newline();
}

void embedis_response_simple(const char* message) {
    embedis_out('+');
    while(*message) {
        embedis_out(*message);
        message++;
    }
    embedis_response_newline();
}

void embedis_response_string_length(size_t length) {
    size_t i, j, k;
    for (i = 1; length / (i*10); i *= 10) {}
    j = length;
    embedis_out('$');
    while (i) {
        k = j / i;
        embedis_out('0' + k);
        j -= k * i;
        i /= 10;
    }
    embedis_response_newline();
}

void embedis_response_string(const char* message, size_t length) {
    size_t i;
    embedis_response_string_length(length);
    for (i = 0; i < length; i++) {
        embedis_out(message[i]);
    }
    embedis_response_newline();
}

void embedis_response_null() {
    const char* nullstr = "$-1";
    while(*nullstr) {
        embedis_out(*nullstr);
        nullstr++;
    }
    embedis_response_newline();
}


static struct {
    const char* argv[EMBEDIS_COMMAND_MAX_ARGS+1];
    unsigned char argc;
    char state;
    size_t pos;
    char buf[EMBEDIS_COMMAND_BUF_SIZE];
} command;


void embedis_reset() {
    command.state = 0;
    command.argc = 0;
    command.argv[0] = command.buf;
    command.pos = 0;
}


static void embedis_dispatch() {
    const embedis_command* cmd = &embedis_commands[0];
    char* upcase = command.buf;
    size_t i;

    while (*upcase) {
        if (*upcase >= 'a' && *upcase <= 'z') {
            *upcase -= 32;
        }
        upcase++;
    }

    while (cmd->name) {
        i = 0;
        while (cmd->name[i] && cmd->name[i] == command.buf[i]) i++;
        if (!cmd->name[i] && !command.buf[i]) break;
        cmd++;
    }
    (*cmd->call)(command.argc, command.argv);
}


void embedis_in(char data) {

    if (command.state == 0) {
        switch (data) {
            case '+':
            case '-':
            case ':':
            case '$':
            case '*':
//                state = data; // TODO
                return;
            case '\r':
            case '\n':
                // nop
                return;
            default:
                if (command.pos != 0) {
                    embedis_reset();
                    embedis_response_error(EMBEDIS_SYNTAX_ERROR);
                    return;
                }
                command.state = ' ';
                break;
        }
    }


//    if (command.state == 0 && command.pos == 0) command.state = ' ';


    if (command.state == ' ') {
        if (data == '\r' || data == '\n') {
            if (!command.pos) {
                embedis_reset();
                return;
            }
            // Deal with trailing spaces
            if (command.argv[command.argc] != &command.buf[command.pos]) {
                command.argc++;
            } else {
                command.pos--;
            }
            // Check for overflows
            if (command.argc > EMBEDIS_COMMAND_MAX_ARGS) {
                embedis_response_error(EMBEDIS_ARGS_ERROR);
                embedis_reset();
                return;
            }
            if (command.pos >= EMBEDIS_COMMAND_BUF_SIZE) {
                embedis_response_error(EMBEDIS_BUFFER_OVERFLOW);
                embedis_reset();
                return;
            }
            command.buf[command.pos] = 0;
            command.pos++;
            command.argv[command.argc] = &command.buf[command.pos];
            embedis_dispatch();
            embedis_reset();
            return;
        }

        if (data == ' ') {
            if (command.argv[command.argc] == &command.buf[command.pos]) {
                // drop extra spaces
                return;
            }
            if (command.pos < EMBEDIS_COMMAND_BUF_SIZE) {
                command.buf[command.pos] = 0;
                command.pos++;
            }
            command.argc++;
            if (command.argc <= EMBEDIS_COMMAND_MAX_ARGS) {
                command.argv[command.argc] = &command.buf[command.pos];
            }
            return;
        }
        
        if (command.pos < EMBEDIS_COMMAND_BUF_SIZE) {
            command.buf[command.pos] = data;
            command.pos++;
        }
        
        return;
    }

}

