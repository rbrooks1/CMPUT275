/*
Name: Ryan Brooks
ID: 1530605
Course: CMPUT 275 winter 2019
assignment: Weekly exercise 3: pseudocode to code
*/

#include <iostream>
#include <cstring>  // for strlen()

using namespace std;

// function that implements the textmatch algorithm to
// find the first index where a substring exists in a given string
void textMatch(char s[100001], char t[100001]) {
  // initializing variables for use in the algorithm
  int step[100001];
  step[0] = -1;
  int k = -1;
  int m = -1;

  // filling up the step function
  for (unsigned int i = 1; i < (strlen(s)); i++) {
    while ((k >= 0) && (s[k + 1] != s[i])) {
      k = step[k];
    }
    if (s[k + 1] == s[i]) {
      k += 1;
    }
    step[i] = k;
  }
  // checking for indices where a substring starts in a
  // given string
  for (unsigned int i = 0; i < (strlen(t)); i++) {
    while ((m >= 0) && (s[m + 1] != t[i])) {
      m = step[m];
    }
    if (s[m + 1] == t[i]) {
        m += 1;
    }
    if (m == (int)(strlen(s) - 1)) {
        cout << i + 1 - strlen(s) << ' ';
        m = step[m];
    }
  }
}

// main function
int main() {
  // initializing variables
  int n = 100001;
  char s[n];
  char t[n];
  cin >> s;
  cin >> t;

  // running the algorithm and printing the endline
  // to stop presentation errors in the code
  textMatch(s, t);
  cout << endl;
  return 0;
}
