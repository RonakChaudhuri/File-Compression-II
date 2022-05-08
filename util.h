//
// STARTER CODE: util.h
//
// Project: File Compression II
// Name: Ronak Chaudhuri
// Project Overview: Compress files and encode/decode them using Huffman encoding

#include <iostream>
#include <fstream>
#include <map>
#include <queue>          // std::priority_queue
#include <utility>
#include <vector>         // std::vector
#include <functional>     // std::greater
#include <string>
#include "bitstream.h"
#include "hashmap.h"
#include "mymap.h"
#pragma once

struct HuffmanNode {
    int character;
    int count;
    HuffmanNode* zero;
    HuffmanNode* one;
};

// Prioritize class for huffman node priority queue
class prioritize {
 public:
    bool operator()(const HuffmanNode* p1, const HuffmanNode* p2) const {
        return p1->count > p2->count;
    }
};

// Recursive helper function using postorder traversal to free all the nodes
void _clearRecursion(HuffmanNode* node) {
    if (node == nullptr) {
        return;
    }
    _clearRecursion(node->zero);
    _clearRecursion(node->one);
    delete node;
}
//
// *This method frees the memory allocated for the Huffman tree.
//
void freeTree(HuffmanNode* node) {
    _clearRecursion(node);
}

//
// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
// Loops through file and adds/updates frequency value for each character put in map
//
void buildFrequencyMap(string filename, bool isFile, hashmap &map) {
    if (isFile) {
        ifstream inFS(filename);
        char c;
        while (inFS.get(c)) {
            if (map.containsKey(c)) {
                map.put(c, map.get(c) + 1);
            } else {
                map.put(c, 1);
            }
        }
    } else {
        for (char c : filename) {
            if (map.containsKey(c)) {
                map.put(c, map.get(c) + 1);
            } else {
                map.put(c, 1);
            }
        }
    }
    map.put(PSEUDO_EOF, 1);
}
//
// *This function builds an encoding tree from the frequency map.
// Creates a node for every character, then goes through the priority queue to create a tree from the characters
//
HuffmanNode* buildEncodingTree(hashmap &map) {
    priority_queue<pair<int, int>, vector<HuffmanNode*>, prioritize>  pq;
    vector<int> vec = map.keys();
    for (int key : vec) {
        HuffmanNode* newNode = new HuffmanNode();
        newNode->zero = nullptr;
        newNode->one = nullptr;
        newNode->character = key;
        newNode->count = map.get(key);
        pq.push(newNode);
    }
    while (pq.size() > 1) {
        HuffmanNode* first = pq.top();
        pq.pop();
        HuffmanNode* second = pq.top();
        pq.pop();
        HuffmanNode* node = new HuffmanNode();
        node->character = NOT_A_CHAR;
        node->count = first->count + second->count;
        node->zero = first;
        node->one = second;
        pq.push(node);
    }
    return pq.top();
}

// Recursive helper function using preorder recursion to adds each character's string traversal through tree value
void _treeRecursion(HuffmanNode* node, string str, mymap <int, string> &map) {
    if (node->character != NOT_A_CHAR) {
        map.put(node->character, str);
        return;
    }
    _treeRecursion(node->zero, str+"0", map);
    _treeRecursion(node->one, str+"1", map);
}
//
// *This function builds the encoding map from an encoding tree.
//
mymap <int, string> buildEncodingMap(HuffmanNode* tree) {
    mymap <int, string> encodingMap;
    string str;
    _treeRecursion(tree, str, encodingMap);
    return encodingMap;
}

//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input, mymap <int, string> &encodingMap,
              ofbitstream& output, int &size, bool makeFile) {
    string str;
    char c;
    while (input.get(c)) {
        str += encodingMap.get(c);
    }
    str += encodingMap.get(PSEUDO_EOF);
    if (makeFile) {
        for (char o : str) {
            if (o == '0') {
                output.writeBit(0);
            }
            if (o == '1') {
                output.writeBit(1);
            }
        }
    }
    size = str.size();
    return str;
}
//
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
    HuffmanNode* curr = encodingTree;
    string str;
    while (!input.eof()) {
        int bit = input.readBit();
        if (bit == 0) {
            curr = curr->zero;
        }
        if (bit == 1) {
            curr = curr->one;
        }
        if (curr->character == PSEUDO_EOF) break;
        if (curr->character != NOT_A_CHAR) {
            output.put(curr->character);
            str += curr->character;
            curr = encodingTree;
        }
    }
    return str;
}
//
// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) {
    hashmap map;
    buildFrequencyMap(filename, true, map);
    HuffmanNode* tree;
    tree = buildEncodingTree(map);
    mymap<int, string> encodingMap;
    encodingMap = buildEncodingMap(tree);
    string str;
    int size = 0;
    ofbitstream output(filename + ".huf");
    output << map;
    ifstream input(filename);
    str = encode(input, encodingMap, output, size, true);
    freeTree(tree);
    return str;
}
//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename) {
    ifbitstream input(filename);
    string delimiter = ".txt";
    string delimiter2 = ".huf";
    string s = filename.substr(0, filename.find(delimiter));
    filename = filename.substr(0, filename.find(delimiter2));
    ofstream output(s + "_unc.txt");
    hashmap map;
    input >> map;
    buildFrequencyMap(filename, true, map);
    HuffmanNode* tree;
    tree = buildEncodingTree(map);
    string str;
    str = decode(input, tree, output);
    freeTree(tree);
    return str;
}