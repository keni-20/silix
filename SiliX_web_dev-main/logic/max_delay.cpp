#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <climits>
#include <queue>
#include <algorithm>
#include <bits/stdc++.h>
using namespace std;

// Data structure to represent a gate
struct Gate
{
    string type;
    vector<string> inputs;
    string output;
    int delay; // Delay of this gate
};

// Data structure to represent a net
struct Net
{
    string name;
    vector<string> drivers; // inputs coming towards node
    vector<string> loads;   // outputs going through node
    int maxDelay;           // Maximum delay through this net
};

// Parse the Verilog file and populate gates and nets
void parseVerilogFile(const string &filename, vector<Gate> &gates, map<string, Net> &nets)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error opening file" << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {
        stringstream iss(line);
        string word;
        iss >> word;

        if (word == "module" || word == "endmodule")
        {
            continue;
        }
        else if (word == "input")
        {
            while (iss >> word)
            {
                if (word.back() == ',' || word.back() == ';')
                    word.pop_back();
                nets[word] = {word, {}, {}, 0}; // Initialize primary input with maximum delay
            }
        }
        else if (word == "output")
        {
            while (iss >> word)
            {
                if (word.back() == ',' || word.back() == ';')
                    word.pop_back();
                nets[word] = {word, {}, {}, 0}; // Initialize output nets with 0 delay
            }
        }
        else if (word == "wire")
        {
            while (iss >> word)
            {
                if (word.back() == ',' || word.back() == ';')
                    word.pop_back();
                nets[word] = {word, {}, {}, 0}; // Initialize wire nets with maximum delay
            }
        }
        else if (word == "nand" || word == "or" || word == "not" || word == "buf" || word == "and" || word == "nor" || word == "xor" || word == "xnor")
        {
            Gate gate;
            gate.type = word;
            string out;
            vector<string> inputs;
            iss >> word;
            
            // Read the gate connections
            while (iss >> word)
            {
                // Remove any trailing characters like ',' or ';'
                while (!word.empty() && (word == "(" || (word.back() == ',' || word.back() == ';' || word.back() == ')')))
                {
                    word.pop_back();
                }

                // Identify output and inputs
                if (out.empty())
                {
                    out = word;
                }
                else 
                {
                    inputs.push_back(word);
                }
            }

            // Store the gate in gates vector if not already stored
            gate.output = out;
            gate.inputs = inputs;

            gates.push_back(gate);

            // Update nets
            nets[out].drivers.push_back(word);
            for (const auto &input : inputs)
            {
                nets[input].loads.push_back(out);
            }
        }
    }
}

// Function to input delays for each gate from user
void inputGateDelays(vector<Gate> &gates)
{
    map<pair<string, int>, int> uniqueGates;

    for (auto &gate : gates)
    {
        // Create a key based on gate type and inputs
        pair<string, int> gateKey = {gate.type, gate.inputs.size()};

        // Check if the gate (type, inputs) is unique
        if (uniqueGates.find(gateKey) == uniqueGates.end()) {
            cout << "Enter delay for gate " << gate.type << " with input size " << gate.inputs.size() << ": ";
            cin >> gate.delay;
            uniqueGates[gateKey] = gate.delay; // Store delay in uniqueGates
        } else {
            // If gate is already in uniqueGates, use previously entered delay
            gate.delay = uniqueGates[gateKey];
            // cout << "Using previously entered delay for gate " << gate.type << " with input size " << gate.inputs.size() << ": " << gate.delay << endl;
        }
    }
}

// Function to calculate maximum path delay
int calculateMaxPathDelay(const vector<Gate> &gates, map<string, Net> &nets)
{
    map<string, int> maxDelay; // Map to store maximum delay to each net
    queue<string> q;           // Queue for BFS
    for (const auto &net : nets)
    {
        if (net.second.drivers.empty()) // If no drivers, it's a primary input
        {
            q.push(net.first);
            maxDelay[net.first] = 0; // Initialize primary inputs with delay 0
        }
    }

    while (!q.empty())
    {
        string currentNet = q.front();
        q.pop();

        int currentDelay = maxDelay[currentNet];

        for (const auto &load : nets[currentNet].loads)
        {
            int loadDelay = currentDelay; // Delay through current net

            for (const auto &gate : gates)
            {
                if (gate.output == load && find(gate.inputs.begin(), gate.inputs.end(), currentNet) != gate.inputs.end())
                {
                    loadDelay += gate.delay;
                    break; // Assuming there's only one gate between any driver and load
                }
            }

            if (maxDelay.find(load) == maxDelay.end())
            {
                maxDelay[load] = loadDelay;
                nets[load].maxDelay = loadDelay; // Update maxDelay in nets map
                q.push(load);
            }
            else if (loadDelay > maxDelay[load])
            {
                maxDelay[load] = loadDelay;
                nets[load].maxDelay = loadDelay; // Update maxDelay in nets map
                q.push(load);
            }
        }
    }

    // Finding the maximum delay among all outputs
    int maxPathDelay = 0;
    for (const auto &net : nets)
    {
        if (net.second.loads.empty()) // If no loads, it's an output
        {
            maxPathDelay = max(maxPathDelay, maxDelay[net.first]);
        }
    }

    return maxPathDelay;
}

void printGates(const vector<Gate> &gates)
{
    cout << "List of Gates:" << endl;
    for (const auto &gate : gates)
    {
        cout << "Type: " << gate.type << ", Inputs: ";
        for (const auto &input : gate.inputs)
        {
            cout << input << " ";
        }
        cout << ", Output: " << gate.output << ", Delay: " << gate.delay << endl;
    }
}

// Function to print the map of nets
void printNetsMap(const map<string, Net> &nets)
{
    cout << "Map of Nets:" << endl;
    for (const auto &entry : nets)
    {
        const Net &net = entry.second;
        cout << "Name: " << net.name << ", Drivers:";
        for (const auto &driver : net.drivers)
        {
            cout << " " << driver;
        }
        cout << ", Loads:";
        for (const auto &load : net.loads)
        {
            cout << " " << load;
        }
        cout << ", Max Delay: " << net.maxDelay << endl;
    }
}


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <Verilog file path>" << endl;
        return 1;
    }

    string filename = argv[1];

    vector<Gate> gates;
    map<string, Net> nets;

    // Replace "filename.v" with your Verilog file name
    // parseVerilogFile("F:/SRI/netlist-analyzer/logic build/Slack/ex_1.v", gates, nets);
    parseVerilogFile(filename, gates, nets);

    cout << "Input gate delays:" << endl;
    // Input delays for each gate
    inputGateDelays(gates);

    // printGates(gates);

    int maxPathDelay = calculateMaxPathDelay(gates, nets);

    // printNetsMap(nets);

    cout << "Maximum path delay in the circuit: " << maxPathDelay << endl;

    return 0;
}