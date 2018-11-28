
set terminal png
set output "frag.png"

set title "Fragmentation benchmark of buddy algorithm"

set key left center

set xlabel "#BlockBufferSize"
set ylabel "#KB"

plot  "frag.dat" u 1:2 w linespoints title "OSAllocated",\
      "frag.dat" u 1:3 w linespoints title "UserGiven",\
      "frag.dat" u 1:4 w linespoints title "UserUsed"
