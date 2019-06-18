cd $(dirname "$0")
clear
python pqnn64_opm/precision.py 
python pqnn64_opm/plots.py 
python pqnn64/precision.py 
python pqnn64/plots.py 
python pqnn32/precision.py
python pqnn32/plots.py 
