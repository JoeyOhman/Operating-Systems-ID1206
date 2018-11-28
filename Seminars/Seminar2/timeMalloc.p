
set terminal png
set output "timeMalloc.png"

set title "Time benchmark of malloc, avg allocs/free time for each size"

set key left center

set xlabel "Block Size (Bytes)"
set ylabel "Time (ms)"

set yrange[0: 0.003]

plot  "timeMalloc.dat" u 1:2 w linespoints title "malloc",\
      "timeMalloc.dat" u 1:3 w linespoints title "free"
