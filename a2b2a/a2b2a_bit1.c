#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <math.h>


void errorexit(char *fmt, ...) {    
   va_list arg;

   va_start(arg, fmt);
   printf(" *** ERROR: *** ");
   vprintf(fmt, arg);
   va_end(arg);
   printf("\n");
   exit(1);
}


void write_bytes(uint32_t u32bits, FILE *fw, int i_bit, int nbits) {
   unsigned char value[4];
   int bc, q, r;

   value[0] = (uint32_t)u32bits >> 24;
   value[1] = (uint32_t)u32bits >> 16;
   value[2] = (uint32_t)u32bits >> 8;
   value[3] = (uint32_t)u32bits;
   bc = i_bit + nbits;
   q = bc/8;
   r = bc%8;
   if (r == 0) {
      bc = q;
   } else {
      bc = q + 1;
   }
   //printf("BIN VALUE: %x %x %x %x (bc=%d,nbits=%d,i_bit=%d)\n",
   //      value[0], value[1], value[2], value[3], bc, nbits, i_bit);
   fwrite(value, bc, 1, fw);
}
int read_1byte(FILE *f) {
   return fgetc(f);
}
int read_bytes(uint32_t *u32bits, FILE *fr){
   // Returns number of bytes read and value in u32bits
   // 
   uint32_t u, ut;
   unsigned char value[5];
   int bc, tbc;
   int i;

   bc = 0;
   for (i=0; i<4; i++) {
      tbc = fread((void *)value+i, 1, 1, fr);
      if (tbc < 0) {
         errorexit("Unable to read file. tbc=%d\n", tbc);
      }
      if (tbc == 0 ){
         break;
      }
      bc += 1;
   }
   if (bc == 0) {
      //printf("READ_BYTES: END OF FILE\n");
      return 0;
   }
   u = 0;
   for(i=0; i < bc; i++){
      //printf("READ_BYTES: value[%d]=0x%x\n", i, value[i]);
      ut = value[i];
      u += ut << (24 - i*8);
   }
   *u32bits = u;
   //printf("READ_BYTES returns: u32bits=0x%x(%x),bc=%d\n", u, *u32bits, bc);
   return bc;
}


int bin_2_ascii_xbit(int nbits, char *in_file, char *out_file) {
   FILE *f;
   FILE *fw;
   uint32_t bucket, n, nbits_mask, u32bits_w;
   int bc, bib; // bib is Bits In Bucket
   int bits_in_ll;

   f = fopen(in_file, "rb");
   if (f == NULL) {
      printf("cant open file:%s.", in_file);
      exit(1);
   }
   fw = fopen(out_file, "w");
   if (fw == NULL) {
      printf("cant open output file %s", out_file);
      exit(1);
   }
   bucket = 0;
   bib = 0;
   bits_in_ll = 0;
   nbits_mask = pow(2, nbits) - 1; // 3 bits is 111 = &, 4 bits is 1111 = 15
   while(1) {
      bc = read_1byte(f);
      if (bc == EOF) {
         //printf("END OF FILE\n");
         return 0;
      }
      //printf("BEGIN:GOT A CHAR: %d (0x%x), bib=%d\n", bc, bc, bib);
      //printf("OLD BUCKET: 0x%x bib=%d\n", bucket, bib);
      bucket <<= 8;
      bucket = bucket + (bc & 0xff);
      bib += 8;
      //printf("CHAR IN BUCKET: (0x%x), bib=%d\n", bucket, bib);
      while (bib >= nbits) {
         n = bucket >> (bib - nbits) & nbits_mask;
         //printf("NUMBER FOUND: %d (0x%x)\n", n, n);
         //printf("B4 EXTRACT: bucket:0x%x bib=%d\n", bucket, bib);
         bib -= nbits;
          bits_in_ll += nbits;
         u32bits_w = pow(2, bib) - 1;
         bucket = bucket &  u32bits_w;
         //printf("AFTER EXTRACT: bucket:0x%x bib=%d\n", bucket, bib);
         if (nbits > 1 && n != 0) {
            fprintf(fw, " %d", n);
         }
      }
      //printf("END:NEW BUCKET: 0x%x bib=%d\n============\n", bucket, bib);
      if ((32-bits_in_ll) < nbits) {
         //printf("B4 RESET: bucket:0x%x bib=%d\n", bucket, bib);
         bits_in_ll = 0;
         bib = 0;
         bucket = 0;
      }
   }
   //fprintf(fw, "\n");
   fclose(f);
   fclose(fw);
   //printf("Finished!\n");
   return EXIT_SUCCESS;
}


int is_empty(const char *s) {
  while (*s != '\0') {
    if (!isspace((unsigned char)*s))
      return 0;
    s++;
  }
  return 1;
}


int ascii_2_bin_xbit(int nbits, char *in_file, char *out_file) {
   FILE *f;
   FILE *fw;
   unsigned char *line = NULL;
   size_t len = 0;
   ssize_t read;
   int i_bit;
   int n;
   char *word;
   uint32_t u32bits, u32bits_t;
   long long dll, dul;

   if (nbits < 2 || nbits > 31) {
      errorexit("wrong number of bits given: %d\n", nbits);
      exit(1);
   }
   f = fopen(in_file, "r");
   if (f == NULL) {
      printf("cant open input file: %s\n", in_file);
      exit(1);
   }
   fw = fopen(out_file, "wb");
   if (fw == NULL) {
      printf("cant open output file %s\n", out_file);
      exit(1);
   }
   //printf("Open successful\n");

   i_bit = -nbits;
   u32bits = 0;
   dul = pow(2,  nbits) - 1;
   dll = pow(2, nbits - 1);
   while ((read = getline((char **)&line, &len, f)) != -1) {
      //printf("line: %s\n", line);
      word = strtok((char *)line, " ");
      while (word != NULL){
         //printf("A word is:%s.\n", word);
         if (is_empty(word)) {
             word = strtok(NULL, " ");
            continue;
         }
         i_bit += nbits;
         n = strtoul(word, NULL, 10);
         //printf("GOT number:%d (%x)\n", n, n);
         if (n > dul) {
            errorexit("bad val:%d. nbits=%d, dul=%ld", n, nbits, dul);
         }
         if (n < dll) {
            errorexit("bad val:%d. nbits=%d, dll=%ld", n, nbits, dll);
         }
         u32bits_t = (uint32_t)n;
         u32bits += (uint32_t)u32bits_t << ((32-nbits) - i_bit);
         if (i_bit + nbits > (32 - nbits)) {
            //printf("WRITING: %x\n", u32bits);
            write_bytes(u32bits, fw, i_bit, nbits);
            i_bit = -nbits;
            u32bits = 0;
         }

         // Must be last
         word = strtok(NULL, " ");
      }
   }
   if (i_bit >= 0) {
      write_bytes(u32bits, fw, i_bit, nbits);
   }
    fclose(fw);
    fclose(f);
   return EXIT_SUCCESS;
}


/*
 * One bit compression/decompressions needs special functions
 */
int ascii_2_bin_1bit(int bitn, char *in_file, char *out_file) {

   FILE *f;
   FILE *fw;
   unsigned char *line = NULL;
   unsigned char b;
   size_t len = 0;
   ssize_t read;
   unsigned char cc;
   int ii;
   int i_bit;
   int n;
   char *word;
   //unsigned long ccc = 0;

   f = fopen(in_file, "r");
   if (f == NULL) {
      printf("cant open input file: %s\n", in_file);
      exit(1);
   }
   fw = fopen(out_file, "wb");
   if (fw == NULL) {
      printf("cant open output file %s\n", out_file);
      exit(1);
   }
   //printf("Open successful\n");

   i_bit = -1;
   b = 0;
   while ((read = getline((char **)&line, &len, f)) != -1) {
      //printf("line: %s\n", line);
      word = strtok((char *)line, " ");
      while (word != NULL){
         //printf("A word is:%s.\n", word);
         if (is_empty(word)) {
             word = strtok(NULL, " ");
            continue;
         }
         i_bit++;
         n = strtoul(word, NULL, 10);
         if (n == 1) {
            b += 1 << (7-i_bit);
         }
         if (i_bit >= 7){
	    //if (ccc % 1000000 == 0) printf("w %ld\n", ccc);
	    //ccc++;
            fwrite(&b, 1, 1, fw);
            i_bit = -1;
            b = 0;
         }
         // Must be last
         word = strtok(NULL, " ");
      }




      /*
      //printf("line: %ld\n", strlen(line));
      for (ii=0; ii < strlen((const char *)line); ii++) {
         cc = line[ii];
         if (isspace(cc)) {
            continue;
         }
         //printf("(%c)", cc);
         i_bit++;
         if (cc == '1') {
            b += 1 << (7-i_bit);
         }
         if (i_bit >= 7){
            fwrite(&b, 1, 1, fw);
            i_bit = -1;
            b = 0;
         }
      }
      //printf("\n\n");
      */
   }
   if (i_bit >= 0) {
      fwrite(&b, 1, 1, fw);
   }
   // last index of bit in last byte is in i_bit 
   // if -1, then all bits are valid.
   // 0 to 6 represet 1 to 7 bits are valid
   b = i_bit & 0xff;
   printf("WRITING COUNT BYTE AS:%x\n", b);
   fwrite(&b, 1, 1, fw);
   fclose(fw);
   fclose(f);
   return EXIT_SUCCESS;
}


int bin_2_ascii_1bit(int bitn, char *in_file, char *out_file) {
   FILE *f;
   FILE *fw;
   int i;
   int c_int;
   unsigned char last_c, bitcount_c, c;

   f = fopen(in_file, "rb");
   if (f == NULL) {
      printf("cant open file:%s.", in_file);
      exit(1);
   }
   fw = fopen(out_file, "w");
   if (fw == NULL) {
      printf("cant open output file %s", out_file);
      exit(1);
   }
   c_int = fgetc(f);
   last_c = (unsigned char) c_int;
   c_int = fgetc(f);
   bitcount_c = (unsigned char) c_int;
   while(1) {
      c_int = fgetc(f);
      if (c_int == EOF ) {
         break;
      }
     c = last_c;
     last_c = (unsigned char) bitcount_c;
     bitcount_c = (unsigned char) c_int;

     for (i=0; i<8; i++){
        if (c & 1<<(7-i)) {
           fputc(' ', fw);
           fputc('1', fw);
        } else {
           fputc(' ', fw);
           fputc('0', fw);
        }
     }
     //fputc('\n', fw);
   }
   printf("Writing %d partial bits\n", bitcount_c);
   if (bitcount_c == 0xff) {
      bitcount_c = 7;
   }
   printf("Writing partial bits\n");
   for (i=0; i<bitcount_c+1; i++) {
      if (last_c & 1<<(7-i)) {
         fputc(' ', fw);
         fputc('1', fw);
      } else {
         fputc(' ', fw);
         fputc('0', fw);
      }
   }
   //fputc('\n', fw);
   fclose(f);
   fclose(fw);
   printf("Finished!\n");
   return EXIT_SUCCESS;
}
