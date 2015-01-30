/**
Loosely based on:
Author: Qingye Jiang
From: http://jqyhuffman.blogspot.com/
Date: Jan 17 2014
**/

#ifndef OINK_HUFF_LIB
#define OINK_HUFF_LIB

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <queue>
#include <memory>
#include <boost/dynamic_bitset.hpp>

using namespace std;

typedef std::string code_t;
typedef std::string kmer_t;
typedef std::pair<code_t, kmer_t> decodingTuple;

/**********************************************************************
//
// Node class
//
**********************************************************************/
template<class T> class Node {
public:
  Node () {};

  Node (T kmer) { content = kmer; }

  Node (double freq) {
    frequency = freq;
  }

  Node (double freq, int id) {
    frequency = freq;
    node_id = id;
  }

  Node (double freq, T s) {
    frequency = freq;
    content = s;
  }

  Node (double freq, T s, int id) {
    frequency = freq;
    content = s;
    node_id = id;
  };

  double getFreq() const {return frequency;}
  void setFreq(double f) {frequency = f;}

  T getContent() const { return content; }

  void setLeftChild(shared_ptr<Node> n) {   leftChild = n;   }
  void setRightChild(shared_ptr<Node> n) {  rightChild = n;  }

  shared_ptr<Node> getLeft() const {return leftChild;}

  shared_ptr<Node> getRight() const {return rightChild;}

  bool isLeaf() const {return leftChild==NULL && rightChild==NULL; }

  int getId() const {return node_id; }

private:  
  shared_ptr<Node> leftChild;
  shared_ptr<Node> rightChild;
  double frequency = 0;
  T content;
  // string code;
  int node_id = -1;
};

/***************************************************************************
**
**  HuffmanCode class
**
****************************************************************************/
class HuffmanCode {
  public:
    HuffmanCode (shared_ptr<vector<pair<string, int>>> freq) { frequencies = freq; }
    /**********************************************************************
    **
    ***********************************************************************/
    shared_ptr< unordered_map<string, string> > getHuffmanCode() {
        shared_ptr<Node<string>> root;
        root = buildHuffmanTree();

        shared_ptr< unordered_map<string, string> > encoding(new unordered_map<string,string>() );
        shared_ptr< vector<pair<string, string> > > encoding_vect(new vector< pair< string,string > >() );
        boost::dynamic_bitset<> bitset;  
        BFS_vect(root, bitset, encoding_vect);
        root.reset(); // start freeing this memory
        encoding->insert(encoding_vect->begin(), encoding_vect->end());
        return encoding;
    }
    /**********************************************************************
    **
    ***********************************************************************/
    shared_ptr< vector<pair<string, string> > > getHuffmanCode_vect() {
        shared_ptr<Node<string>> root;
        // cerr << "Build tree" << endl;
        root = buildHuffmanTree();
        shared_ptr< vector<pair<string, string> > > encoding_vect(new vector< pair< string,string > >() );
        // cerr << "Traverse tree" << endl;
        boost::dynamic_bitset<> bitset;
        BFS_vect(root, bitset, encoding_vect);
        return encoding_vect;
    }

  private:

    shared_ptr<vector<pair<string, int>>> frequencies;

    /**********************************************************************
    **
    ***********************************************************************/
    shared_ptr<Node<string>> buildHuffmanTree() {
        // comparator for Nodes
        struct OrderByFreq {
            bool operator() (shared_ptr<Node<string>> const &a, shared_ptr<Node<string>> const &b) const {
                return a->getFreq() > b->getFreq(); 
            }
        };
        priority_queue<shared_ptr<Node<string>>, vector<shared_ptr<Node<string>>>, OrderByFreq> PQ;

        // creating leaves
        int node_id = 0;
        for (auto pair : *frequencies) {
            // set content field for the leaves
            double score = pair.second;
            PQ.push( shared_ptr<Node<string>>(new Node<string>(score, pair.first, node_id++) ) );
            // PQ.push( shared_ptr<Node>(new Node(pair.second, pair.first, node_id++) ) );
        }

        while ( PQ.size() > 1 ) {
            shared_ptr<Node<string>> min1 = PQ.top(); PQ.pop();
            shared_ptr<Node<string>> min2 = PQ.top(); PQ.pop();
            double s1 = min1->getFreq();
            double s2 = min2->getFreq();
            shared_ptr<Node<string>> root( new Node<string>(s1 + s2, node_id++ ) ); // set frequency to be sum of child nodes' frequencies
            root->setLeftChild( min2 );
            root->setRightChild( min1 );
            PQ.push(root);
        }

        if (PQ.size() == 1) {
            shared_ptr<Node<string>> root = PQ.top(); PQ.pop();
            return root;
        }
        else {
            cerr << "[ERROR] PriorityQueue size: " << PQ.size() << endl;
            exit(0);
        }
    }

    /**********************************************************************
    **
    ***********************************************************************/
    void BFS(const shared_ptr<Node<string>> & root, boost::dynamic_bitset<> & bits, shared_ptr< unordered_map<string, string> > & encoding) {
        if (root->isLeaf() ) {
            string str;
            boost::to_string(bits, str);
            encoding->emplace(root->getContent(), "");
        }
        else {
            shared_ptr<Node<string>> left   = root->getLeft();
            shared_ptr<Node<string>> right  = root->getRight();
            boost::dynamic_bitset<> right_bits(bits);
            bits.push_back(0);
            right_bits.push_back(1);
            BFS( left, bits, encoding);
            BFS( right, right_bits, encoding);
        }
    }

    /**********************************************************************
    **
    ***********************************************************************/
    void BFS_vect(const shared_ptr<Node<string>> & root, boost::dynamic_bitset<> & s, shared_ptr< vector<pair<string, string> > > & encoding) {
        if (root == NULL) {
            cerr << "[INFO] Root is null" << endl;
            return;
        }

        if (root->isLeaf() ) {
            string str;
            boost::to_string(s, str);
            std::reverse(str.begin(), str.end());
            encoding->emplace_back(root->getContent(), str);
        }
        else {
            // descend into the left subtree
            shared_ptr<Node<string>> child = root->getLeft();
            boost::dynamic_bitset<> copy(s);
            s.push_back(0);
            BFS_vect(child, s, encoding);

            // descent into the right subtree
            copy.push_back(1);
            child  = root->getRight();
            BFS_vect( child, copy, encoding);
        }
    }
};

shared_ptr<Node<string>> buildDecodingTree(vector<decodingTuple>::iterator start, vector<decodingTuple>::iterator end, 
    shared_ptr<vector<decodingTuple> > & codes, int idx, string t) {
    if ( end - start <= 1 ) {
        // reached a leaf
        shared_ptr<Node<string>> leaf(new Node<string>(start->second)); // save the kmer this leaf codes for
        return leaf;
    }

    auto zerosEnd = start;
    for (; zerosEnd != end; zerosEnd++) {
        if (zerosEnd->first.size() <= idx) {
            zerosEnd++;
            break;
        }
        else if (zerosEnd->first.size() > idx && zerosEnd->first.at(idx) != '0') {
            break;
        }
    }
    shared_ptr<Node<string>> root(new Node<string>());
    root->setLeftChild( buildDecodingTree(start, zerosEnd, codes, idx+1, t+"\t") );
    root->setRightChild( buildDecodingTree(zerosEnd, end, codes, idx+1, t+"\t") );
    return root;
}


#endif
