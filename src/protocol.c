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
  * @file        protocol.c
  * @copyright   PatternAgents, LLC
  * @brief       The Embedis Dictionary
  *
  ******************************************************************************
  */

#include "embedis.h"

const char* EMBEDIS_OK = "OK";
const char* EMBEDIS_UNKNOWN_COMMAND = "unknown command";
const char* EMBEDIS_SYNTAX_ERROR = "syntax error";
const char* EMBEDIS_BUFFER_OVERFLOW = "buffer overflow";
const char* EMBEDIS_ARGS_ERROR = "bad argument count";
const char* EMBEDIS_STORAGE_OVERFLOW = "storage overflow";


void embedis_emit_newline(embedis_state* state) {
    (*state->output)('\r');
    (*state->output)('\n');
}


void embedis_emit_integer(embedis_state* state, int i) {
    size_t j, k;
    if (i < 0) {
        i = -i;
        (*state->output)('-');
    }
    for (j = 1; i / (j*10); j *= 10) {}
    while (j) {
        k = i / j;
        (*state->output)('0' + k);
        i -= k * j;
        j /= 10;
    }
}


void embedis_emit_size(embedis_state* state, char kind, size_t length) {
    (*state->output)(kind);
    embedis_emit_integer(state, length);
    embedis_emit_newline(state);
}


void embedis_response_error(embedis_state* state, const char* message) {
    const char* errstr = "-ERROR";
    if (message == EMBEDIS_OK) {
        embedis_response_simple(state, message);
        return;
    }
    while(*errstr) {
        (*state->output)(*errstr);
        errstr++;
    }
    if (message) {
        (*state->output)(' ');
        while(*message) {
            (*state->output)(*message);
            message++;
        }
    }
    embedis_emit_newline(state);
}


void embedis_response_simple(embedis_state* state, const char* message) {
    (*state->output)('+');
    while(*message) {
        (*state->output)(*message);
        message++;
    }
    embedis_emit_newline(state);
}


void embedis_response_string(embedis_state* state, const char* message, size_t length) {
    size_t i = 0;
    embedis_emit_size(state, '$', length);
    for (i = 0; i < length; i++) {
        (*state->output)(message[i]);
    }
    embedis_emit_newline(state);
}


void embedis_response_null(embedis_state* state) {
    const char* nullstr = "$-1";
    while(*nullstr) {
        (*state->output)(*nullstr);
        nullstr++;
    }
    embedis_emit_newline(state);
}


// Call embedis_state_last(0) to get last embedis_state in linked list.
embedis_state* embedis_state_last(embedis_state* current) {
    static embedis_state* last = 0;
    if (!current) return last;
    embedis_state* x = last;
    last = current;
    return x;
}

// Reset only the protocol state. For eaxmple,
// call this when a UART notifies you of a break condition.
void embedis_reset(embedis_state* state) {
    state->protocol.mode = 0;
    state->protocol.pos = 0;
    state->argc = 0;
    state->argv[0] = state->protocol.buf;
}


int embedis_strcmp(const char* s1, const char* s2) {
    int rc;
    while (1) {
        if (!*s1 && !*s2) return 0;
        rc = *s1 - *s2;
        if (rc) return rc;
        s1++;
        s2++;
    }
}


int embedis_stricmp(const char* s1, const char* s2) {
    int rc, us1, us2;
    while (1) {
        if (!*s1 && !*s2) return 0;
        us1 = *s1;
        us2 = *s2;
        if (us1 >= 'a' && us1 <= 'z') us1 -= 32;
        if (us2 >= 'a' && us2 <= 'z') us2 -= 32;
        rc = us1 - us2;
        if (rc) return rc;
        s1++;
        s2++;
    }

}


// Main command dispatcher.
static void embedis_dispatch(embedis_state* state) {
    const embedis_command* cmd = &embedis_commands[0];
    while (cmd->name) {
        if (!embedis_stricmp(cmd->name, state->argv[0])) break;
        cmd++;
    }
    (*cmd->call)(state);
}


// Process characters received over connection.
void embedis_in(embedis_state* state, char data) {

    if (state->protocol.mode == 0) {
        switch (data) {
        case '+':
        case '-':
        case ':':
        case '$':
        case '*':
            //                mode = data; // TODO
            return;
        case '\r':
        case '\n':
            // nop
            return;
        default:
            if (state->protocol.pos != 0) {
                embedis_reset(state);
                embedis_response_error(state, EMBEDIS_SYNTAX_ERROR);
                return;
            }
            state->protocol.mode = ' ';
            break;
        }
    }


    //    if (state->protocol.mode == 0 && state->protocol.pos == 0) state->protocol.mode = ' ';


    if (state->protocol.mode == ' ') {
        if (data == '\r' || data == '\n') {
            if (!state->protocol.pos) {
                embedis_reset(state);
                return;
            }
            // Deal with trailing spaces
            if (state->argv[state->argc] != &state->protocol.buf[state->protocol.pos]) {
                state->argc++;
            } else {
                state->protocol.pos--;
            }
            // Check for overflows
            if (state->argc > state->protocol.argv_length) {
                embedis_response_error(state, EMBEDIS_ARGS_ERROR);
                embedis_reset(state);
                return;
            }
            if (state->protocol.pos >= state->protocol.buf_length) {
                embedis_response_error(state, EMBEDIS_BUFFER_OVERFLOW);
                embedis_reset(state);
                return;
            }
            state->protocol.buf[state->protocol.pos] = 0;
            state->protocol.pos++;
            state->argv[state->argc] = &state->protocol.buf[state->protocol.pos];
            embedis_dispatch(state);
            embedis_reset(state);
            return;
        }

        if (data == ' ') {
            if (state->argv[state->argc] == &state->protocol.buf[state->protocol.pos]) {
                // drop extra spaces
                return;
            }
            if (state->protocol.pos < state->protocol.buf_length) {
                state->protocol.buf[state->protocol.pos] = 0;
                state->protocol.pos++;
            }
            state->argc++;
            if (state->argc <= state->protocol.argv_length) {
                state->argv[state->argc] = &state->protocol.buf[state->protocol.pos];
            }
            return;
        }

        if (state->protocol.pos < state->protocol.buf_length) {
            state->protocol.buf[state->protocol.pos] = data;
            state->protocol.pos++;
        }

        return;
    }

}
