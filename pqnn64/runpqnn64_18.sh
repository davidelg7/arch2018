cd $(dirname "$0")

for f in *.nasm; do
  filename=$(basename -- "$f")
  extension="${filename##*.}"
  filename="${filename%.*}"
  nasm -felf64 -g -Fdwarf "$f" -o "$filename".o
done

gcc -c  $(ls *.c)  -fopenmp


gcc -Wall -o pqnn64c_18 $(ls *.o) -lm

./pqnn64c_18 $1 $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19} ${20} ${21} ${22} ${23} ${24} ${25} ${26} ${27} ${28} ${29} ${30}
