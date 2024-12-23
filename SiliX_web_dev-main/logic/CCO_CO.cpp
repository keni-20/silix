#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h> // Include Windows API headers

using namespace std;

// Basic data structure to represent a gate
struct Gate
{
    string type;
    vector<string> inputs;
    string output;
};

// Data structure to represent a net
struct Net
{
    string name;
    vector<string> drivers; // input coming towards node
    vector<string> loads; // output going through node
    int cc0; // Controllability 0
    int cc1; // Controllability 1
    int co;  // Observability
};

// Parse the Verilog file
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
                nets[word] = {word, {}, {}, 1, 1, INT_MAX}; // Initialize primary input controllability
            }
        }
        else if (word == "output")
        {
            while (iss >> word)
            {
                if (word.back() == ',' || word.back() == ';')
                    word.pop_back();
                nets[word] = {word, {}, {}, 0, 0, 0}; // Initialize output nets
            }
        }
        else if (word == "wire")
        {
            while (iss >> word)
            {
                if (word.back() == ',' || word.back() == ';')
                    word.pop_back();
                nets[word] = {word, {}, {}, -1, -1, INT_MAX}; // Initialize wire nets
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

            // Store the gate in gates vector
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


// Compute controllability (simplified)
void computeControllability(const vector<Gate> &gates, map<string, Net> &nets)
{
    for (const auto &gate : gates)
    {
        int cc0, cc1;
        if (gate.type == "buf")
        {
            cc0 = nets[gate.inputs[0]].cc0 + 1;
            cc1 = nets[gate.inputs[0]].cc1 + 1;
        }
        else if (gate.type == "not")
        {
            cc0 = nets[gate.inputs[0]].cc1 + 1;
            cc1 = nets[gate.inputs[0]].cc0 + 1;
        }
        else if (gate.type == "and")
        {
            cc0 = INT_MAX;
            cc1 = 0;
            for (const auto &input : gate.inputs)
            {
                cc0 = min(cc0, nets[input].cc0);
                cc1 += nets[input].cc1;
            }
            cc0 += 1;
            cc1 += 1;
        }
        else if (gate.type == "or")
        {
            cc0 = 0;
            cc1 = INT_MAX;
            for (const auto &input : gate.inputs)
            {
                cc0 += nets[input].cc0;
                cc1 = min(cc1, nets[input].cc1);
            }
            cc0 += 1;
            cc1 += 1;
        }
        else if (gate.type == "nand")
        {
            cc1 = INT_MAX;
            cc0 = 0;
            for (const auto &input : gate.inputs)
            {
                cc1 = min(cc1, nets[input].cc0);
                cc0 += nets[input].cc1;
            }
            cc0 += 1;
            cc1 += 1;
        }
        else if (gate.type == "nor")
        {
            cc1 = 0;
            cc0 = INT_MAX;
            for (const auto &input : gate.inputs)
            {
                cc1 += nets[input].cc0;
                cc0 = min(cc0, nets[input].cc1);
            }
            cc0 += 1;
            cc1 += 1;
        }
        else if (gate.type == "xor")
        {
            cc0 = INT_MAX;
            cc1 = INT_MAX;
            for (const auto &input1 : gate.inputs)
            {
                for (const auto &input2 : gate.inputs)
                {
                    if (input1 != input2)
                    {
                        cc0 = min(cc0, nets[input1].cc0 + nets[input2].cc0);
                        cc0 = min(cc0, nets[input1].cc1 + nets[input2].cc1);
                        cc1 = min(cc1, nets[input1].cc0 + nets[input2].cc1);
                        cc1 = min(cc1, nets[input1].cc1 + nets[input2].cc0);
                    }
                }
            }
            cc0 += 1;
            cc1 += 1;
        }
        else if (gate.type == "xnor")
        {
            cc1 = INT_MAX;
            cc0 = INT_MAX;
            for (const auto &input1 : gate.inputs)
            {
                for (const auto &input2 : gate.inputs)
                {
                    if (input1 != input2)
                    {
                        cc1 = min(cc1, nets[input1].cc0 + nets[input2].cc0);
                        cc1 = min(cc1, nets[input1].cc1 + nets[input2].cc1);
                        cc0 = min(cc0, nets[input1].cc0 + nets[input2].cc1);
                        cc0 = min(cc0, nets[input1].cc1 + nets[input2].cc0);
                    }
                }
            }
            cc0 += 1;
            cc1 += 1;
        }
        else
        {
            // Handle other gate types or add an error message
            continue;
        }
        nets[gate.output].cc0 = cc0;
        nets[gate.output].cc1 = cc1;
    }
}

// Compute observability (simplified)
void computeObservability(const vector<Gate> &gates, map<string, Net> &nets)
{
    // Initialize primary outputs' observability
    for (auto &net : nets)
    {
        if (net.second.loads.empty())
        {
            net.second.co = 0;
        }
    }

    // Reverse traverse gates for observability calculation
    for (auto it = gates.rbegin(); it != gates.rend(); ++it)
    {
        int co = nets[it->output].co + 1;
        if (it->type == "buf")
        {
            nets[it->inputs[0]].co = min(nets[it->inputs[0]].co, co);
        }
        else if (it->type == "not")
        {
            nets[it->inputs[0]].co = min(nets[it->inputs[0]].co, co);
        }
        else if (it->type == "and")
        {
            for (const auto &input : it->inputs)
            {
                int other_co = co;
                for (const auto &other_input : it->inputs)
                {
                    if (input != other_input)
                    {
                        other_co += nets[other_input].cc1;
                    }
                }
                nets[input].co = min(nets[input].co, other_co);
            }
        }
        else if (it->type == "or")
        {
            for (const auto &input : it->inputs)
            {
                int other_co = co;
                for (const auto &other_input : it->inputs)
                {
                    if (input != other_input)
                    {
                        other_co += nets[other_input].cc0;
                    }
                }
                nets[input].co = min(nets[input].co, other_co);
            }
        }
        else if (it->type == "nand")
        {
            for (const auto &input : it->inputs)
            {
                int other_co = co;
                for (const auto &other_input : it->inputs)
                {
                    if (input != other_input)
                    {
                        other_co += nets[other_input].cc1;
                    }
                }
                nets[input].co = min(nets[input].co, other_co);
            }
        }
        else if (it->type == "nor")
        {
            for (const auto &input : it->inputs)
            {
                int other_co = co;
                for (const auto &other_input : it->inputs)
                {
                    if (input != other_input)
                    {
                        other_co += nets[other_input].cc0;
                    }
                }
                nets[input].co = min(nets[input].co, other_co);
            }
        }
        else if (it->type == "xor")
        {
            nets[it->inputs[0]].co = min(nets[it->inputs[0]].co, co + min(nets[it->inputs[1]].cc0, nets[it->inputs[1]].cc1));
            nets[it->inputs[1]].co = min(nets[it->inputs[1]].co, co + min(nets[it->inputs[0]].cc0, nets[it->inputs[0]].cc1));
        }
        else if (it->type == "xnor")
        {
            nets[it->inputs[0]].co = min(nets[it->inputs[0]].co, co + min(nets[it->inputs[1]].cc0, nets[it->inputs[1]].cc1));
            nets[it->inputs[1]].co = min(nets[it->inputs[1]].co, co + min(nets[it->inputs[0]].cc0, nets[it->inputs[0]].cc1));
        }
        else
        {
            // Handle other gate types or add an error message
            continue;
        }
    }
}


// Function to get unique gate types
set<string> getUniqueGateTypes(const vector<Gate> &gates)
{
    set<string> uniqueGateTypes;
    for (const auto &gate : gates)
    {
        uniqueGateTypes.insert(gate.type);
    }
    return uniqueGateTypes;
}

// Function to write results to a text file
bool writeResultsToFile(const string &outputFilePath, const set<string> &uniqueGateTypes, const map<string, Net> &nets)
{
    ofstream outputFile(outputFilePath);
    if (!outputFile.is_open())
    {
        cerr << "Error opening output file: " << outputFilePath << endl;
        return false;
    }

    // Write unique gate types to file
    outputFile << "Unique Gate Types: ";
    for (const auto &type : uniqueGateTypes)
    {
        outputFile << type << " ";
    }
    outputFile << endl;

    // Write nets information to file
    for (const auto &net : nets)
    {
        outputFile << "Net: " << net.first
                   << ", CC0: " << net.second.cc0
                   << ", CC1: " << net.second.cc1
                   << ", CO: " << net.second.co << endl;
    }

    outputFile.close();
    cout << "Results have been written to " << outputFilePath << endl;
    return true;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <verilog_filename>" << endl;
        return 1;
    }

    vector<Gate> gates;
    map<string, Net> nets;

    string verilogFilePath = argv[1];

    // Parse the Verilog file
    parseVerilogFile(verilogFilePath, gates, nets);

    // Compute controllability
    computeControllability(gates, nets);

    // Compute observability
    computeObservability(gates, nets);

    // Get unique gate types
    set<string> uniqueGateTypes = getUniqueGateTypes(gates);

    // Example of using Windows API to create a directory
    std::string outputPath = "./uploaded/output";
    if (!CreateDirectory(outputPath.c_str(), NULL))
    {
        if (ERROR_ALREADY_EXISTS == GetLastError())
        {
            std::cout << "Output directory already exists." << std::endl;
        }
        else
        {
            std::cerr << "Failed to create output directory." << std::endl;
            return 1;
        }
    }
    else
    {
        std::cout << "Output directory created successfully." << std::endl;
    }

    // Generate output file path
    std::string outputFilePath = outputPath + "\\results.txt";

    // Write results to file
    if (!writeResultsToFile(outputFilePath, uniqueGateTypes, nets))
    {
        cerr << "Failed to write results to file." << endl;
        return 1;
    }


    return 0;
}

