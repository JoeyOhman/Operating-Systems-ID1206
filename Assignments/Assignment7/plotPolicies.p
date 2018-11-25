# Gnuplot script for plotting data in file "optimal.dat"
set terminal png
set output "plotPolicies.png"

set title "Page replacement using different policies"

set key right center

set xlabel "frames in memory"
set ylabel "hit ratio"

set xrange [0:100]
set yrange [0:1]

plot "optimal.dat" u 1:2 w linespoints title "optimal",\
      "random.dat" u 1:2 w linespoints title "random",\
      "lru.dat" u 1:2 w linespoints title "lru",\
      "clock.dat" u 1:2 w linespoints title "clock"
