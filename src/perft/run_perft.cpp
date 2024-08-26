#include "attack_table.h"
#include "board.h"
#include "move_gen.h"
#include "move_parse.h"

#include <algorithm>
#include <boost/program_options.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace po = boost::program_options;

struct PerftArgs {
    int depth;
    std::string fen;
    std::vector<std::string> moves;
};

std::optional<PerftArgs> parse_args(int argc, char **argv) {
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

        return PerftArgs { depth, fen, moves };
    } catch (const std::exception &e) {
        std::cerr << desc << "\n";
        return std::nullopt;
    } catch (...) {
        std::cerr << desc << "\n";
        return std::nullopt;
    }
}

int main(int argc, char **argv) {
    const auto args { parse_args(argc, argv) };
    if (!args.has_value()) {
        return 1;
    }

    const AttackTable at {};
    auto board { Board::init(args->fen) };
    if (!board.has_value()) {
        std::cerr << "Error: Invalid fen string\n";
        return 1;
    }

    std::vector<EncodedMove> legal_moves;
    legal_moves.reserve(256);

    const auto is_legal_move = [&](const EncodedMove move) {
        return std::find(legal_moves.begin(), legal_moves.end(), move) != legal_moves.end();
    };
    for (const auto &move_string : args->moves) {
        const auto parsed_move { parse_move_input(move_string, *board) };
        if (!parsed_move.has_value()) {
            std::cerr << "Error: move input \"" << move_string << "\" is invalid\n";
            return 1;
        }

        MoveGen(legal_moves, *board, at).gen();
        if (is_legal_move(*parsed_move)) {
            board->make_move(*parsed_move);
        } else {
            std::cerr << "Error: move \"" << move_string << "\" is not a legal move\n";
            return 1;
        }
    }
}
