/*
Name: Ryan Brooks
ID: 1530605
Course: CMPUT 275 winter 2019
assignment: Weekly exercise 3: pseudocode to code
*/

#include <math.h>  // for ceil
#include <iostream>
// ceil is used because in the pseudo code there are
// symbols for ceiling and not plain square brackets
using namespace std;

// function that implements the flipflopsort algorithm
// to sort an array of integers
void FFSort(unsigned int arr[], unsigned int n) {
  if (n == 2) {
    // if n is equal to 2 then just swap the 2 values
    if (arr[0] > arr[1]) {
        swap(arr[0], arr[1]);
    }
  } else {
    // run FFSort if n is greater than 2
    unsigned int ceiling = ceil(2*n/3.0);
    FFSort(arr, ceiling);
    FFSort(&arr[n-ceiling], ceiling);
    FFSort(arr, ceiling);
  }
}

// main function
int main() {
  // take in input
  unsigned int n;
  cin >> n;
  unsigned int arr[n];
  for (unsigned int i = 0; i < n; i++) {
    cin >> arr[i];
  }
  // assuming the length of the array if greater than 1
  // run the sorting algorithm
  if (n >= 2) {
    FFSort(arr, n);
  }
  // output the results of the sorted array
  for (unsigned int i = 0; i < n; i++) {
    if (i == (n - 1)) {
        cout << arr[i] << endl;
    } else {
        cout << arr[i] << ' ';
    }
  }
  return 0;
}
