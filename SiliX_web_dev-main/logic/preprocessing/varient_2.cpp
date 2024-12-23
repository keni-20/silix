#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <unordered_set>
#include <unordered_map>

struct Module {
    std::string name;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::vector<std::string> wires;
    std::vector<std::string> gates;
};

void parseVerilog(const std::string& filename, Module& module) {
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    std::regex module_regex(R"(module\s+(\w+)\s*\()");
    // std::regex input_regex(R"(\s*input\s+(\w+)\s*[,;]?)");
    // std::regex output_regex(R"(\s*output\s+(\w+)\s*[,;]?)");
    // std::regex wire_regex(R"(wire\s+(.*)\s*;)");
    std::regex input_regex(R"(\s*input\s*(\w+)\s*[,;]?)");
    std::regex output_regex(R"(\s*output\s*(\w+)\s*[,;]?)");
    std::regex wire_regex(R"(wire\s*(.*?)\s*;)");
    std::regex gate_regex(R"((\w+)\s+(\w+)\s*\((.*)\)\s*;)");

    bool module_decl = false;
    while (std::getline(infile, line)) {
        std::smatch match;
        if (std::regex_search(line, match, module_regex)) {
            module.name = match[1].str();
            module_decl = true;
            continue;
        }
        if (module_decl) {
            if (std::regex_search(line, match, input_regex)) {
                module.inputs.push_back(match[1].str());
                continue;
            }
            if (std::regex_search(line, match, output_regex)) {
                module.outputs.push_back(match[1].str());
                continue;
            }
            if (line.find(");") != std::string::npos) {
                module_decl = false;
                continue;
            }
        }
        if (std::regex_search(line, match, wire_regex)) {
            std::istringstream ss(match[1].str());
            std::string wire;
            while (ss >> wire) {
                if (wire.back() == ',') wire.pop_back();
                module.wires.push_back(wire);
            }
            continue;
        }
        if (std::regex_search(line, match, gate_regex)) {
            // Process gate line and parameters correctly
            std::string gate_type = match[1].str();
            std::string instance_name = match[2].str();
            std::string params_str = match[3].str();
            std::istringstream ss(params_str);
            std::string param;
            std::vector<std::string> parameters;
            while (std::getline(ss, param, ',')) {
                // Trim spaces
                param.erase(param.find_last_not_of(" \n\r\t") + 1);
                param.erase(0, param.find_first_not_of(" \n\r\t"));
                if (!param.empty()) {
                    parameters.push_back(param);
                }
            }
            if (!parameters.empty()) {
                // Construct the gate instance with all parameters
                std::ostringstream gate_instance;
                gate_instance << gate_type << " " << instance_name << " ( ";
                for (size_t i = 0; i < parameters.size(); ++i) {
                    if (i != 0) gate_instance << ", ";
                    gate_instance << parameters[i];
                }
                gate_instance << " );";
                module.gates.push_back(gate_instance.str());
            } else {
                // If no parameters, add the gate line as is
                module.gates.push_back(line);
            }
            continue;
        }
    }

    infile.close();
}

void writeVerilog(const std::string& filename, const Module& module) {
    std::ofstream outfile(filename);
    if (!outfile) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    outfile << "module " << module.name << " (";
    for (size_t i = 0; i < module.inputs.size(); ++i) {
        if (i != 0) outfile << ",";
        outfile << module.inputs[i];
    }
    for (size_t i = 0; i < module.outputs.size(); ++i) {
        if (i != 0 || !module.inputs.empty()) outfile << ",";
        outfile << module.outputs[i];
    }
    outfile << ");\n\n";

    outfile << "input ";
    for (size_t i = 0; i < module.inputs.size(); ++i) {
        if (i != 0) outfile << ", ";
        outfile << module.inputs[i];
    }
    outfile << ";\n\n";

    outfile << "output ";
    for (size_t i = 0; i < module.outputs.size(); ++i) {
        if (i != 0) outfile << ", ";
        outfile << module.outputs[i];
    }
    outfile << ";\n\n";

    if (!module.wires.empty()) {
        outfile << "wire ";
        for (size_t i = 0; i < module.wires.size(); ++i) {
            if (i != 0) outfile << ", ";
            outfile << module.wires[i];
        }
        outfile << ";\n\n";
    }

    for (const auto& gate : module.gates) {
        std::regex gate_space_regex(R"((\w+)\s+(\w+)\s*\((.*)\)\s*;)");
        std::smatch match;
        if (std::regex_match(gate, match, gate_space_regex)) {
            std::string formatted_gate = match[1].str() + " " + match[2].str() + " (";
            std::istringstream ss(match[3].str());
            std::string param;
            while (ss >> param) {
                formatted_gate += " " + param;
            }
            //if (formatted_gate.back() == ',') formatted_gate.pop_back(); // Remove trailing comma
            formatted_gate += " );";
            outfile << formatted_gate << '\n';
        } else {
            // If regex doesn't match, write the gate line as is
            outfile << gate << '\n';
        }
    }

    outfile << "endmodule\n";
    outfile.close();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_filename> <output_filename>" << std::endl;
        return 1;
    }

    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];

    Module module;
    parseVerilog(inputFilename, module);
    writeVerilog(outputFilename, module);

    std::cout << "Updated Verilog file has been written to " << outputFilename << std::endl;

    return 0;
}