cd $(dirname "$0")
for distance in -adc -sdc
do
  for nr in 2048 4000
  do
  for kc in 256  2048
   do
  for k in 256 512
  do
    for m in 8 16
    do
      for knn in 1 2 4
      do
        echo -noexaustive -s 1 $distance -nr $nr -kc $kc -k $k -m $m -knn $knn
        (time ./pqnn64_18_opm ./ds/prova -noexaustive -s 1 $distance -nr $nr -kc $kc -k $k -m $m -knn $knn) &> test/"-noexaustive $distance -nr $nr -kc $kc -k $k -m $m -knn $knn.txt"
      done
    done
  done
  done
  done
  for k in 256 512
  do
    for m in 8 16
    do
      for knn in 1 2 4
      do
        echo -exaustive -k $k -knn $knn -m $m $distance
      (time ./pqnn64_18_opm ./ds/prova -s 1 -adc -exaustive $distance -k $k -m $m -knn $knn) &> test/"-exaustive $distance -k $k -m $m -knn $knn.txt"
      done
    done
  done
done
