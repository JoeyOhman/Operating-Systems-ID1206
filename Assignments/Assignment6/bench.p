
set terminal png
set output "bench.png"

set title "TLB benchmark, 4KiByte pages, 10 Gi operations"

set key right center

set xlabel "#pages"
set ylabel "#seconds"

set logscale x 2

plot "bench.dat" u 1:2 w linespoints title "page refs"
