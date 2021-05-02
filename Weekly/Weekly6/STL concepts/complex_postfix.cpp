/*
Name: Ryan Brooks
ID: 1530605
Course: CMPUT 275 winter 2019
assignment: Weekly exercise 6: STL concepts
*/
#include <iostream>
#include <utility>
#include <stack>

using namespace std;

// declaration of type variables to make the code more readable
typedef long long int lInt;
typedef pair<lInt, lInt> cmplx;

// declaration of our storage device
stack<cmplx> storage;


// function that reads in input from the keyboard and acts
// accordingly based on what values are passed as the first
// input
void calculate() {
    // initial variable creation
    lInt r1, r2;
    char whatDo;
    char binaryOp, unaryOp;
    // infinite loop until the value S is passed as input
    while (true) {
        // read in the input that tells the code what to do
        cin >> whatDo;
        // if whatDo is V then we read in the complex value and store
        // it as a pair in the storage device
        if (whatDo == 'V') {
            cin >> r1 >> r2;
            cmplx value (r1, r2);
            storage.push(value);
        } else if (whatDo == 'B') {  // if whatDo is B then we read in the
                                     // arithmetic operator and act accordingly
            cin >> binaryOp;
            cmplx val1 = storage.top(); storage.pop();
            cmplx val2 = storage.top(); storage.pop();
            // add the two topmost values in the stack
            if (binaryOp == '+') {
                lInt imVal1 = val1.first + val2.first;
                lInt imVal2 = val1.second + val2.second;
                cmplx val (imVal1, imVal2);
                storage.push(val);
            } else if (binaryOp == '-') {  // subtract the two topmost values in the stack
                lInt imVal1 = val1.first - val2.first;
                lInt imVal2 = val1.second - val2.second;
                cmplx val (imVal1, imVal2);
                storage.push(val);
            } else if (binaryOp == '*') {  // multiply the two topmost values in the stack
                lInt imVal1 = (val1.first * val2.first) - (val1.second * val2.second);
                lInt imVal2 = (val1.first * val2.second) + (val1.second * val2.first);
                cmplx val (imVal1, imVal2);
                storage.push(val);
            }
        } else if (whatDo == 'U') { // if whatDo is U then we take out only the topmost
                                    // value from the stack and perform the proper action
                                    // upon it.
            cin >> unaryOp;
            cmplx val1 = storage.top(); storage.pop();
            // negate both the real and complex value of the topmost
            // value in the stack
            if (unaryOp == '-') {
                cmplx val2 ((val1.first * -1), (val1.second * -1));
                storage.push(val2);
            } else if (unaryOp == 'c') {  // negate the complex value of the topmost pair
                                          // in the stack.
                cmplx val2 (val1.first, (val1.second * -1));
                storage.push(val2);
            }
        } else if (whatDo == 'S') {  // if whatDo is S then print the only value
                                     // in the stack and print it.
            cmplx val = storage.top(); storage.pop();
            cout << val.first << ' ' << val.second << endl;
            return;
        }
    }
}

// main function
int main() {
    calculate();
    return 0;
}
