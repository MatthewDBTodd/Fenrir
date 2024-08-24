#include "pawn.h"
#include "king.h"
#include "knight.h"
#include "sliding_piece.h"

#include <iostream>
#include "fenrir_assert.h"
#include <chrono>

int main() {
    const auto t0 { std::chrono::steady_clock::now() };
    const SlidingPieceAttacks attacks;
    const auto t1 { std::chrono::steady_clock::now() };
    const auto taken { std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count() };
    std::cout << "Took " << taken/1000 << "." << taken%1000 << " seconds\n";

    const auto t2 { std::chrono::steady_clock::now() };
    constexpr piece::PawnAttackTable pawns;
    constexpr auto knights { piece::generate_knight_att_squares() };
    constexpr auto kings { piece::generate_king_att_squares() } ;
    (void)knights;
    (void)kings;
    (void)pawns;
    const auto t3 { std::chrono::steady_clock::now() };
    const auto taken2 { std::chrono::duration_cast<std::chrono::milliseconds>(t2-t3).count() };
    std::cout << "Took " << taken2 / 1000 << "." << taken2 % 1000 << " seconds\n";
}