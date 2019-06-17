cd $(dirname "$0")
clear

for distance in -adc -sdc
do
  for nr in 2048
  do
  for kc in 256
   do
  for k in 256
  do
    for m in 4
    do
      for knn in 2
      do
        echo -noexaustive -s 1 $distance -nr $nr -kc $kc -k $k -m $m -knn $knn
        ./compiled/pqnn32 ./ds/prova -noexaustive -s 1 $distance -nr $nr -kc $kc -k $k -m $m -knn $knn  | tee test/"-noexaustive $distance -nr $nr -kc $kc -k $k -m $m -knn $knn.txt" &
      done
      wait
    done
  done
  done
  done
  for k in 256
  do
    for m in 4
    do
      for knn in 2
      do
        echo -exaustive -k $k -knn $knn -m $m $distance
        ./compiled/pqnn32 ./ds/prova -s 1 -exaustive $distance -k $k -m $m -knn $knn | tee test/"-exaustive $distance -k $k -m $m -knn $knn.txt" &
      done
      wait
    done

  done
done
python precision.py
python plots.py
