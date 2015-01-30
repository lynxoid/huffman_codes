#include <fstream>
#include "HuffmanCode.hpp"

int main(int argc, char * argv []) {
	string fname = argv[1];
	ifstream f_in(fname);
	if (!f_in) {
		cerr << "[ERRRO] File not found: " << fname << endl;
		exit(1);
	}
	string line, eol("\t\b");
	shared_ptr<vector<pair<string,int>>> frequencies(new vector<pair<string,int>>());
	while (getline(f_in, line)) {
		auto pos = line.find_first_of(eol);
		if (pos == string::npos) {
			cerr << "[ERROR] Unexpected file format (expected tab separated <string> <frequency> pairs per line)." << endl;
			exit(1);
		}
		string symbol = line.substr(0, pos);
		pos = line.find_first_not_of(eol, pos);
		auto freq = stoi(line.substr(pos));
		frequencies->push_back( make_pair(symbol, freq));
	}
	HuffmanCode hc(frequencies);
	shared_ptr< unordered_map<string, string> > codes = hc.getHuffmanCode();
	for (auto pair : *codes) {
		cout << pair.first << "\t" << pair.second << endl;
	}
}