clang++ -O3 -Wall -std=c++11 -o huffman buildHC.cpp -I $HOME/boost/include/

./huffman example.txt
