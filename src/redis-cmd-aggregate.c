#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>

#include "commands.h"

#define BUFSIZE 128

/* counter for "maybe print" */
int mp = 0;

extern const char **commands[];
extern int num_commands;

int main(int argc, char *argv[]) {
  ssize_t read(int fildes, void *buf, size_t nbyte);

  int i, j, commandline_options_check;
  char buffer[BUFSIZE];
  char line[BUFSIZE];
  ssize_t bytes_read = 0;

  void handle_line(char (*buf)[BUFSIZE], struct command *command_list, int bytes_read);
  void print_header();

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
  else if (get_cmd_success == 0) {
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
    if (strcmp(*buf, "OK") == 0) {
      return 2;
    }
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
  int i;
  /* linked list of commands is generated backwards from array,
   * so interate in reverse, and truncate name to ensure even spacing
   * at the cost of some legibility */

  for (i = num_commands - 1; i >= 0; i--) {
    printf("%.5s\t", (*commands)[i]);
  }
  printf("\n");
  return;
}
