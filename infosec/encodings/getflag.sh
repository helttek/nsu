#!/bin/bash
x() {
  n="$1"
  for i in {0..150}
    do
      if (( $n % 2 ))
      then
         n=$(( $n * 3 + 1 ))
      else
	     n=$(( $n / 2 ))
      fi
    done
  while !(( $n % 2 ))
  do
    n=$(( $n / 2 ))
  done
  n=$(($n * 10))
  t=$(($1 + n))
  printf "\\$(printf '%03o' "$t")"
}

x 68
x 73
x 75
x 113
x 57
x 72
x 66
x 60
x 95
x 105
x 109
x 91
x 95
x 104
x 90
x 115
echo
