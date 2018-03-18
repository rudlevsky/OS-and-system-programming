#!/bin/bash

ex=$1
link=$2
folder=$3

need_count=4
all_count=$#

function check_count
{
	perem=$(test -d "$link")
	if [ $all_count -lt $need_count ] 
	then
		echo "missing argument"
		exit 1
	elif [ $perem ]
	then 
		echo "adress is wrong"
		exit 1
	fi
}

check_count

exec 2>$4
find "$link" -type f -name "*.$ex" -exec cp -lt "$folder" {} +


