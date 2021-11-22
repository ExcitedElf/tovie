#include "c_compiler.hpp"
#include "info.hpp"
#include "operations.hpp"
#include "parser.hpp"
#include "python_compiler.hpp"
#include "runtimelib.hpp"
#include "table.hpp"
#include "vm.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <string>

std::string read_file(std::string filePath) {
	std::ifstream	  file(filePath);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void write_file(std::string data, std::string filePath) {
	std::ofstream out(filePath);
	out << data;
	out.close();
}

void print_usage() {
	std::cout << "Usage: toviec <job> <input_file> [output_file]" << std::endl;
	std::cout << "  job: " << std::endl;
	std::cout << "    ver  : print version info" << std::endl;
	std::cout << "    com  : compile" << std::endl;
	std::cout << "    sis  : simulate from soruce" << std::endl;
	std::cout << "    sib  : simulate from bytecode" << std::endl;
	std::cout << "    ssd  : simulate from source in debug mode" << std::endl;
	std::cout << "    sbd  : simulate from bytecode in debug mode" << std::endl;
	std::cout << "    gts  : generate operations table from source" << std::endl;
	std::cout << "    gtb  : generate operations table from binary" << std::endl;
	std::cout << "    t2p  : translate to python 3" << std::endl;
	std::cout << "    t2c  : translate to c (c11)" << std::endl;
	// std::cout << "    t2s  : translate to c#" << std::endl;
	// std::cout << "    t2j  : translate to java" << std::endl;
	exit(0);
}

static void print(std::vector<Operation> ops) {
	std::cout << "Operation List : " << std::endl;
	for (auto& op : ops) {
		std::cout << op.op << std::endl;
	}
}

static void save(std::vector<Operation> ops, std::string filePath) {
	std::ofstream fout(filePath, std::ios::out | std::ios::binary);
	fout.write((char*) &ops[0], ops.size() * sizeof(Operation));
	fout.close();
}

static std::vector<Operation> load(std::string filePath) {
	std::ifstream	file(filePath, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char>	   buffer(size);
	std::vector<Operation> ops;
	if (file.read(buffer.data(), size)) {
		int		   count = size / sizeof(Operation);
		Operation* opsp	 = (Operation*) buffer.data();
		for (int i = 0; i < count; i++) {
			ops.push_back(*opsp);
			opsp++;
		}
	}
	return ops;
}

// parse file data from argument and print oprtaions
int main(int argc, char** argv) {
	std::string fileData	= "";
	std::string filePath	= "";
	std::string ofilePath	= "a.toviec";
	std::string mode		= "com";
	std::string includePath = ".";
	if (argc == 2 && strcmp(argv[1], "ver") == 0) {
		std::cout << get_name() << std::endl;
		exit(0);
	}
	if (argc == 3) {
		mode	 = argv[1];
		filePath = argv[2];

	} else if (argc == 4) {
		mode	  = argv[1];
		filePath  = argv[2];
		ofilePath = argv[3];
	} else {
		print_usage();
	}

	if (mode == "com") {
		fileData = read_file(filePath);
		std::cout << "Compiling..." << std::endl;
		std::vector<Operation> operations = parse(fileData, includePath);
		save(operations, ofilePath);
	} else if (mode == "sis") {
		fileData = read_file(filePath);
		simulate(parse(fileData, includePath));
	} else if (mode == "ssd") {
		fileData = read_file(filePath);
		simulate(parse(fileData, includePath), true);
	} else if (mode == "sib") {
		std::vector<Operation> operations = load(filePath);
		simulate(operations);
	} else if (mode == "sbd") {
		std::vector<Operation> operations = load(filePath);
		simulate(operations, true);
	} else if (mode == "gts") {
		std::cout << "Generating OP table " << filePath << " ..." << std::endl;
		std::string			   data		  = read_file(filePath);
		std::vector<Operation> operations = parse(data, includePath);
		write_file(generate_op_table(operations), filePath + ".tovieopt");
	} else if (mode == "gtb") {
		std::cout << "Generating OP table " << filePath << " ..." << std::endl;
		std::vector<Operation> operations = load(filePath);
		write_file(generate_op_table(operations), filePath + ".tovieopt");
	} else if (mode == "t2p") {
		fileData = read_file(filePath);
		std::cout << "Transpiling " << filePath << " ..." << std::endl;
		std::cout << std::endl;
		std::string out = tovie2py(parse(fileData, includePath));
		write_file(out, ofilePath + ".py");
	} else if (mode == "t2c") {
		fileData = read_file(filePath);
		std::cout << "Transpiling " << filePath << " ..." << std::endl;
		std::cout << std::endl;
		std::string out = tovie2c(parse(fileData, includePath));
		write_file(out, ofilePath + ".c");
	} else {
		print_usage();
	}
}