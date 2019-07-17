set terminal pdf truecolor size 200,150 enhanced font "Serif,1000"
set grid lw 100 back
set key above horizontal enhanced font "Serif,800"
set border lw 150 back

set xlabel 'FCTs(ms)'
set ylabel 'CDF'

set output 'cdf_vl2.pdf'

set logscale x

#RANGES
set yr[0:1]

#YLABEL
set ylabel offset 2
set ytics 0,0.2,1


#XLABEL
set xlabel offset 0,0.5
set xtics 0.1,100,10000


#LINESTYLES
set style line 1 lt 2 lw 200 pt 8 ps 35 pi 15000 lc rgb "#cc0000"
set style line 2 lt 2 lw 200 pt 7 ps 35 pi 15000 lc rgb "#3366ff"
set style line 3 lt 2 lw 200 pt 4 ps 35 pi 15000 lc rgb "#000000"
set style line 4 lt 2 lw 200 pt 10 ps 35 pi 15000 lc rgb "#404040"



cdf=4

plot 'output' u 1:cdf title '2D' w linespoints ls 1, \
'output' u 2:cdf title 'FIFO' w linespoints ls 2, \
'output' u 3:cdf title 'PS' w linespoints ls 3