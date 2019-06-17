cd $(dirname "$0")
clear
for distance in -adc -sdc
do
  for k in 256 512
  do
    for m in 8 16
    do
      for knn in 1 2 4
      do
        echo -exaustive -k $k -knn $knn -m $m $distance
        ./compiled/pqnn64 ./ds/prova -s 1 -adc -eps 1 -exaustive $distance -k $k -m $m -knn $knn | tee test/"-exaustive $distance -k $k -m $m -knn $knn.txt" &
      done
      wait
    done
  done
done
