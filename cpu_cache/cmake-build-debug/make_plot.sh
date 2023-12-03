#! /usr/bin/gnuplot -persist
set terminal png size 1224, 968
set output "result.png"
set grid xtics ytics
#set terminal postscript eps enhanced color solid
#set output "result.ps"

set title "cpu cache"
set xlabel "array size" font "Helvetica Bold	,18"
set ylabel "ticks" font "Helvetica Bold	,18"

set xrange [0:500000]
set yrange [0:100]
plot "coordinates.csv" using 1:2 with lines title "Direct", '' using 1:3 with lines title "Reverse", '' using 1:4 with lines title "Random"
