#include "attack_table.h"
#include "board.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <string>


namespace po = boost::program_options;

int main(int argc, char **argv) {
    po::options_description desc("Allowed Options");
    desc.add_options()
        ("depth", po::value<int>()->required(), "Maximum depth to search")
        ("fen", po::value<std::string>()->required(), "FEN string of the base position")
        ("moves", po::value<std::vector<std::string>>()->multitoken(), 
            "space-separated list of moves from the base position to the position "
            "to be evaluated, where each move is formatted as $source$target$promotion, "
            "e.g. e2e4 or a7b8Q"
        );
    po::positional_options_description positional;
    positional.add("depth", 1)
              .add("fen", 1)
              .add("moves", -1);

    try {
        po::variables_map vm;
        po::store(
            po::command_line_parser(argc, argv)
                .options(desc)
                .positional(positional)
                .run(),
            vm
        );
        po::notify(vm);

        const int depth { vm["depth"].as<int>() };
        const std::string fen { vm["fen"].as<std::string>() };
        std::vector<std::string> moves;

        if (vm.count("moves")) {
            moves = vm["moves"].as<std::vector<std::string>>();
        }

        std::cout << "DEPTH: " << depth << "\n";
        std::cout << "FEN: \"" << fen << "\"\n";
        std::cout << "MOVES: ";
        for (const auto &move : moves) {
            std::cout << move << " ";
        }
        std::cout << "\n";
    } catch (const std::exception &e) {
        std::cerr << desc << "\n";
    } catch (...) {
        std::cerr << desc << "\n";
    }
}