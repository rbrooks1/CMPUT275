/*
Name: Ryan Brooks, Charlie Chen
ID: 1530605, 1530488
Course: CMPUT 275 winter 2019
assignment: Final project: basic search engine
*/
#include<cstdlib>
#include<unistd.h>
#include<fstream>
#include<string>
#include<iostream>
#include<stdio.h>
#include<vector>
#include<utility>
#include<bits/stdc++.h>
#include<set>
#include<unordered_map>
#include<sstream>
#include<algorithm>
#include<cctype>
#include<cmath>
#include"heap.h"

using namespace std;

// global variable and constant declaration
#define libSize 18

unordered_map<string, vector<string> > totalWords;
unordered_map<string, double> fileWeights;

// the code below was to be used for our own hashtable implementation.
/*
// Takes a string as input and hashes it as a long long for
// use in the planned hashtable.
long long compute_hash(string const& s) {
    const int p = 31;
    const int m = 1e9 + 9;
    long long hash_value = 0;
    long long p_pow = 1;
    for (char c : s) {
        hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }
    return hash_value;
    }

// a struct for storing a word and a vector of document names
struct entry {
    string key;
    vector<string> doclist;
};

// the wrapper object for inserting and retrieving from
// the hashtable.
struct strWrapper {
    entry e;
    long long hash() const {
       return compute_hash(e.key);
    }
    // still need this for the HashTable
    bool operator!=(const strWrapper& rhs) const {
        return e.key != rhs.e.key;
    }
 };
*/

// converts string type to const char* type
const char *conversion(string line) {
    return line.c_str();;
}

// function that checks if the .pdf file has been put into the map
// and if it has then it updates the weight value, otherwise it
// adds the word to the map with the first weighht value
void updateWeights(string text, double weight) {
    int val = text.find('.', 0);
    string imVal = text.substr(0, val);
    text = imVal + ".pdf";
    if (fileWeights.find(text) != fileWeights.end()) {
        fileWeights.find(text)->second += weight;
    } else {
        fileWeights.insert({text, weight});
    }
}

// checks if the word is in the global totalWords hashtable and
// adds it as a key with the document it appears in as a vector element
// in the item if the word is not present and if it alreeady exists, then
// the function updates the vector in the item with the current document.
void updateMap(string word, string text) {
    if (totalWords.find(word) != totalWords.end()) {
        totalWords.find(word)->second.push_back(text);
    } else {
        vector<string> doclist;
        doclist.push_back(text);
        totalWords.insert({word, doclist});
    }
}

// creates the database
void hashCreate(string text) {
    ifstream file;
    string word;
    file.open(conversion(text));
    set<string> u;
    while (file >> word) {
        // preprocess the word so no punctuation exists
        for (int i = 0, len = word.size(); i < len; i++) {
            if (ispunct(word[i])) {
                word.erase(i--, 1);
                len = word.size();
            }
        }
        // check if the word has already been entered and
        // add it to the database if it hasn't
        if (!word.empty() && u.find(word) == u.end()) {
            u.insert(word);
            updateMap(word, text);
        }
    }
    file.close();
}

// find the total word count of a file and the count of the unique
// word entered as an argument
pair<double, double> readLine(string text, string searchWord) {
    ifstream file;
    string word;
    file.open(conversion(text));
    int count = 0;
    int wordCount = 0;
    // total word count
    while (file >> word) {
        if (!word.empty()) {
            // preprocessing of each word to make sure no
            // punctuation exists in each word
            for (int i = 0, len = word.size(); i < len; i++) {
                if (ispunct(word[i])) {
                    word.erase(i--, 1);
                    len = word.size();
                }
            }
            // increment the wordcount if the unique word
            // was found
            if (word == searchWord) {
                wordCount++;
            }
            count++;
        }
    }

    file.close();
    remove(conversion(text));
    // delete the text file to save space
    return {count, wordCount};
}

// determines the weight for each word in a file which can then be
// used to determine the most relevant files to return for the user
double tfidf(string text, string word, int size) {
    pair<double, double> wordCount = readLine(text, word);
    double tf = wordCount.second / (wordCount.first / 1000);
    double idf = log10(libSize / size);
    return (tf * idf);
}

// search function that takes in a search string as input and
// outputs the most relevant results based on the search words
void search() {
    cout << "Welcome.  Please make a search" << endl;
    vector<string> words;
    string value;
    string intVal;
    // make sure an empty string was not read
    while (true) {
        getline(cin, value);
        if (!value.empty()) {
            break;
        }
    }
    // take each word from the getline function and
    // store them seperately in a vector
    istringstream iss(value);
    BinaryHeap<string, double> maxHeap;
    while (iss >> intVal) {
        words.push_back(intVal);
    }
    // calculate the fileweights for each file
    for (int i = 0; i < words.size(); i++) {
        if (totalWords.find(words[i]) != totalWords.end()) {
            for (int j = 0; j < totalWords.find(words[i])->second.size(); j++) {
                double val = 0;
                val = tfidf(totalWords.find(words[i])->second[j], words[i], totalWords.find(words[i])->second.size());
                updateWeights(totalWords.find(words[i])->second[j], val);
            }
        }
    }
    // insert the values from the fileWeights map into a heap
    // making sure the heap size is always less then or equal to 4
    for (auto v : fileWeights) {
        maxHeap.insert(v.first, v.second);
    }
    int k = 0;
    fileWeights.clear();
    // if the heap has a size of zero tell the user no files
    // matched the search string, otherwise print out the heap
    // values for the user to see.
    if (maxHeap.size() == 0) {
        cout << endl << endl;
        cout << "No search results found" << endl;
    } else {
        cout << endl << endl << "Your search results are:" << endl;
        while (k < maxHeap.size()) {
            pair<string, double> searchResult;
            searchResult = maxHeap.max();
            cout << searchResult.first << endl;
            maxHeap.popMax();
            k++;
        }
    }
}

// main function
int main(int argc, char **argv) {
    vector<string> names;
    ifstream mainFile;
    string line;
    mainFile.open("database.txt");
    cout << "Please wait.  Building database" << endl;
    // read each line of the database.txt file and
    // create a text file based on the pdf file
    // and create the database.
    while (getline(mainFile, line)) {
        names.push_back(line);
        chdir("/home/cmput274/Desktop/Final/Library");
        string str = "pdftotext " + line;
        system(conversion(str));
        int val = line.find('.', 0);
        string imVal = line.substr(0, val);
        string remFile = imVal + ".txt";
        hashCreate(remFile);
    }
    mainFile.close();
    while (true) {
        search();
        cout << "would you like to make another search? [Y/n]" << endl;
        char ans;
        cin >> ans;
        if (ans == 'n') {
            break;
        } else if (ans == 'Y') {
            continue;
        }
    }
    // delete all extra text files to save space
    // and move the directory back to where the source code
    // is.
    chdir("/home/cmput274/Desktop/Final");
    mainFile.open("database.txt");
    while (getline(mainFile, line)) {
        chdir("/home/cmput274/Desktop/Final/Library");
        int val = line.find('.', 0);
        string imVal = line.substr(0, val);
        string remFile1 = imVal + ".txt";
        remove(conversion(remFile1));
        chdir("/home/cmput274/Desktop/Final");
    }
    mainFile.close();
    return 0;
}
