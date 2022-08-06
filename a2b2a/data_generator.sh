#!/bin/bash -e -x

dest_dir="$1"
dname=$(dirname $0)
if [ -z "$dest_dir" ]; then
    printf "Destination directory missing\n"
	exit 1
fi
fcount=0
for nbits in $(seq 1 31); do
	p_inc=$(echo $RANDOM | cut -c1)
	p_end=$(echo $RANDOM | cut -c1,2,3)
	let p_end=p_end+2048 # Add 2k numbers
	let p_inc=p_inc+31 
	for sz in $(seq 4 $p_inc $p_end); do
	  echo $nbits $sz
	  $dname/data_generator.py $nbits $sz > ${dest_dir}/in.$nbits.$sz.txt
	  let fcount=fcount+1
        done

	# cover all sizes
	let p_begin=1024
	let p_end=1024+512
	for sz in $(seq $p_begin 1 $p_end); do
	  echo $nbits $sz
	  $dname/data_generator.py $nbits $sz > ${dest_dir}/in.$nbits.$sz.txt
	  let fcount=fcount+1
        done
done
echo "Created $fcount number of files\n"
