# gnuplot command file for graphic presentation of daempfer data files

# datafile is parameter #1 for -e
# cmdfile ist this file (gp.cmd)
# usage:
# gnuplot -e "datafile='test0.csv'" gp.cmd

set term x11
set datafile separator ';'

# scale factors for f (force) and d (distance)
# f0 = 397000
f0 = 33000
f1 = 1000
d0 = 500
d1 = 6.95

#set xrange[29500:31000]
plot datafile using 1:($2-f0)/f1 with lines, datafile using 1:($3-d0)/d1 with lines
set term png size 1024,768
set output 'result.png'
plot datafile using 1:($2-f0)/f1 with lines, datafile using 1:($3-d0)/d1 with lines
set output
pause 10

# set xrange[29000:31000]
# plot ns1 using ($1-x0):($2-f0)/f1 with lines, ns1 using ($1-x0):($3-d0)/d1 with lines, r6 using 1:($2-f0)/f1 with lines, r6 using 1:($3-d01)/d1 with lines
# User and default variables:
# pi = 3.14159265358979
# GNUTERM = "qt"
# NaN = NaN
# VoxelDistance = 9.00500479207635e-308
# ARGC = 0
# ARG0 = ""
# ARGV = <0 element array>
# f0 = 4000
# f1 = 1000
# d0 = 500
# d1 = 6.95
# datafile = "dammi07.csv"
# datafile1 = "dammi06.csv"
# MOUSE_KEY = 1
# MOUSE_CHAR = "\001"
# ns1 = "dammi06.csv"
# r6 = "dammi07.csv"
# d01 = 1100
# x0 = 5280
