#include <benchmark/benchmark.h>

#include "attack_table.h"
#include "board.h"
#include "decoded_move.h"
#include "move_gen.h"

#include <vector>
#include <string_view>
#include <utility>

static void BM_board_copy(benchmark::State &state) {
    Board board { *Board::init() };
    for (auto _ : state) {
        Board b { board };
        benchmark::DoNotOptimize(b);
    }
}

static void BM_board_make_double_pawn_push(benchmark::State &state) {
    Board board { *Board::init() };
    for (auto _ : state) {
        Board b { board };
        b.make_move(move_type_v::DoublePawnPush {
            move_type_v::Common {
                E2, E4, PAWN, WHITE
            },
            E3
        });
    }
}

static void BM_board_make_quiet(benchmark::State &state) {
    Board board { *Board::init() };
    for (auto _ : state) {
        Board b { board };
        b.make_move(move_type_v::Quiet {
            move_type_v::Common {
                G1, F3, KNIGHT, WHITE
            }
        });
    }
}

static void BM_board_make_capture(benchmark::State &state) {
    Board board { *Board::init("r1bqkbnr/1ppp1ppp/p1B5/4p3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 0 4") };
    for (auto _ : state) {
        Board b { board };
        b.make_move(move_type_v::Capture {
            move_type_v::Common {
                D7, C6, PAWN, BLACK
            },
            BISHOP
        });
    }
}

static void BM_board_make_castle_kingside(benchmark::State &state) {
    Board board { *Board::init("r1bqkbnr/1pp2ppp/p1p5/4p3/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 5") };
    for (auto _ : state) {
        Board b { board };
        b.make_move(move_type_v::CastleKingSide {
            move_type_v::Common {
                D1, G1, KING, WHITE
            }
        });
    }
}

static void BM_board_make_castle_queenside(benchmark::State &state) {
    Board board { *Board::init("r3kbnr/1pp1qppp/p1p5/4p3/4P1b1/2N2N2/PPPP1PPP/R1BQR1K1 b kq - 5 7") };
    for (auto _ : state) {
        Board b { board };
        b.make_move(move_type_v::CastleQueenSide {
            move_type_v::Common {
                E8, C8, KING, BLACK 
            }
        });
    }
}

static void BM_board_make_enpassant(benchmark::State &state) {
    Board board { *Board::init("r1bqkbnr/ppppp1pp/2n5/4Pp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3") };
    for (auto _ : state) {
        Board b { board };
        b.make_move(move_type_v::EnPassant {
            move_type_v::Common {
                E5, F6, PAWN, WHITE
            },
            F5
        });
    }
}

static void BM_board_make_move_promotion(benchmark::State &state) {
    Board board { *Board::init("r1bqkb1r/pppp2Pp/2n4n/4p3/8/8/PPPP1PPP/RNBQKBNR w KQkq - 0 5") };
    for (auto _ : state) {
        Board b { board };
        b.make_move(move_type_v::MovePromotion {
            move_type_v::Common {
                G7, G8, PAWN, WHITE
            },
            QUEEN
        });
    }
}

static void BM_board_make_capture_promotion(benchmark::State &state) {
    Board board { *Board::init("rnbqkbnr/ppp1pppp/8/8/3P1B2/2N5/PPP3pP/R2QKBNR b KQkq - 1 5") };
    for (auto _ : state) {
        Board b { board };
        b.make_move(move_type_v::CapturePromotion {
            move_type_v::Common {
                G2, H1, PAWN, BLACK
            },
            ROOK, QUEEN
        });
    }
}

static void BM_board_undo_double_pawn_push(benchmark::State &state) {
    Board board { *Board::init() };
    board.make_move(move_type_v::DoublePawnPush {
        move_type_v::Common {
            E2, E4, PAWN, WHITE
        },
        E3
    });
    for (auto _ : state) {
        Board b { board };
        b.undo_last_move();
    }
}

static void BM_board_undo_quiet(benchmark::State &state) {
    Board board { *Board::init() };
    board.make_move(move_type_v::Quiet {
        move_type_v::Common {
            G1, F3, KNIGHT, WHITE
        }
    });
    for (auto _ : state) {
        Board b { board };
        b.undo_last_move();
    }
}

static void BM_board_undo_capture(benchmark::State &state) {
    Board board { *Board::init("r1bqkbnr/1ppp1ppp/p1B5/4p3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 0 4") };
    board.make_move(move_type_v::Capture {
        move_type_v::Common {
            D7, C6, PAWN, BLACK
        },
        BISHOP
    });
    for (auto _ : state) {
        Board b { board };
        b.undo_last_move();
    }
}

static void BM_board_undo_castle_kingside(benchmark::State &state) {
    Board board { *Board::init("r1bqkbnr/1pp2ppp/p1p5/4p3/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 5") };
    board.make_move(move_type_v::CastleKingSide {
        move_type_v::Common {
            D1, G1, KING, WHITE
        }
    });
    for (auto _ : state) {
        Board b { board };
        b.undo_last_move();
    }
}

static void BM_board_gen_moves(benchmark::State &state) {
    const AttackTable at {};
    Board board { *Board::init("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -") };
    std::vector<EncodedMove> moves;
    moves.reserve(256);
    for (auto _ : state) {
        Board b { board };
        MoveGen(moves, b, at).gen();
    }
}

BENCHMARK(BM_board_copy);
BENCHMARK(BM_board_make_double_pawn_push);
BENCHMARK(BM_board_make_quiet);
BENCHMARK(BM_board_make_capture);
BENCHMARK(BM_board_make_castle_kingside);
BENCHMARK(BM_board_make_castle_queenside);
BENCHMARK(BM_board_make_enpassant);
BENCHMARK(BM_board_make_move_promotion);
BENCHMARK(BM_board_make_capture_promotion);
BENCHMARK(BM_board_undo_double_pawn_push);
BENCHMARK(BM_board_undo_quiet);
BENCHMARK(BM_board_undo_capture);
BENCHMARK(BM_board_undo_castle_kingside);
BENCHMARK(BM_board_gen_moves);

BENCHMARK_MAIN();