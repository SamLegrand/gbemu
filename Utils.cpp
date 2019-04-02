//
// Created by sam on 29/03/19.
//

#include "Utils.h"

#include <iostream>
#include <fstream>

using namespace std;

void Utils::compareFiles() {
    ifstream f1, f2;
    f1.open("../NILS_10000.txt");
    f2.open("../output.txt");

    for(string line; getline(f1, line);)
    {
        // line = van kapot
        string line2;
        getline(f2, line2);
        if (line != line2)
        {
            cout << "oei: " << endl << "Nils: " << line << endl << "Mijn: " << line2 << endl;
            return;
        }
    }
    cout << "Geen fouten!" << endl;
}
