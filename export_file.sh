#!/bin/sh

if [ $# -ne 2 ]; then
    echo Usage: $0 input_file output_file >&2
    exit 1
fi

cat $1 \
	| xxd --include /dev/stdin \
	| sed "s/_dev_stdin/$(basename $1 | tr . _)_buf/g" \
	> $2
