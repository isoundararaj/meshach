#!/bin/bash -e
gcc -o a2b2a *.c
dpfx="/tmp/d"
rm $dpfx/*.bin || echo cleaned files
rm $dpfx/*.chk || echo cleaned files
f_count=0
for in_file in $dpfx/*.txt; do
   let f_count=f_count+1
   out_file=${in_file%.txt}.bin
   chk_file=${in_file%.txt}.chk
   bit_count="$(echo $in_file | cut -f2 -d'.')"
   ./a2b2a -a -c${bit_count} $in_file $out_file
   ./a2b2a -b -c${bit_count} $out_file $chk_file
   # in the following we do:
   # 1) replace newlines with spaces
   # 2) ignore space at beginning of line
   # 3) ignore space at end of line
   # 4) multiple spaces will be replaced by a single space
   diff <(tr '\n' ' ' < $in_file | sed 's/  */ /g;s/^ //;s/ $//')  <(tr '\n' ' ' < $chk_file | sed 's/  */ /g;s/^ //;s/ $//')
   if [ $? -ne 0 ]; then
      printf "

                **** FAILED *****

        idx: $bit_count.  Files:
            ${in_file} and
            ${chk_file}
        differs

                **** FAILED *****

      "
      exit 1
   fi
done
printf "

        Successfully tested $f_count number of files

"
exit 0
