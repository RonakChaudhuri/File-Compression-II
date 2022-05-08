// mymap.h
//
// Project: mymap
// Name: Ronak Chaudhuri
// Project Overview: This project is for building the mymap functionality for a binary search tree. Implements the
// functions of the mymap self-balancing and threaded binary search tree.
#pragma once

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

template<typename keyType, typename valueType>
class mymap {
private:
    struct NODE {
        keyType key;  // used to build BST
        valueType value;  // stored data for the map
        NODE* left;  // links to left child
        NODE* right;  // links to right child
        int nL;  // number of nodes in left subtree
        int nR;  // number of nodes in right subtree
        bool isThreaded;
    };
    NODE* root;  // pointer to root node of the BST
    int size;  // # of key/value pairs in the mymap

    // Recursive helper function using inorder traversal and outputs key and value of each node in order
    void _stringRecursion(NODE* node, ostream& out) {
        if(node == nullptr) {
            return;
        }
        _stringRecursion(node->left, out);
        out << "key: " << node->key << " value: " << node->value << endl;
        if(node->isThreaded) {
            _stringRecursion(nullptr, out);
        } else {
            _stringRecursion(node->right, out);
        }
    }

    // Recursive helper function using postorder traversal to clear every node
    void _clearRecusion(NODE* node) {
        if(node == nullptr) {
            return;
        }
        _clearRecusion(node->left);
        if(node->isThreaded) {
            _clearRecusion(nullptr);
        } else {
            _clearRecusion(node->right);
        }
        delete node;
    }

    // Recursive helper function using preorder traversal to copy every node and put in tree
    void _copyRecursion(NODE* otherNode) {
        if(otherNode == nullptr) {
            return;
        }
        (*this).put(otherNode->key, otherNode->value);
        _copyRecursion(otherNode->left);
        if(otherNode->isThreaded) {
            _copyRecursion(nullptr);
        } else {
            _copyRecursion(otherNode->right);
        }
    }

    // Recursive helper function using in order traversal to add every key value pair to a vector parameter
    void _vecRecursion(NODE* node, vector<pair<keyType, valueType>> &vec) {
        if(node == nullptr) {
            return;
        }
        _vecRecursion(node->left, vec);
        vec.push_back({node->key, node->value});
        if(node->isThreaded) {
            _vecRecursion(nullptr, vec);
        } else {
            _vecRecursion(node->right, vec);
        }
    }

    // Helper function to check if balanced
    bool checkSeesaw(int nR, int nL) {
        if(max(nR, nL) <= 2*min(nR, nL) + 1) {
            return true;
        }
        return false;
    }

    //
    // iterator:
    // This iterator is used so that mymap will work with a foreach loop.
    //
    struct iterator {
    private:
        NODE* curr;  // points to current in-order node for begin/end

    public:
        iterator(NODE* node) {
            curr = node;
        }

        keyType operator *() {
            return curr -> key;
        }

        bool operator ==(const iterator& rhs) {
            return curr == rhs.curr;
        }

        bool operator !=(const iterator& rhs) {
            return curr != rhs.curr;
        }

        bool isDefault() {
            return !curr;
        }

        //
        // operator++:
        //
        // This function should advance curr to the next in-order node.
        // O(logN)
        // Advances node to right, and if node is not threaded, will advance curr to leftmost node
        iterator operator++() {
            if(curr->isThreaded) {
                curr = curr->right;
            } else {
                curr = curr->right;
                while(curr->left != nullptr) {
                    curr = curr->left;
                }
            }
            return iterator(curr);
        }

    };

public:
    //
    // default constructor:
    //
    // Creates an empty mymap.
    // Time complexity: O(1)
    //
    mymap() {
        root = nullptr;
        size = 0;
    }

    //
    // copy constructor:
    //
    // Constructs a new mymap which is a copy of the "other" mymap.
    // Sets all member variables appropriately.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    mymap(const mymap& other) {
           NODE* otherCurr = other.root;
           size = 0;
           root = nullptr;
           _copyRecursion(otherCurr);
    }

    //
    // operator=:
    //
    // Clears "this" mymap and then makes a copy of the "other" mymap.
    // Sets all member variables appropriately.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    mymap& operator=(const mymap& other) {
        NODE* otherCurr = other.root;
        (*this).clear();
        _copyRecursion(otherCurr);
        return *this;
    }


    // clear:
    //
    // Frees the memory associated with the mymap; can be used for testing.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    void clear() {
        size = 0;
        NODE* curr = root;
        _clearRecusion(curr);
        root = nullptr;
    }

    //
    // destructor:
    //
    // Frees the memory associated with the mymap.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    ~mymap() {
        NODE* curr = root;
        _clearRecusion(curr);
    }



    //
    // put:
    //
    // Inserts the key/value into the threaded, self-balancing BST based on
    // the key.
    // Time complexity: O(logn + mlogm), where n is total number of nodes in the
    // threaded, self-balancing BST and m is the number of nodes in the
    // sub-tree that needs to be re-balanced.
    // Space complexity: O(1)
    //
    void put(keyType key, valueType value) {
        NODE* prev = nullptr;
        NODE* curr = root;
        NODE* violate = nullptr;
        NODE* violateParent = nullptr;
        bool violated = false;
        //Searches for key, updates value if found
        while(curr != nullptr) {
            if(key == curr->key) {
                curr->value = value;
                return;
            }
            if(key < curr->key) {
                curr->nL++;
                if(!checkSeesaw(curr->nR, curr->nL) && !violated) {
                    violate = curr;
                    violateParent = prev;
                    violated = true;
                }
                prev = curr;
                curr = curr->left;
            } else {
                curr->nR++;
                if(!checkSeesaw(curr->nR, curr->nL) && !violated) {
                    violate = curr;
                    violateParent = prev;
                    violated = true;
                }
                prev = curr;
                curr = (curr->isThreaded) ? nullptr : curr->right;
            }
        }
        //Inserts new node at right spot
        NODE* n = new NODE();
        n->key = key;
        n->value = value;
        n->left = nullptr;
        n->right = nullptr;

        if(prev == nullptr) {
            root = n;
        } else if(key < prev->key) {
            prev->left = n;
            n->right = prev;
            n->isThreaded = true;
        } else {
            n->right = prev->right;
            prev->right = n;
            prev->isThreaded = false;
            n->isThreaded = true;
        }
        size++;
    }

    //
    // contains:
    // Returns true if the key is in mymap, return false if not.
    // Time complexity: O(logn), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    bool contains(keyType key) {
        NODE *curr = root;
        while(curr != nullptr) {
            if(key == curr->key) {
                return true;
            }
            if(key < curr->key) {
                curr = curr->left;
            } else {
                curr = (curr->isThreaded) ? nullptr : curr->right;
            }
        }
        return false;
    }

    //
    // get:
    //
    // Returns the value for the given key; if the key is not found, the
    // default value, valueType(), is returned (but not added to mymap).
    // Time complexity: O(logn), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    valueType get(keyType key) {
        NODE *curr = root;
        while(curr != nullptr) {
            if(key == curr->key) {
                return curr->value;
            }
            if(key < curr->key) {
                curr = curr->left;
            } else {
                curr = (curr->isThreaded) ? nullptr : curr->right;
            }
        }
        return valueType();
    }

    //
    // operator[]:
    //
    // Returns the value for the given key; if the key is not found,
    // the default value, valueType(), is returned (and the resulting new
    // key/value pair is inserted into the map).
    // Time complexity: O(logn + mlogm), where n is total number of nodes in the
    // threaded, self-balancing BST and m is the number of nodes in the
    // sub-trees that need to be re-balanced.
    // Space complexity: O(1)
    //
    valueType operator[](keyType key) {
        NODE *curr = root;
        while(curr != nullptr) {
            if(key == curr->key) {
                return curr->value;
            }
            if(key < curr->key) {
                curr = curr->left;
            } else {
                curr = (curr->isThreaded) ? nullptr : curr->right;
            }
        }
        put(key, valueType());
        return valueType();
    }

    //
    // Size:
    //
    // Returns the # of key/value pairs in the mymap, 0 if empty.
    // O(1)
    //
    int Size() {
        return size;
    }

    //
    // begin:
    //
    // returns an iterator to the first in order NODE.
    // Time complexity: O(logn), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    iterator begin() {
        NODE* curr = root;
        while(curr->left != nullptr) {
            curr = curr->left;
        }
        return iterator(curr);
    }

    //
    // end:
    //
    // returns an iterator to the last in order NODE.
    // this function is given to you.
    //
    // Time Complexity: O(1)
    //
    iterator end() {
        return iterator(nullptr);
    }


    //
    // toString:
    //
    // Returns a string of the entire mymap, in order.
    // Format for 8/80, 15/150, 20/200:
    // "key: 8 value: 80\nkey: 15 value: 150\nkey: 20 value: 200\n
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    string toString() {
        stringstream ss;
        _stringRecursion(root, ss);
        return ss.str();
    }

    //
    // toVector:
    //
    // Returns a vector of the entire map, in order.  For 8/80, 15/150, 20/200:
    // {{8, 80}, {15, 150}, {20, 200}}
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    vector<pair<keyType, valueType> > toVector() {
        vector<pair<keyType, valueType>> vec;
        _vecRecursion(root, vec);
        return vec;
    }

    //
    // checkBalance:
    //
    // Returns a string of mymap that verifies that the tree is properly
    // balanced.  For example, if keys: 1, 2, 3 are inserted in that order,
    // function should return a string in this format (in pre-order):
    // "key: 2, nL: 1, nR: 1\nkey: 1, nL: 0, nR: 0\nkey: 3, nL: 0, nR: 0\n";
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    string checkBalance() {


        // TODO: write this function.


        return {};  // TODO: Update this return.
    }
};