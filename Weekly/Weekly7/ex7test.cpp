/*
  Test program for exercise 7.

  Tests various public methods for AVLMap including the new method countLessThan().

  Currently does not compile because the countLessThan() method is not yet implemented.
  That is your job.
*/

#include <iostream>
#include <string>
#include "avlmap.h"

void printTree(const AVLMap<string, int>& tree) {
  for (AVLIterator<string, int> iter = tree.begin(); iter != tree.end(); ++iter) {
    cout << " - " << iter.key() << ' ' << iter.item() << endl;
  }
}

int main() {
  AVLMap<string, int> tree;

  cout << "Possible Commands:" << endl
       << "S - print the size of the map" << endl
       << "C <name> - count the number of entries whose name is less than (<) the given name" << endl
       << "U <name> <grade> - update the grade for the name" << endl
       << "F <name> - check if the name is in the tree" << endl
       << "R <name> - remove the entry with the given name" << endl
       << "P - print all entries in the tree, ordered by key" << endl
       << "Q - stop" << endl
       << endl;

  while (true) {
    char cmd;
    string name;
    int grade;

    cin >> cmd;
    switch(cmd) {
    case 'S':
        cout << tree.size() << endl;
        break;

    case 'U':
      cin >> name >> grade;
      tree[name] = grade;
      break;

    case 'C':
      cin >> name;
      cout << tree.countLessThan(name) << endl;
      break;

    case 'F':
      cin >> name;
      if (tree.hasKey(name)) {
        cout << name << " found with grade " << tree[name] << endl;
      }
      else {
        cout << name << " not found" << endl;
      }
      break;

    case 'R':
      cin >> name;
      if (!tree.hasKey(name)) {
        cout << name << " not found" << endl;
      }
      tree.remove(name);
      break;

    case 'P':
      cout << "printing" << endl;
      printTree(tree);
      break;

    case 'Q':
      cout << "stopping" << endl;
      return 0;

    default:
      cout << "invalid command" << endl;
      // eat up the rest of the line
      getline(cin, name);
      break;
    }

    tree.checkStructure();
  }
}
