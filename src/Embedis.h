/*  Embedis - Embedded Dictionary Server
    Copyright (C) 2016 PatternAgents, LLC

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

#ifndef EMBEDIS_H
#define EMBEDIS_H
#ifdef __cplusplus


/* Arduino is like a box of chocolates,
   You never know what you're going to get...
   
   could be any compiler, version, or c/c++ libraries,
   could be any processor architecture, board, configuration, ...
   I have looked at many examples, but if anyone has a better,
   more canonical way to do this, please, please show me...
*/
/* Non-Specific (i.e.generic) Includes */
/* these should generally work everywhere... */
#include <stddef.h>
#include <stdint.h>

/* C/C++ Compiler Specific */
#if   defined ( __CC_ARM )
  #define __ASM            __asm                                      /*!< asm keyword for ARM Compiler          */
  #define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler       */
  #define __STATIC_INLINE  static __inline
#elif defined ( __ICCARM__ )
  #define __ASM            __asm                                      /*!< asm keyword for IAR Compiler          */
  #define __INLINE         inline                                     /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */
  #define __STATIC_INLINE  static inline
#elif defined ( __TMS470__ )
  #define __ASM            __asm                                      /*!< asm keyword for TI CCS Compiler       */
  #define __STATIC_INLINE  static inline
#elif defined ( __GNUC__ )
  #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
  #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
  #define __STATIC_INLINE  static inline
  /* Obsolete versions are stubbed out for now...
     #include <cstddef>
     #include <cstdint>
     #include <new>
   */
#elif defined ( __TASKING__ )
  #define __ASM            __asm                                      /*!< asm keyword for TASKING Compiler      */
  #define __INLINE         inline                                     /*!< inline keyword for TASKING Compiler   */
  #define __STATIC_INLINE  static inline

#endif

/* Architecture Specific */
#if defined(ARDUINO_ARCH_AVR)
   /* AVR specific code - break down further by cpu model if necessary */
   // #pragma message ( "AVR Architecture Selected" )
   #include "avr/pgmspace.h"
    /* it's NEW! */
	/* "<new>" doesn't seem to be supported on all platforms/compilers           */
	/* this should likely be selected per compiler and not by Arduino IDE...     */
	/* but this works, until I can track down all the difference in compilers... */
	inline void * operator new(size_t size, void * ptr) {
      (void)size;
      return ptr;
    }
#elif defined(ARDUINO_ARCH_SAM)
   /* SAM3X specific code - break down further by cpu model if necessary */
   // #pragma message ( "SAM3X Architecture Selected" )
  #include <new>
#elif defined(ARDUINO_ARCH_SAMD)
   /* SAMD specific code - break down further by cpu model if necessary */
   // #pragma message ( "SAMD Architecture Selected" )
  #include <new>
#elif defined(ARDUINO_ARCH_ARC32)
   /* Currie/101 specific code - break down further by cpu model if necessary */
   // #pragma message ( "ARC32/Currie Architecture Selected" )
    /* it's NEW! */
	/* "<new>" doesn't seem to be supported on all platforms/compilers           */
	/* this should likely be selected per compiler and not by Arduino IDE...     */
	/* but this works, until I can track down all the difference in compilers... */
	inline void * operator new(size_t size, void * ptr) {
      (void)size;
      return ptr;
    }
#elif defined(ARDUINO_ARCH_ESP8266)
   /* ESP8266 specific code - break down further by cpu model if necessary */
   // #pragma message ( "ESP8266 Architecture Selected" )
  #include <new>
#elif defined(CORE_TEENSY)
   /* Teensy3.x specific code - break down further by cpu model if necessary */
   /* this is not ideal, should change to use "_ARCH_" */ 
   // #pragma message ( "Teensy Core Architecture Selected" )
  #include "avr/pgmspace.h"
  #include <new>
#elif defined(__ARDUINO_X86__)
   /* Edison, Galileo, x86 specific code - break down further by cpu model if necessary */
   /* this is not ideal, should change to use "_ARCH_" */ 
   // #pragma message ( "X86 Architecture Selected" )
   #include "avr/pgmspace.h"
#else
  /* untested architecture, it might work, but likely not... */
  // #pragma message "Core Architecture not Recognized - untested... "
  #include "avr/pgmspace.h"
  #include <new>
#endif

/* Arduino Version Specific */
#if defined(ARDUINO)
    #include "WString.h"
    #include "Stream.h"
  #if (ARDUINO >= 100)
    /* newer Arduino IDE Version */
    #include "Arduino.h"
  #else
   /* older Arduino IDE Version */
   #include "WProgram.h"
  #endif
#else
   /* NOT the Arduino IDE... */
   /* (i.e. Travis Testing Framework */
   /* these files generally need to be provided to the testing framework */
   /* if it is not within the Arduino IDE, which is why they are called out here */
   #include "WString.h"
   #include "Stream.h"
#endif

/* embedis class */
class Embedis {
protected:

    // Tiny vector.
    template <typename T>
    class TVec {
        size_t length;
        T* ptr;
        void refinish (size_t n) {
            if (n == 0) {
                free(ptr);
                ptr = 0;
                length = 0;
                return;
            }
            T* reptr = (T*)realloc(ptr, sizeof(T) * n);
            if (reptr) {
                ptr = reptr;
                for (size_t i = length; i < n; i++) {
                    new(&ptr[i]) T;
                }
                length = n;
            }
        }
    public:
        TVec () : length(0), ptr(0) {}
        ~TVec () {
            resize(0);
        }
        TVec(const TVec&);
        TVec& operator=(const TVec&);
        T& operator[] (const size_t index) {
            return ptr[index];
        }
        const T& operator[] (const size_t index) const {
            return ptr[index];
        }
        size_t size () const {
            return length;
        }
        void resize (size_t n) {
            if (n == length) return;
            for (size_t i = n; i < length; i++) {
                ptr[i].~T();
            }
            refinish(n);
        }
        void remove (size_t n) {
            if (n >= length) return;
            ptr[n].~T();
            memmove(&ptr[n], &ptr[n+1], sizeof(T) * (length-n-1));
            refinish(length-1);
        }
    };

    size_t argvlen;
    size_t buflen;
    char* buf;
    size_t pos;
    char mode;
    size_t mode_args;
    int mode_chars;
    size_t subcount;
    size_t current_dict;

    struct Channel {
        String name;
        TVec<Embedis*> subs;
    };
    static TVec<Channel> channels;

    struct Command {
        String name;
        void (*call)(Embedis*);
    };
    static TVec<Command> commands;

    struct Dictionary {
        String name;
        union {
            struct {
                // required:
                bool (*get)(const String& key, String& value);
                // optional:
                bool (*set)(const String& key, const String& value);
                bool (*del)(const String& key);
                void (*keys)(Embedis*);
                void (*select)(Embedis*);
            };
            struct {
                // set custom to zero for ram type
                // it's non-zero when "get" is set
                void* custom;
                // required:
                size_t size;
                char (*fetch)(size_t pos);
                void (*store)(size_t pos, char value);
                // optional:
                void (*commit)();
            };
        };
    };
    static TVec<Dictionary> dictionaries;

    struct Key {
        PGM_P name;
        short int id;
    };
    static TVec<Key> keys;

    struct Hardware  {
        String name;
        void (*read)(Embedis*);
        void (*write)(Embedis*);
    };
    static TVec<Hardware> hardwares;

public:

    // Well known responses
    enum Status {
        OK,    // generic, no extra message
        ERROR, // generic, no extra message
        UNKNOWN_COMMAND,
        SYNTAX_ERROR,
        BUFFER_OVERFLOW,
        ARGS_ERROR,
        NOT_FOUND
    };

    // Complicated responses can be output directly here.
    // See the response() overloads for more convenience.
    Stream* const stream;

    // The argc and argv guarantee:
    // argv[0] is always the command.
    // All args are zero-terminated. However, to support binary payloads
    // the commands are guaranteed to be in order. This allows you to subtract
    // the pointers, plus the zero, to obtain the exact length.
    // e.g. size_t argv1len = argv[2] - argv[1] - 1;
    size_t argc;
    char** const argv;

    // Constructor / destructor / disallow copy and move
    Embedis(Stream& stream, size_t buflen = 128, size_t argvlen = 8);
    ~Embedis();
    Embedis(const Embedis&);
    Embedis& operator=(const Embedis&);

    // This can reset the input state when, for example, a break signal
    // arrives on your UART. When everything is true all subscriptions
    // are cancelled and the selected dictionary is reset to default.
    void reset(bool everything = false);

    // Call this in your Arduino loop()
    // void loop() { myembedis.process() }
    void process();

    // Your custom commands can use these for their response.
    // It's also fine to use the stream directly.
    void response(Status);
    void response(char, int);
    void response(const String&);
    void response(const __FlashStringHelper*);

    // Add custom commands.
    // Embedis::command( F("REBOOT"), my_func_call);
    static void command(const String& name, void (*call)(Embedis*));

    // Hardware access. Both read and write are optional. For example,
    // your could read a temperature sensor or turn on a fan.
    static void hardware(const String& name,
                         void (*read)(Embedis*),
                         void (*write)(Embedis*));

    // Add a fully custom dictionary. Only "get" is required.
    static void dictionary(const String& name,
                           bool (*get)(const String& key, String& value),
                           bool (*set)(const String& key, const String& value) = 0,
                           bool (*del)(const String& key) = 0,
                           void (*keys)(Embedis*) = 0,
                           void (*select)(Embedis*) = 0);

    // Add a dictionary using the built-in key-value store algorithm.
    // This is designed for random access storage such as EEPROM, FRAM,
    // or even RAM. The optional "commit" is called at the conclusion
    // of any write sequences.
    static void dictionary(const String& name,
                           size_t size,
                           char (*fetch)(size_t pos),
                           void (*store)(size_t pos, char value),
                           void (*commit)() = 0);

    // To avoid storing keys as strings with the built-in key-value store,
    // you can give them a numeric id (>0). Beware that if you change
    // a key's id or stop assigning it one that key will be deleted
    // from the store. However, changing an id's name has the effect of
    // renaming the key.
    static void key(const __FlashStringHelper* name, short int id);

    // Publish a message to all subscribers.
    // Embedis::publish( F("syslog"), F("All systems nominal.") );
    static size_t publish(const String& channel, const String& message);

    // Dictionary access.
    static bool get(const String& key, String& value);
    static bool get(const String& dict, const String& key, String& value);
    static bool set(const String& key, const String& value);
    static bool set(const String& dict, const String& key, const String& value);
    static bool del(const String& key);
    static bool del(const String& dict, const String& key);

private:

    void dispatch();

    static size_t find_dictionary(const String& name, bool allow_create = false);

    static void add_basic();
    static void add_readwrite();
    static void add_dict();

    static void cmd_COMMANDS(Embedis*);
    static void cmd_PUBLISH(Embedis*);
    static void cmd_SUBSCRIBE(Embedis*);
    static void cmd_UNSUBSCRIBE(Embedis*);

    static void cmd_HARDWARE(Embedis*);
    static void cmd_READ(Embedis*);
    static void cmd_WRITE(Embedis*);

    static void cmd_DICTIONARIES(Embedis*);
    static void cmd_GET(Embedis*);
    static void cmd_SET(Embedis*);
    static void cmd_DEL(Embedis*);
    static void cmd_KEYS(Embedis*);
    static void cmd_SELECT(Embedis*);

    static short int key_to_id(const char* key, size_t length);
    static PGM_P key_from_id(short int id);

    static bool kvs_get(Dictionary* dict, const char* key, size_t key_len, String& value);
    static bool kvs_set(Dictionary* dict, const char* key, size_t key_len, const char* value, size_t value_len);
    static bool kvs_del(Dictionary* dict, const char* key, size_t key_len);
    static void kvs_keys(Dictionary* dict, Embedis*);

    static int kvs_work(Dictionary* dict, const char* key_name, int key_len,
                        const char* value, size_t* value_len, size_t* value_pos);

    static int kvs_reset(Dictionary* dict, const char* key_name, int key_len,
                         const char* value, size_t* value_len, size_t* value_pos);

    static void kvs_cleanup(Dictionary* dict);

};


#endif /* __cplusplus */
#endif /*EMBEDIS_H*/
