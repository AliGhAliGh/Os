#!/bin/bash
cd serial
make 1>/dev/null
cd ../parallel
make 1>/dev/null
cd ..
out1=$(./serial/ImageFilters.out input.bmp 2)
out2=$(./parallel/ImageFilters.out input.bmp 2)
echo "scale=4; $out1 / $out2" | bc