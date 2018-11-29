
set terminal png
set output "fragSmall.png"

set title "Fragmentation benchmark of buddy algorithm"

set key left center

set xlabel "#BlockBufferSize"
set ylabel "#KB"

set yrange[0: 100]

plot  "fragSmall.dat" u 1:2 w linespoints title "OSAllocated",\
      "fragSmall.dat" u 1:3 w linespoints title "UserGiven",\
      "fragSmall.dat" u 1:4 w linespoints title "UserUsed"
