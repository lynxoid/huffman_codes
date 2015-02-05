# huffman_codes
Implementation of classic Huffman coding scheme in C++11

##Dependencies
- Boost 1.5.4 or later

##To compile

Here is how I compiled the code on MacOS. To compile on your system, substitute boost's location to be the dir where boost is installed and make sure you use your favorite compiler :)

> clang++ -O3 -Wall -std=c++11 -o huffman buildHC.cpp -I $HOME/boost/include/

##To run

> ./huffman example.txt > codes.txt

`codes.txt` will contain something like this:
```
CCG	111
TCG	110
ACG	01
AGT	10
AAA	00
```
