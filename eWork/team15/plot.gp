reset
set xlabel 'element'
set ylabel 'time'
set title 'perfomance comparison'
set term png enhanced font 'Verdana,10'
set output 'result.png'

plot 'original.txt', 'optimize1.txt', 'optimize2.txt', 'optimize3.txt'

