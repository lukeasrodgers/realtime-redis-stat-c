#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 128

struct command {
  long count;
  const char *name;
  struct command *next;
};

/* counter for "maybe print" */
int mp = 0;

char command_type[30];

const char *key_commands[30] = {
  "del", "dump", "exists", "expire", "expireat", "keys", "migrate",
  "move", "object", "persist", "pexpire", "pexpireat", "pttl",
  "randomkey", "rename", "renamenx", "restore", "sort", "ttl",
  "type", "wait", "scan"
};
const int num_key_commands = 22;

const char *set_commands[30] = {
  "sadd", "scard", "sdiff", "sdiffstore", "sinter", "sinterstore", "sismember",
  "smembers", "smove", "spop", "srandmember", "srem", "sunion", "sunionstore",
  "sscan"
};
const int num_set_commands = 15;

const char *list_commands[30] = {
  "blpop", "brpop", "brpoplpush", "lindex", "linsert", "llen", "lpop", "lpush",
  "lpushx", "lrange", "lrem", "lset", "ltrim", "rpop", "rpoplpush", "rpush",
  "rpushx"
};
const int num_list_commands = 17;

const char *string_commands[30] = {
  "append", "bitcount", "bitop", "bitpos", "decr", "decrby", "get", "getbit",
  "getrange", "getset", "incr", "incrby", "incrbyfloat", "mget", "mset", "msetnx",
  "psetnx", "set", "setbit", "setex", "setnx", "setrange", "strlen"
};
const int num_string_commands = 17;
const char **commands[30];

int main(int argc, char *argv[]) {
  ssize_t read(int fildes, void *buf, size_t nbyte);

  int i, j, commandline_options_check;
  char buffer[BUFSIZE];
  char line[BUFSIZE];
  ssize_t bytes_read = 0;

  void handle_line(char (*buf)[BUFSIZE], struct command *command_list, int bytes_read);
  void print_header();
  struct command *initialize_commands();
  int handle_options(int argc, char **argv[]);

  if ((commandline_options_check = handle_options(argc, &argv)) == 1) {
    return 1;
  }

  struct command *command_list = initialize_commands();
  print_header();
  // NB amount put into buffer will not be null-terminated
  while((bytes_read = read(STDIN_FILENO, buffer, BUFSIZE)) > 0) {
    for (i = 0, j = 0; i < (int) bytes_read; i++, j++) {
      if (buffer[i] != '\n') {
        line[j] = buffer[i];
      }
      else {
        handle_line(&line, command_list, j);
        j = 0;
      }
    }
  }
    
  return 0;
}

void handle_line(char (*buf)[BUFSIZE], struct command *command_list, int bytes_read) {
  char cmd[30];
  int get_cmd_success;
  int extract_command_name(char (*buf)[BUFSIZE], int bytes_read, char (*cmd)[30]);
  int increment_command_count(char (*cmd)[30], struct command *command_list);
  void maybe_print(struct command *command_list);

  get_cmd_success = extract_command_name(buf, bytes_read, &cmd); 
  if (get_cmd_success == 1) {
    fprintf(stderr, "no command read\n");
  }
  else {
    printf("extracted cmd: %s\n", cmd);
    increment_command_count(&cmd, command_list);
  }
  maybe_print(command_list);
  // %zu is C99 
  /* printf("read %zu bytes\n", bytes_read); */
  // we can also cast to unsigned long
  /* printf("read %ld bytes\n", (unsigned long) bytes_read); */
  /* printf("buf: %.*s", bytes_read, *buf); */

  return;
}

int extract_command_name(char (*buf)[BUFSIZE], int bytes_read, char (*cmd)[30]) {
  /* printf("read %d bytes\n", bytes_read); */
  /* printf("buf: %.*s\n", bytes_read, *buf); */
  int i = 0;
  int j, start_pos;
  int cmd_start_offset = 3;
  char c;
  j = 0;
  while (i < bytes_read) {
    /* printf("i: %d, c: %c\n", i, (*buf)[i]); */
    if ((*buf)[i] == ']') {
      start_pos = i + cmd_start_offset;
      i = start_pos;
      break;
    }
    i++;
  }
  if (i == bytes_read) {
    return 1;
  }
  while ((c = (*buf)[i]) != '"' && i < bytes_read) {
    (*cmd)[j] = c;
    j++;
    i++;
  }
  (*cmd)[j] = '\0';
  return 0;
}


int increment_command_count(char (*cmd)[30], struct command *command_list) {
  int i;
  struct command *cur_command = command_list;
  while (1) {
    if (strcmp(cur_command->name, (*cmd)) == 0) {
      cur_command->count++;
      break;
    }
    else if (cur_command->next != NULL) {
      cur_command = cur_command-> next;
    }
    else {
      break;
    }
  }
  return 0;
}

void maybe_print(struct command *command_list) {
  struct command *cur_command = command_list;
  while (1) {
    printf("%ld\t", cur_command->count);
    if (cur_command->next != NULL) {
      cur_command = cur_command->next;
    }
    else {
      printf("\n");
      break;
    }
  }
  mp++;
  return;
}

void print_header() {
  int i, num_commands;
  if (strcmp(command_type, "key") == 0) {
    num_commands = num_key_commands;
    /* linked list of commands is generated backwards from array,
     * so interate in reverse */
    for (i = num_commands - 1; i >= 0; i--) {
      /* truncate command name to ensure even spacing */
      printf("%.5s\t", key_commands[i]);
    }
    printf("\n");
  }
}

struct command *initialize_commands() {
  struct command *add_command(struct command *current_command, int i);
  int i;
  int num_commands;
  struct command *current_command = NULL;
  if (strcmp(command_type, "key") == 0) {
    *commands = key_commands;
    num_commands = num_key_commands;
  }
  for (i = 0; i < num_commands; i++) {
    current_command = add_command(current_command, i);
  }
  return current_command;
}

struct command *add_command(struct command *current_command, int i) {
  struct command *new_command = (struct command*) malloc(sizeof(struct command));
  printf("adding %s\n", (*commands)[i]);
  new_command->count = 0;
  new_command->name = (*commands)[i];
  /* current_command may be NULL, which is okay */
  new_command->next = current_command;
  return new_command;
}

int handle_options(int argc, char **argv[]) {
  if (argc < 1) {
    fprintf(stderr, "must provide at least one option");
    return 1;
  }
  strcpy(command_type, "key");
  return 0;
}
