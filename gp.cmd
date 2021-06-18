# gnuplot command file for graphic presentation of daempfer data files

# datafile is parameter #1 for -e
# cmdfile ist this file (gp.cmd)
# usage:
# gnuplot -e "datafile='test0.csv'" gp.cmd

set term x11
set datafile separator ';'

# scale factors for f (force) and d (distance)
f0 = 397000
f1 = 20
d0 = 1000
d1 = 1

plot datafile using 1:($2-f0)/f1 with lines, datafile using 1:($3-d0)/d1 with lines
set term png size 1024,768
set output 'result.png'
plot datafile using 1:($2-f0)/f1 with lines, datafile using 1:($3-d0)/d1 with lines
set output
pause 10
