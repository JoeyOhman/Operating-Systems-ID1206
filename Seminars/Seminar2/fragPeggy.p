
set terminal png
set output "fragPeggy.png"

set title "Fragmentation benchmark of buddy algorithm with peggysue, 100k allocs each size"

set key left center

set xlabel "#BlockBufferSize"
set ylabel "#KB"

plot  "fragPeggy.dat" u 1:2 w linespoints title "OSAllocated",\
      "fragPeggy.dat" u 1:3 w linespoints title "UserGiven",\
      "fragPeggy.dat" u 1:4 w linespoints title "UserUsed"
