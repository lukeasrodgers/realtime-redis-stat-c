#ifndef COMMANDS
#define COMMANDS

struct command {
  long count;
  const char *name;
  struct command *next;
};

extern struct command *initialize_commands();
extern int handle_options(int argc, char **argv[]);

#endif
