
set terminal png
set output "time.png"

set title "Time benchmark of buddy algorithm, avg allocs/free time for each size"

set key right center

set xlabel "Block Size (Bytes)"
set ylabel "Time (ms)"

plot  "time.dat" u 1:2 w linespoints title "Balloc",\
      "time.dat" u 1:3 w linespoints title "Bfree"
