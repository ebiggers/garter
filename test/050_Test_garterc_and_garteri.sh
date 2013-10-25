#!/bin/bash

set -e -u

for src in test/garterc_and_garteri_Tests/*.ga; do
	echo "Testing ${src}"
	base=${src%.*}
	./garterc ${src} -o ${base}.exe
	${base}.exe > ${base}.out
	cmp ${base}.out ${base}.expected_out
	./garteri ${src} > ${base}.out
	cmp ${base}.out ${base}.expected_out
done
rm test/garterc_and_garteri_Tests/*.{exe,out,o}

cat << EOF
==========================================================
  ${0##*/}:  All tests passed!
==========================================================
EOF
