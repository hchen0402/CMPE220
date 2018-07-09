set terminal png size 1200,1200 enhanced font "default,10"
set output 'graph_float_data.png'
set palette rgbformulae 33,13,10
set pm3d
set zlabel 'throughput'
set xlabel 'stride (x8 bytes)'
set ylabel 'size (bytes)'
splot 'graph_float_data.csv' matrix with lines
exit
