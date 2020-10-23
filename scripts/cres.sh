#!/bin/sh

help() {
	echo "Recursively convert resource files into embeddable C headers"
	echo ""
	echo "USAGE"
	echo "  $ cres <src> <dest>"
}

if [ -z "$1" ] || [ -z "$2" ]
then
	help
	exit 1
fi

if [ ! -d "$1" ]
then
	echo "$1 not found"
	exit 1
fi

base_path=$(pwd)
src_path="$1"
dest_path="$2"

mkdir -p "$dest_path"

convert() {
	for entry in "$1"/*
	do
		dest=$dest_path${entry##$src_path}
		if [ -d "$entry" ]
		then
			mkdir -p "$dest"
			convert "$entry"
		else
			echo "$entry -> $dest.h"
			cd "$1" || exit
			xxd -i "${entry##$1/}" > "$base_path/$dest.h"
			cd "$base_path" || exit
		fi
	done
}

convert "$src_path"

