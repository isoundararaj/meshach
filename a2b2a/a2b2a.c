/*
 ============================================================================
 Name        : a2b2a.c
 Author      : Manoj Daniel and Meshach Ponraj
 Version     : v1.0
 Copyright   : copyright (c) M&M
 Description : Convert files from/to ascii numbers and binary
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

extern int optind;

extern int ascii_2_bin_1bit();
extern int bin_2_ascii_1bit();

extern int ascii_2_bin_xbit(int, char*, char*);
extern int bin_2_ascii_xbit(int, char*, char*);

void usageexit(char *s, int argc, char *argv[]) {
   fprintf(stderr, "Error: %s\n", s);
   fprintf(stderr, "Usage: %s [-ab1234567] in-file out-file\n", argv[0]);
   exit(EXIT_FAILURE);
}
int main(int argc, char *argv[]) {
   int opt;
   char *in_file, *out_file;
   int bitn = 0;

   enum {
      NONE_A2B = 0,
      A2B,
      B2A,
   } a2b = NONE_A2B;

   while ((opt = getopt(argc, argv, "abc:")) != -1) {
      switch (opt) {
         case 'a': a2b = A2B; break;
         case 'b': a2b = B2A; break;
         case 'c':
             bitn = strtol(optarg, NULL, 10); break;
         default:
            usageexit("Unknown option given", argc, argv);
        }
    }
   if (a2b == NONE_A2B) {
      usageexit("Missing a2b (ascii 2 bin) or b2a  \
         (binary to ascii) argument", argc, argv);
   }
   if (bitn == 0 || bitn < 0 || bitn > 31) {
      usageexit("Missing Missing bit count specificatin \
         (bitcout is 1 to 31)", argc, argv);
   }

   if ((optind + 2) != argc) {
      usageexit("missing input and output files", argc, argv);
   }
   in_file = argv[optind];
   out_file = argv[optind+1];
   printf("%s\n", in_file);
   if (bitn == 1) {
      if (a2b == A2B) {
         ascii_2_bin_1bit(bitn, in_file, out_file);
      } else {
         bin_2_ascii_1bit(bitn, in_file, out_file);
      }
   } else {
      if (a2b == A2B) {
         ascii_2_bin_xbit(bitn, in_file, out_file);
      } else {
         bin_2_ascii_xbit(bitn, in_file, out_file);
      }
   }
   return EXIT_SUCCESS;
}
