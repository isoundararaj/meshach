#!/bin/bash -e

for in_file in $* ; do
  filename=$(basename -- "$in_file")
  dirname=$(dirname -- "$in_file")
  extension="${filename##*.}"
  mainname="${filename%.*}"

  bit_num=$(echo $mainname | sed 's/val//')
  data_bin="$dirname/../data-bin/"
  data_chk="$dirname/../data-chk/"
  ./a2b2a -a -c$bit_num $in_file $data_bin/$mainname.bin
  ./a2b2a -b -c$bit_num $data_bin/$mainname.bin $data_chk/$mainname.chk
  cmp $in_file $data_chk/$mainname.chk
  if [ $? -eq 0 ]; then
	  printf "  ===> PASS: $in_file\n"
  else
	  printf  " FAILED for $in_file\n"
	  exit 1
  fi
  #filename=$(basename -- "$fullfile")
  #extension="${filename##*.}"
  #filename="${filename%.*}"
done
