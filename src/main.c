#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// https://man.freebsd.org/cgi/man.cgi?query=sysexits&manpath=FreeBSD+4.3-RELEASE
#define EX_USAGE 64
#define EX_DATAERR 65
#define EX_IOERR 74

void run(char *source) {
  printf("Running: %s", source);
  free(source);
}

void runrepl() {
  size_t n = 0;
  char *line = NULL;

  printf("Welcome to Hydrogen, the lightest programming language!\n");
  while (true) {
    printf("> ");
    if (getline(&line, &n, stdin) != -1) {
      run(line);
    }
  }
}

char *readfile(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open file, %s.\n", filename);
    exit(EX_IOERR);
  }

  fseek(file, 0L, SEEK_END);
  size_t filesize = ftell(file);
  rewind(file);

  char *buffer = malloc(filesize + 1); // +1 for '/0'
  if (buffer == NULL) {
    fprintf(stderr, "Could not read file, %s.\n", filename);
    exit(EX_IOERR);
  }

  size_t bytesread = fread(buffer, sizeof(char), filesize, file);
  if (bytesread < filesize) {
    fprintf(stderr, "Could not read file, %s.\n", filename);
    exit(EX_IOERR);
  }

  buffer[bytesread] = '\0';
  fclose(file);
  return buffer;
}

void runfile(const char *filename) {
  char *dot = strrchr(filename, '.');
  if (dot == NULL || (strcmp("hydro", dot + 1) != 0)) {
    fprintf(stderr, "File must have .hydro extension.\n");
    exit(EX_DATAERR);
  }

  char *source = readfile(filename);
  run(source);
}

void usage() {
  printf("\nUSAGE:\n");
  printf("\thydro [FILE]\n");
  printf("DESCRIPTION\n");
  printf("\tRuns hydrogen FILE which must have .hydro extension. If FILE is "
         "not provided, runs REPL.\n\n");
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    runrepl();
  } else if (argc == 2) {
    runfile(argv[1]);
  } else {
    usage();
    return EX_USAGE;
  }

  return EXIT_SUCCESS;
}
