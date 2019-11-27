#!/bin/sh

if [ $# -ne 2 ]; then
    echo Usage: $0 input_file output_file >&2
    exit 1
fi

(cat $1 && cat /dev/zero | head -c 1) \
	| xxd --include /dev/stdin \
	| sed "s/_dev_stdin/$(basename $1 | cut -d. -f1)_shader_src/g" \
	> $2
