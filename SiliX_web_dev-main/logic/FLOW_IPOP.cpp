#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <vector>
#include <string>
#include <unordered_map>
#include <windows.h>
using namespace std;

struct Gate // the structure to store the data of gate
{
    string type;
    string g_name;
    vector<string> inputs;
    vector<string> outputs;
};

struct Module // to store the whole circuit's basic information
{
    vector<string> inputs;
    vector<string> outputs;
    vector<string> wires;
    vector<Gate> gates;
};

struct Node // to store the node (primary, gate)
{
    string name;
    string type;
    Node *next = NULL;
};

Module parseVerilog(const string &filename) // function to fatch Verilog file and return the Module
{
    ifstream file(filename);
    string line;
    regex inputRegex(R"(input\s+([a-zA-Z0-9_,\s]+);)");
    regex wireRegex(R"(wire\s+([a-zA-Z0-9_,\s]+);)");
    regex outputRegex(R"(output\s+([a-zA-Z0-9_,\s]+);)");
    regex nandRegex(R"(nand\s+([()a-zA-Z0-9_,\s]+);)");
    regex andRegex(R"(and\s+([()a-zA-Z0-9_,\s]+);)");
    regex orRegex(R"(or\s+([()a-zA-Z0-9_,\s]+);)");
    regex xorRegex(R"(xor\s+([()a-zA-Z0-9_,\s]+);)");
    regex xnorRegex(R"(xnor\s+([()a-zA-Z0-9_,\s]+);)");
    regex norRegex(R"(nor\s+([()a-zA-Z0-9_,\s]+);)");
    regex notRegex(R"(not\s+([()a-zA-Z0-9_,\s]+);)");
    regex bufRegex(R"(buf\s+([()a-zA-Z0-9_,\s]+);)");
    
    Module module;

    while (getline(file, line))
    {
        smatch match;

        if (regex_search(line, match, inputRegex))
        {
            string inputsStr = match[1].str();

            stringstream inputsStream(inputsStr);
            string input;
            while (getline(inputsStream, input, ','))
            {
                input.erase(remove_if(input.begin(), input.end(), ::isspace), input.end());
                module.inputs.push_back(input);
            }
        }
        else if (regex_search(line, match, wireRegex))
        {
            string wiresStr = match[1].str();
            stringstream wiresStream(wiresStr);
            string wire;
            while (getline(wiresStream, wire, ','))
            {
                wire.erase(remove_if(wire.begin(), wire.end(), ::isspace), wire.end());
                module.wires.push_back(wire);
            }
        }
        else if (regex_search(line, match, outputRegex))
        {
            string outputsStr = match[1].str();
            stringstream outputsStream(outputsStr);
            string output;
            while (getline(outputsStream, output, ','))
            {
                output.erase(remove_if(output.begin(), output.end(), ::isspace), output.end());
                module.outputs.push_back(output);
            }
        }
        else if (regex_search(line, match, nandRegex) ||
                 regex_search(line, match, andRegex) ||
                 regex_search(line, match, norRegex) ||
                 regex_search(line, match, xnorRegex) ||
                 regex_search(line, match, xorRegex) ||
                 regex_search(line, match, orRegex) ||
                 regex_search(line, match, notRegex) ||
                 regex_search(line, match, bufRegex))
        {
            Gate gate;
            gate.type = match[0].str().substr(0, match[0].str().find(" "));
            string inputsStr = match[1].str();
            inputsStr.erase(remove(inputsStr.begin(), inputsStr.end(), ' '), inputsStr.end());
            stringstream inputsStream(inputsStr);
            string input;
            string ss;
            getline(inputsStream, ss, '(');
            gate.g_name = ss;
            while (getline(inputsStream, input, ','))
                gate.inputs.push_back(input);
            string sss = gate.inputs.back();
            sss = sss.substr(0, sss.size() - 1);
            gate.inputs[gate.inputs.size() - 1] = sss;
            if (gate.inputs.size() != 0)
            {
                gate.outputs.push_back(gate.inputs[0]);
                gate.inputs.erase(gate.inputs.begin());
            }
            module.gates.push_back(gate);
        }
    }

    return module;
}

void connectNodes(vector<Node *> &nodes, const Module &module) // to make connections between all nodes
{
    unordered_map<string, int> mp;
    int ind = 0;
    for (auto it : module.inputs)
    {
        Node *ne = new Node;
        ne->name = it;
        ne->type = "INPUT";
        ne->next = NULL;
        nodes.push_back(ne);
        mp[it] = ind++;
    }
    for (auto it : module.outputs)
    {
        Node *ne = new Node;
        ne->name = it;
        ne->next = NULL;
        nodes.push_back(ne);
        mp[it] = ind++;
    }
    for (auto it : module.gates)
    {
        string type_g = it.type;
        vector<string> ip = it.inputs;
        string op = it.outputs[0];
        for (auto it1 : ip)
        {
            Node *node = nodes[mp[it1]];
            Node *ne = new Node();
            ne->name = op;
            ne->type = type_g;
            nodes[mp[op]]->type = type_g;
            ne->next = NULL;
            while (node->next != NULL)
                node = node->next;
            node->next = ne;
        }
    }
}

void displaycircuit(vector<Node *> &nodes, ofstream &outputFile) // to display the circuit
{

    for (auto it : nodes)
    {
        Node *node = it;
        while (node)
        {
            //outputFile << node->type << " ";
            outputFile << node->name << " -> ";
            node = node->next;
        }
        outputFile << "NULL" << endl;
    }
}

// fuctions to compute the values of logic gate
/*--------------------------------------------------------------*/

bool computeXORGate(const vector<string> &inputs, unordered_map<string, int> &mp)
{
    bool output = false;
    for (const auto &input : inputs)
    {
        output = output ^ mp[input];
    }
    return output;
}
bool computeANDGate(const vector<string> &inputs, unordered_map<string, int> &mp)
{
    bool output = true;
    for (const auto &input : inputs)
    {
        output = output && mp[input];
    }
    return output;
}
bool computeORGate(const vector<string> &inputs, unordered_map<string, int> &mp)
{
    bool output = false;
    for (const auto &input : inputs)
    {
        output = output || mp[input];
    }
    return output;
}
bool computeNOTGate(bool inputs)
{
    return !inputs;
}
bool computebufGate(bool inputs)
{
    return inputs;
}
/*--------------------------------------------------------------*/

void compute(unordered_map<string, int> &mp, vector<Gate> &ge) // will compute the outputs of the circuit
{
    for (auto it : ge)
    {
        vector<string> inp = it.inputs;
        string typ = it.type;
        string st = it.outputs[0];

        if (typ == "or")
        {
            bool ans = computeORGate(inp, mp);
            mp[st] = ans;
        }
        if (typ == "nor")
        {
            bool ans = computeORGate(inp, mp);
            mp[st] = computeNOTGate(ans);
        }
        if (typ == "and")
        {
            bool ans = computeANDGate(inp, mp);
            mp[st] = ans;
        }
        if (typ == "nand")
        {
            bool ans = computeANDGate(inp, mp);
            mp[st] = computeNOTGate(ans);
        }
        if (typ == "xnor")
        {
            bool ans = computeXORGate(inp, mp);
            mp[st] = computeNOTGate(ans);
        }
        if (typ == "xor")
        {
            bool ans = computeXORGate(inp, mp);
            mp[st] = ans;
        }
        if (typ == "not")
            mp[st] = computeNOTGate(mp[inp[0]]);

        if (typ == "buf")
            mp[st] = computebufGate(mp[inp[0]]);
    }
}

void generateAllBinaryStrings(int n, vector<int> &arr, int i, Module &module, unordered_map<string,int>&mp, ofstream &outputFile) // to generate the permutations for input
{
    if (i == n)
    {
        outputFile << "Inputs: ";
        for (int j = 0; j < n; j++)
        {
            outputFile << module.inputs[j] << " : " << arr[j];
            if(j != n-1){
                outputFile << ", ";
            }
            mp[module.inputs[j]] = arr[j];
        }
        outputFile << endl;
        compute(mp,module.gates);
        outputFile << "Outputs: ";
        for (auto it : mp)
        {
            outputFile << it.first << " : " << it.second << ", ";
        }
        outputFile << endl;
        outputFile << endl;
        return;
    }
    arr[i] = 0;
    generateAllBinaryStrings(n, arr, i + 1, module, mp, outputFile);

    arr[i] = 1;
    generateAllBinaryStrings(n, arr, i + 1, module, mp, outputFile);
}

// Function to generate output filename
string generateOutputFilename(const string &inputFilename)
{
    size_t lastDotPos = inputFilename.find_last_of(".");
    if (lastDotPos == string::npos)
    {
        return inputFilename + "_output.txt";
    }
    return inputFilename.substr(0, lastDotPos) + "_output.txt";
}

/*-------------------------------: Driver Function :-----------------------------------*/
int main(int argc, char *argv[])
{
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_verilog_file>" << endl;
        return 1;
    }

    string file_name = argv[1]; // Read input file name from command-line argument
    // string file_name = "F:/SRI/netlist-analyzer/logic build/example.v";
    // string file_name;
    // cout << "Enter the file name you want to make circuit: ";
    // cin >> file_name;

    Module module = parseVerilog(file_name); // fatching the verilog file
    vector<Node *> nodes;
    connectNodes(nodes, module);
    unordered_map<string, int> mp;

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

    string outputFilename = outputPath + "\\results.txt";
    ofstream outputFile(outputFilename);
    if (!outputFile.is_open())
    {
        cerr << "Error: Could not open output file " << outputFilename << endl;
        return 1;
    }

    // cout << endl;
    // outputFile << "-------------Circuit Representation-------------" << endl << endl;
    // displaycircuit(nodes, outputFile);
    // outputFile << endl << endl;

    outputFile << "----------------Input Section------------------" << endl << endl;
    int n = module.inputs.size();
    vector<int> arr(n, 0);
    generateAllBinaryStrings(n, arr, 0, module, mp, outputFile);

    outputFile.close();

    return 0;
}