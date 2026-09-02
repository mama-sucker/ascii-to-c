/*
 * ascii_to_c.c
 *
 * Reads a file containing raw ASCII art and generates a text file
 * containing a ready-to-use C string variable and printf() call.
 *
 * Usage:
 *   ./ascii_to_c <input_art.txt> <output.txt> [variable_name]
 *
 * Example:
 *   ./ascii_to_c logo.txt logo_out.txt logo
 *
 * Produces output.txt containing something like:
 *
 *   const char *logo =
 *       "  /\\_/\\  \n"
 *       " ( o.o ) \n"
 *       "  > ^ <  \n";
 *
 *   // Example usage:
 *   printf("%s", logo);
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 4096

/* Writes one raw line as a properly escaped, quoted C string literal
 * (without the trailing \n from the source line, since we add our own). */
static void write_escaped_line(FILE *out, const char *line) {
    fputc('"', out);
    for (const char *p = line; *p; p++) {
        switch (*p) {
            case '\\': fputs("\\\\", out); break;
            case '"':  fputs("\\\"", out); break;
            case '\t': fputs("\\t", out);  break;
            case '\r': break; /* strip stray CR from CRLF files */
            default:
                fputc(*p, out);
        }
    }
    fputs("\\n\"", out);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_art.txt> <output.txt> [variable_name]\n", argv[0]);
        return 1;
    }

    const char *input_path = argv[1];
    const char *output_path = argv[2];
    const char *var_name = (argc >= 4) ? argv[3] : "ascii_art";

    FILE *in = fopen(input_path, "r");
    if (!in) {
        perror("Failed to open input file");
        return 1;
    }

    FILE *out = fopen(output_path, "w");
    if (!out) {
        perror("Failed to open output file");
        fclose(in);
        return 1;
    }

    char line[MAX_LINE_LEN];
    int line_count = 0;

    fprintf(out, "const char *%s =\n", var_name);

    while (fgets(line, sizeof(line), in)) {
        /* strip trailing newline */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        fputs("    ", out);
        write_escaped_line(out, line);
        fputc('\n', out);
        line_count++;
    }

    fputs(";\n\n", out);
    fprintf(out, "// Example usage:\n");
    fprintf(out, "printf(\"%%s\", %s);\n", var_name);

    fclose(in);
    fclose(out);

    if (line_count == 0) {
        fprintf(stderr, "Warning: input file was empty, no lines processed.\n");
    } else {
        printf("Processed %d line(s). Output written to %s\n", line_count, output_path);
    }

    return 0;
}