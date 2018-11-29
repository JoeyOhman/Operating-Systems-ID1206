
set terminal png
set output "fragPeggySmall.png"

set title "Fragmentation benchmark of buddy algorithm with peggysue small"

set key left center

set xlabel "#BlockBufferSize"
set ylabel "#KB"

set yrange[0: 100]

plot  "fragPeggySmall.dat" u 1:2 w linespoints title "OSAllocated",\
      "fragPeggySmall.dat" u 1:3 w linespoints title "UserGiven",\
      "fragPeggySmall.dat" u 1:4 w linespoints title "UserUsed"
