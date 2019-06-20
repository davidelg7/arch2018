cd $(dirname "$0")

./pqnn64_opm/runpqnn64_18_opm.sh
./pqnn64/runpqnn64_18.sh
./pqnn32/runpqnn32_18.sh
clear

echo ./pqnn64_opm/runTest.sh
./pqnn64_opm/runTest.sh

echo ./pqnn64/runTest.sh
./pqnn64/runTest.sh

echo ./pqnn32/runTest.sh
./pqnn32/runTest.sh

echo ./plotta.sh


./plotta.sh
