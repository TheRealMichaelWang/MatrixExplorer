﻿// MatrixExplorer.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "repl_completer.h"
#include "HulaScript.h"
#include "matrix.h"

using namespace std;

static bool should_quit = false;

static HulaScript::instance::value quit(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance) {
	should_quit = true;
	return HulaScript::instance::value();
}

static HulaScript::instance::value print(std::vector<HulaScript::instance::value> arguments, HulaScript::instance& instance) {
	for (auto argument : arguments) {
		std::cout << instance.get_value_print_string(argument);
	}
	std::cout << endl;
	return HulaScript::instance::value(static_cast<double>(arguments.size()));
}

static HulaScript::instance::value parse_numerical(std::string str, HulaScript::instance& instance) {
	return instance.add_foreign_object(std::make_unique<MatrixExplorer::matrix::mat_number_type>(MatrixExplorer::rational::parse(str)));
}

int main()
{
	cout << "Matrix Explorer 2024" << std::endl;

	cout << "\nCall \"help\", \"credits\", or \"license\" for more information.\n" << std::endl;

	HulaScript::repl_completer repl_completer;
	HulaScript::instance instance(parse_numerical);

	instance.declare_global("quit", instance.make_foreign_function(quit));
	instance.declare_global("print", instance.make_foreign_function(print));

	instance.declare_global("mat", instance.make_foreign_function(MatrixExplorer::make_matrix));
	instance.declare_global("vec", instance.make_foreign_function(MatrixExplorer::make_vector));
	instance.declare_global("ident", instance.make_foreign_function(MatrixExplorer::make_identity_matrix));
	instance.declare_global("zero", instance.make_foreign_function(MatrixExplorer::make_zero_matrix));

	while (!should_quit) {
		cout << ">>> ";

		while (true) {
			string line;
			getline(cin, line);

			try {
				auto res = repl_completer.write_input(line);
				if (res.has_value()) {
					break;
				}
				else {
					cout << "... ";
				}
			}
			catch (HulaScript::compilation_error& error) {
				repl_completer.clear();
				std::cout << error.to_print_string() << std::endl;
				std::cout << ">>> ";
			}
		}

		try {
			auto res = instance.run(repl_completer.get_source(), "REPL");
			if (holds_alternative<HulaScript::instance::value>(res)) {
				cout << instance.get_value_print_string(std::get<HulaScript::instance::value>(res));
			}
			else if (holds_alternative<std::vector<HulaScript::compilation_error>>(res)) {
				auto warnings = std::get<std::vector<HulaScript::compilation_error>>(res);

				cout << warnings.size() << " warning(s): " << std::endl;
				for (auto warning : warnings) {
					cout << warning.to_print_string() << std::endl;
				}

				cout << "Press ENTER to aknowledge and continue execution..." << std::endl;
				while (cin.get() != '\n') {}

				auto run_res = instance.run_loaded();
				if (run_res.has_value()) {
					cout << instance.get_value_print_string(run_res.value());
				}
			}
		}
		catch (const HulaScript::compilation_error& error) {
			cout << error.to_print_string();
		}
		catch (const HulaScript::runtime_error& error) {
			cout << error.to_print_string();
		}
		cout << std::endl;
	}

	return 0;
}
