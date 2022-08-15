#!/bin/bash

# Use this script to remove the lines producing "error: ‘PlaceholderText’ is not a member of ‘QPalette’". Run ./rm_error_QPalette [line numbers separated by spaces] . Example: "./rm_error_QPalette 1001 1003 1002"

set -e

if [ $# -eq 0 ]
	then
		echo "error: no arguments supplied"
		exit 1
fi

# used to check that all args are integer
re='^[0-9]+$'

# quit if we get a non-integer argument
for var in "$@"; do
	if ! [[ "$var" =~ $re ]]
	then
		echo "error: argument $var NaN"
		exit 2
	fi
done

# sort the arguments in reverse order, so we can remove the last lines first
sorted_nums=$(for var in "$@"; do
	echo "$var"
done | sort -nr)

# remove the lines
echo "$sorted_nums" | while read line; do
	echo "Removing line $line"
	sed -i "${line}d" ./ui_mainwindow.h
done
