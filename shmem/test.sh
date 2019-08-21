#!/bin/bash
for i in `seq 0 7`
do
    {
	if [ $i != 8 ]
	then
	    ./shmem_test $i
	else
	    echo "start"
	fi
    }&
done
wait
