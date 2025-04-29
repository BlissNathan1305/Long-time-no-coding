#include <iostream>
using namespace std;

int main() {
    // Loop through numbers 1 to 12 to generate tables
    for (int i = 1; i <= 12; ++i) {
        cout << "Multiplication Table of " << i << ":" << endl;
        for (int j = 1; j <= 12; ++j) {
            cout << i << " x " << j << " = " << i * j << endl;
        }
        cout << endl; // Add a blank line between tables
    }
    return 0;
}
