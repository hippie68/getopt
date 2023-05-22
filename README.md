# getopt

A thread-safe, more intuitive alternative to getopt_long() that features
word-wrapping help output and nested subcommands.

Parsing capabilities:
- Short options and short option groups: -a -b -c or -abc
- Short option arguments: -cARG or -c ARG or -abcARG
- Long option arguments: --option=ARG or --option ARG
- The order of options and operands (non-options) does not matter.
- End of options delimiter: "--" (after this, all arguments are treated as operands)
- Recognizes the stdin/stdout operand "-".
- Subcommands can be called from within a running getopt() switch.
- After parsing is done, argv only contains remaining operands.

## Quick example

```
#include "getopt.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int flag = 0;
    char *string = "unset";

    // 1. Define an option array.
    static struct option opts[] = {
        { 'f', "set-flag",   NULL,   "Set flag to 1."       },
        { 'h', "help",       NULL,   "Print help and quit." },
        { 's', "set-string", "NAME", "Set string to NAME."  },
        { 0 }
    };

    // 2. Use getopt().
    int opt;
    char *optarg;
    while ((opt = getopt(&argc, &argv, &optarg, opts)) != 0) {
        switch (opt) {
            case 'f':
                flag = 1;
                break;
            case 'h':
                printf("Options:\n");
                print_options(stdout, opts);
                return 0;
            case 's':
                string = optarg;
                break;
            case '?': // Error
                return 1;
        }
    }

    // Result:
    printf("flag: %d\nstring: %s\nargc: %d\n", flag, string, argc);
    for (int i = 0; i <= argc; i++)
        printf("argv[%d]: %s\n", i, argv[i] ? argv[i] : "NULL");
}
```

Output of option -h/--help:
```
Options:
  -f, --set-flag         Set flag to 1.
  -h, --help             Print help and quit.
  -s, --set-string NAME  Set string to NAME.
```

## Quick manual

```
struct option {
    int index;         // The option's unique identifier.
                       // A single alphanumeric ASCII character literal means
                       // this is the option's short name. Any other positive
                       // value means the option does not have a short name.
                       // A negative value means the option is a subcommand.
    char *name;        // The option's unique long name (without leading "--")
                       // or the subcommand's name.
    char *arg;         // A single option-argument or the subcommand's operands,
                       // as supposed to appear in the help screen
                       // (e.g. "ARG", "[ARG]", "<arg>", ...).
                       // Square brackets mean the argument is optional.
    char *description; // The option's or the subcommand's description.
};
```

- To hide an option from the help screen, use HIDEOPT as .description.

```
// Return values: 0 when done, '?' on error, otherwise an option's .index value.
int getopt(int *argc, char **argv[], char **optarg, const struct option opts[]);
```

- Returns '-' when encountering "-" (the stdin/stdout option).

```
// Prints the specified option array's options.
// Returns 1 if the array has subcommands, otherwise 0.
int print_options(FILE *stream, const struct option opts[]);
```

- The return value is useful to decide whether to use print_subcommands() inside a help function.

```
// Prints the specified option array's subcommands.
void print_subcommands(FILE *stream, const struct option opts[]);
```

### Subcommands

Each subcommand can use its own option array, which itself may contain other subcommands. One way to use subcommands is to put them inside dedicated functions, for example:

```
int subcommand1(int argc, char *argv[])
{
    static struct option opts[] = {
        ...
    };

    int opt;
    char *optarg;
    while ((opt = getopt(&argc, &argv, &optarg, opts)) != 0) {
        switch (opt) {
            ...
        }
    }

    // Handle remaining operands here.
    ...

    return 0;
}
```

Such subcommand functions can be called directly from within a running getopt() switch, by passing the current argc and argv variables:

```
int main(int argc, char *argv[])
{
    ...
    while ((opt = getopt(&argc, &argv, &optarg, opts)) != 0) {
        ...
        case -1:
            return subcommand1(argc, argv); // Transfer control to subcommand1().
        ...
```

### Preprocessor directives

```
#define GETOPT_LINE_MAXLEN 80 // Max. length of lines of text output.
#define GETOPT_BLCK_MINLEN 30 // Min. length of an option's description block.
```

---

If you want to share improvement ideas or report bugs, please create an [issue](https://github.com/hippie68/getopt/issues).
