#include "consts.h"
#include "circuits/circuit.h"
#include "circuits/element.h"
#include "matrix/matrix.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <iomanip>

using namespace std;


Circuit::Circuit():
     variablesList(MAX_NODES+1),
     netlist(MAX_ELEMS)
{
}

Circuit::Circuit(int numElements,
                 int numNodes,
                 int numVariables,
                 vector<Element> &netlist):
     variablesList(MAX_NODES+1),
     netlist(netlist),
     numElements(numElements),
     numNodes(numNodes),
     numVariables(numVariables)
{
}


Circuit::Circuit(ifstream &netlistFile):
     variablesList(MAX_NODES+1),
     netlist(MAX_ELEMS)
{
    numElements=0;
    numNodes=0;
    numVariables=0;
    // XXX Magic! Really important!
    // Ground node!
    variablesList[0] = "0";
	
    string netlistLine;
    bool isValidElement;
    char netlistLinePrefix;
    getline(netlistFile, netlistLine);
	 x = std::stoi(netlistLine);
    title = netlistLine;
    while (getline(netlistFile, netlistLine)) {
        netlistLinePrefix = netlistLine[0];
        isValidElement = Element::isValidElement(netlistLinePrefix);

        if (isValidElement){
            // Element is valid!
            numElements++; /* XXX Starts from netlist[1] */
            if (numElements>MAX_ELEMS) {
                cout << "Invalid number of elements. Maximum number of elements is " << MAX_ELEMS << endl;
                exit(EXIT_FAILURE);
            }
            netlist[numElements] = Element(netlistLine, numNodes, variablesList,x);
        }
        else if (netlistLinePrefix != '*') {
            // Not a comment, not a valid element...
            // Invalid line!
            cout << "Invalid line: " << netlistLine << endl;
            exit(EXIT_FAILURE);
        }
        // Ignores comments!
    }
    netlistFile.close();
    cout << endl;
    // Figured out elements and all nodes

    // Now, will add extra current variables!
    numVariables = numNodes;
    for(int i=1; i<=numElements; i++){
        netlist[i].addCurrentVariables(numVariables, variablesList);
    }
    if (numVariables > MAX_NODES) {
        cout << "Extra current variables exceeded maximum number of variables: " << MAX_NODES << endl;
        exit(EXIT_FAILURE);
    }
}


void Circuit::printVariables(){
    for (int i=0; i<=numVariables; i++){
        cout << i << " -> " << variablesList[i] << endl;
    }
    cout << endl;
}


void Circuit::printSummary(){
    cout << "# nodes:\t" << numNodes << endl;
    cout << "# variables:\t" << numVariables << endl;
    cout << "# elements:\t" << numElements << endl;
    cout << endl;
}


void Circuit::applyStamps(double Yn[MAX_NODES+1][MAX_NODES+2]){
    Element element;
    for (int i=1; i<=numElements; i++) {
        element = netlist[i];
        // Will pass previousSolution in the near future...
        element.applyStamp(Yn, numVariables);
		
    }
}

void Circuit::printSolution(double Yn[MAX_NODES+1][MAX_NODES+2]){
    for (int i=1; i<=numVariables; i++) {
		std::cout << std::fixed;
		std::cout << std::setprecision(4);
        cout <<variablesList[i]<<":  " << Yn[i][numVariables+1] << endl;
    }
	//to work on later
	/*
	for (int i = 0; i <= numNodes; i++) {
		std::cout << std::fixed;
		std::cout << std::setprecision(4);
		if (i==numNodes) { cout << "I" << i << 0 << ":  " << Currents[i] << endl; }
		else {
			cout << "I" << i << i + 1 << ":  " << Currents[i] << endl;
		}
	}*/
	int f = 0;
		for (int i = 0; i < numNodes; i++)
		{
			cout << "I" << i << i + 1 << ":  " << Yn[numNodes + 1][numVariables + 1] << endl; f = i;
		}
	cout << "I" << f << 0 << ":  " << Yn[numNodes + 1][numVariables + 1] << endl;
}

void Circuit::currentValues(double Yn[MAX_NODES + 1][MAX_NODES + 2], double Xn[MAX_NODES + 1][MAX_NODES + 2])
{
	for (int i=0;i<20;i++)
	{
		Currents[i] = 0;
	}
	for (int i = 0; i < numNodes; i++)
	{
		if (Xn[i][i + 1]!=0)
		Currents[i] = (Yn[i][numVariables + 1] - Yn[i + 1][numVariables + 1])* Xn[i][i + 1];
	}


}

/* Function to write the Solution into an Output File */
bool Circuit::WriteSolutionToFile(string filename, double Yn[MAX_NODES + 1][MAX_NODES + 2]){
    // Opening the File for Writing
    ofstream file(filename.c_str(), ofstream::out);

    /* Writing the Header */
    for (int i = 1; i <= numVariables; i++){
            file << variablesList[i] << ":  " << Yn[i][numVariables + 1] << endl;
    }


    // Printing the message about the file saved
    cout << endl;
    cout << "The output file was saved as " << filename << endl;

        //Closing The File
        file.close();
    return true;
}

int Circuit::getNumElements(){
    return numElements;
};

int Circuit::getNumNodes(){
    return numNodes;
};

int Circuit::getNumVariables(){
    return numVariables;
};

