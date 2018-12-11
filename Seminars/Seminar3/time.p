set terminal png
set output "time2.png"

set title "Time benchmark of creating threads and performing work with different libs \n2 threads"

set key left center

set xlabel "#Productions and Consumes (workload)"
set ylabel "Time (ms)"

plot  "time2.dat" u 1:2 w linespoints title "Green",\
      "time2.dat" u 1:3 w linespoints title "Pthread"
