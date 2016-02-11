#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "commands.h"

const char **commands[30];
static char command_type[30];
int num_commands;

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

struct command *initialize_commands() {
  struct command *add_command(struct command *current_command, int i);
  int i;
  int num_commands;
  struct command *current_command = NULL;
  if (strcmp(command_type, "key") == 0) {
    *commands = key_commands;
    num_commands = num_key_commands;
  }
  else if (strcmp(command_type, "set") == 0) {
    *commands = set_commands;
    num_commands = num_set_commands;
  }
  else if (strcmp(command_type, "list") == 0) {
    *commands = list_commands;
    num_commands = num_list_commands;
  }
  else if (strcmp(command_type, "string") == 0) {
    *commands = string_commands;
    num_commands = num_string_commands;
  }
  for (i = 0; i < num_commands; i++) {
    current_command = add_command(current_command, i);
  }
  return current_command;
}

int handle_options(int argc, char **argv[]) {
  if (argc < 2) {
    fprintf(stderr, "must provide at least one option\n");
    return 1;
  }
  printf("arg1: %s\n", (*argv)[1]);
  if (strcmp((*argv)[1], "key") == 0) {
    strcpy(command_type, "key");
    num_commands = num_key_commands;
  }
  else if (strcmp((*argv)[1], "set") == 0) {
    strcpy(command_type, "set");
    num_commands = num_set_commands;
  }
  else if (strcmp((*argv)[1], "list") == 0) {
    strcpy(command_type, "list");
    num_commands = num_list_commands;
  }
  else if (strcmp((*argv)[1], "string") == 0) {
    strcpy(command_type, "string");
    num_commands = num_string_commands;
  }
  else {
    fprintf(stderr, "invalid command group\n");
    return 1;
  }
  return 0;
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

