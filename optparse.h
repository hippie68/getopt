/*
Parses a program's command-line arguments to identify and process predefined
command-line options before the actual program starts. After parsing, the
remaining, non-option arguments (aka positional arguments, or operands) are
accessible through the main function's adjusted argc/argv.

Features:
  * Supports both short and long options, and short option blocks ("-abc").
  * Arguments following "--" are treated as non-options.
  * Options can be used to:
      - set flags (true/false)
      - store option-arguments in variables/arrays
      - call functions with or without option-arguments as parameters
      - convert option-arguments to specific data types

Visit https://github.com/hippie68/optparse for updates and to report issues.
*/

#ifndef OPTPARSE_H
#define OPTPARSE_H

// Various fixed strings that can be customized here
#define OPTPARSE_STRING_OPTION_DOUBLE_DASH \
  "Arguments following this are not treated as options."

// Enabling this checks options for valid content when calling optparse_init().
// Should be disabled when compiling release builds.
#define OPTPARSE_DEBUG 1

// Causes optparse to stop when it encounters an error.
#define OPTPARSE_HALT_ON_ERROR 0

// Enables the function optparse_print_help().
#define OPTPARSE_HELP 1

// May be required for certain platforms
#define OPTPARSE_FUNCTION_POINTER_COMPATIBILITY 0

enum optparse_action {
  /// Flag actions -------------------------------------------------------------
  OPTPARSE_ACTION_SET_TRUE = 1,
  OPTPARSE_ACTION_SET_FALSE,
  OPTPARSE_ACTION_TOGGLE,
  OPTPARSE_ACTION_INCREMENT,
  OPTPARSE_ACTION_DECREMENT,
  /// Store actions ------------------------------------------------------------
  OPTPARSE_ACTION_STORE,
  OPTPARSE_ACTION_APPEND,
  /// Call actions -------------------------------------------------------------
  OPTPARSE_ACTION_CALL,       // func(TYPE arg);
                              // func(int n, TYPE arg_split[]); (if .list_delim)
  OPTPARSE_ACTION_CALL_VOID,  // func();
  OPTPARSE_ACTION_CALL_PARSE, // func(int argc, char **argv, int *index);
};

// Specifies which type conversion to apply to the option-argument.
enum optparse_type {
  OPTPARSE_TYPE_STR, // No conversion; default
  OPTPARSE_TYPE_CHAR,
  OPTPARSE_TYPE_SCHAR,
  OPTPARSE_TYPE_UCHAR,
  OPTPARSE_TYPE_SHRT,
  OPTPARSE_TYPE_USHRT,
  OPTPARSE_TYPE_INT,
  OPTPARSE_TYPE_UINT,
  OPTPARSE_TYPE_LONG,
  OPTPARSE_TYPE_ULONG,
  OPTPARSE_TYPE_LLONG,
  OPTPARSE_TYPE_ULLONG,
  OPTPARSE_TYPE_FLT,
  OPTPARSE_TYPE_DBL,
  OPTPARSE_TYPE_LDBL,
};

// Specifies how exactly an option calls a function.
// TYPE: normally char *, unless a type conversion took place (by setting .type)
// arg: the (possibly TYPE-converted) option-argument
// arg_split: an array containing the (possibly TYPE-converted)
//   option-argument's list elements
// OPTPARSE_CALL_PARSE can be used to manually parse option-arguments, e.g. to
// implement multiple/unlimited option-arguments.
enum optparse_function_call {
  OPTPARSE_CALL,       // func(TYPE arg);                             // Default
                       // func(int n, TYPE *arg_split); (if .list_delim is set)
  OPTPARSE_CALL_RAW,   // func(char *arg);
  OPTPARSE_CALL_VOID,  // func();
  OPTPARSE_CALL_PARSE, // func(int argc, char **argv, int *index);
};

// Defines a single command line option.
struct optparse_opt {
  /// Required members ---------------------------------------------------------
  signed char short_name;      // The option's short name; -1 marks the end of
                               // an option array passed to optparse_init()
  char *long_name;             // The option's long name, without "--"
  enum optparse_action action; // Specifies how to use .dest
  #if OPTPARSE_FUNCTION_POINTER_COMPATIBILITY
  union {
    void *dest;                // Pointer to a variable
    void (*dest_func)(void);   // Pointer to a function (must be used for CALL
                               // actions if the compiler does not support
  }                            // casting function pointers to void *)
  #else
  void *dest;                  // Pointer to a variable/function
  #endif
  /// Optional members ---------------------------------------------------------
  char *arg;                   // Option-argument(s),
                               // e.g. "<file name> <directory name>"
                               //   or "FILE DIRECTORY
  int argc;                    // Number of required option-arguments (-1/0/1)
                               // -1: the argument is optional
  enum optparse_type type;     // Data type the argument must be converted to
  long double min;             // Minimum argument string length, or minimum
                               // numeric value for type-converted arguments
  long double max;             // Maximum argument string length, or maximum
                               // numeric value for type-converted arguments
  char *description;           // The option's detailed documentation
  int *dest_len;               // Used to keep track of the number of appended
                               // variables or stored list items
  /// List handling (optional) -------------------------------------------------
  char *list_delim;            // Characters that separate list items
  int list_len_min;            // Minimum allowed number of list items
  int list_len_max;            // Maximum allowed number of list items
};

// Registers options; must be called before calling optparse_parse().
void optparse_init(struct optparse_opt options[]);

// Parses command line arguments as specified in optparse_init's options, and
// alters argc/argv to store remaining non-option arguments.
// Returns 0 on success, 1 on error.
int optparse_parse(int *argc, char *argv[]);

#if OPTPARSE_HELP
// Registers help text.
// help_header and help_footer are optional strings that, if
// not NULL, are used during optparse_print_help(). Their purpose is to print
// custom help text before and after the options are printed. help_header
// usually contains usage information and the program manual, help_footer could
// be used to print website links or other footnote information.
void optparse_help_init(char *help_header, char *help_footer);

// Prints help information passed to optparse_init().
void optparse_print_help();
#endif

#endif
