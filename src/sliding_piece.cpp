#include "sliding_piece.h"
#include "direction.h"
#include "subset_iterator.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <limits>
#include <random>
#include <utility>

/*
 * ROOK_MASKS and BISHOP_MASKS are attacks in all that pieces attack directions
 * from each source square. The edges of the boards are all zeroed out
 */
static constexpr std::array<std::uint64_t, NUM_SQUARES> ROOK_MASKS {
    0x000101010101017E, 0x000202020202027C, 0x000404040404047A, 0x0008080808080876,
    0x001010101010106E, 0x002020202020205E, 0x004040404040403E, 0x008080808080807E,
    0x0001010101017E00, 0x0002020202027C00, 0x0004040404047A00, 0x0008080808087600,
    0x0010101010106E00, 0x0020202020205E00, 0x0040404040403E00, 0x0080808080807E00,
    0x00010101017E0100, 0x00020202027C0200, 0x00040404047A0400, 0x0008080808760800,
    0x00101010106E1000, 0x00202020205E2000, 0x00404040403E4000, 0x00808080807E8000,
    0x000101017E010100, 0x000202027C020200, 0x000404047A040400, 0x0008080876080800,
    0x001010106E101000, 0x002020205E202000, 0x004040403E404000, 0x008080807E808000,
    0x0001017E01010100, 0x0002027C02020200, 0x0004047A04040400, 0x0008087608080800,
    0x0010106E10101000, 0x0020205E20202000, 0x0040403E40404000, 0x0080807E80808000,
    0x00017E0101010100, 0x00027C0202020200, 0x00047A0404040400, 0x0008760808080800,
    0x00106E1010101000, 0x00205E2020202000, 0x00403E4040404000, 0x00807E8080808000,
    0x007E010101010100, 0x007C020202020200, 0x007A040404040400, 0x0076080808080800,
    0x006E101010101000, 0x005E202020202000, 0x003E404040404000, 0x007E808080808000,
    0x7E01010101010100, 0x7C02020202020200, 0x7A04040404040400, 0x7608080808080800,
    0x6E10101010101000, 0x5E20202020202000, 0x3E40404040404000, 0x7E80808080808000,
};

static constexpr std::array<std::uint64_t, NUM_SQUARES> BISHOP_MASKS {
    0x0040201008040200, 0x0000402010080400, 0x0000004020100A00, 0x0000000040221400,
    0x0000000002442800, 0x0000000204085000, 0x0000020408102000, 0x0002040810204000,
    0x0020100804020000, 0x0040201008040000, 0x00004020100A0000, 0x0000004022140000,
    0x0000000244280000, 0x0000020408500000, 0x0002040810200000, 0x0004081020400000,
    0x0010080402000200, 0x0020100804000400, 0x004020100A000A00, 0x0000402214001400,
    0x0000024428002800, 0x0002040850005000, 0x0004081020002000, 0x0008102040004000,
    0x0008040200020400, 0x0010080400040800, 0x0020100A000A1000, 0x0040221400142200,
    0x0002442800284400, 0x0004085000500800, 0x0008102000201000, 0x0010204000402000,
    0x0004020002040800, 0x0008040004081000, 0x00100A000A102000, 0x0022140014224000,
    0x0044280028440200, 0x0008500050080400, 0x0010200020100800, 0x0020400040201000,
    0x0002000204081000, 0x0004000408102000, 0x000A000A10204000, 0x0014001422400000,
    0x0028002844020000, 0x0050005008040200, 0x0020002010080400, 0x0040004020100800,
    0x0000020408102000, 0x0000040810204000, 0x00000A1020400000, 0x0000142240000000,
    0x0000284402000000, 0x0000500804020000, 0x0000201008040200, 0x0000402010080400,
    0x0002040810204000, 0x0004081020400000, 0x000A102040000000, 0x0014224000000000,
    0x0028440200000000, 0x0050080402000000, 0x0020100804020000, 0x0040201008040200,
};

static std::uint64_t sparse_random() {
    static std::random_device rd {};
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<std::uint64_t> dist(
        0, std::numeric_limits<std::uint64_t>::max()
    );
    return dist(gen) & dist(gen) & dist(gen);
}

static constexpr std::size_t index_from_blockers(
    const std::uint64_t blockers, const std::uint64_t magic, const int shift
) {
    return static_cast<std::size_t>((blockers * magic) >> shift);
}

Magic::Magic(const std::uint64_t magic, const std::uint64_t mask, const int shift,
             std::vector<std::uint64_t> attacks) :
    magic(magic),
    mask(mask),
    shift(shift),
    attacks(std::move(attacks))
{}

std::uint64_t Magic::get_attacks(const std::uint64_t occupied_unmasked) const {
    const std::uint64_t occupied_masked { occupied_unmasked & mask };
    const std::size_t idx { index_from_blockers(occupied_masked, magic, shift) };
    return attacks[idx];
}

// Given an origin square, a piece, and a blocker mask, calculate its attack squares
static std::uint64_t square_blockers_attacks(const std::uint64_t square_mask, 
                                             const Piece piece,
                                             const std::uint64_t blockers) {

    std::uint64_t result { 0 };

    using direction_fn = std::uint64_t (*)(const std::uint64_t) noexcept;
    using directions = std::array<direction_fn, 4>;
    const directions dirs = [](const Piece piece) -> directions {
        if (piece == ROOK) {
            return {
                direction::north,
                direction::south,
                direction::east,
                direction::west
            };
        } else {
            return {
                direction::north_east,
                direction::north_west,
                direction::south_east,
                direction::south_west
            };
        }
    } (piece);

    for (direction_fn dir : dirs) {
        std::uint64_t tmp_mask { square_mask };
        // north/south shifts off the edge so will be zero. East/west is also fine
        // as the blocker constants zero out the board edges so the break condition
        // will be met.
        while (tmp_mask != 0) {
            tmp_mask = dir(tmp_mask);
            result |= tmp_mask;

            // ignore everything behind the first blocker
            if (tmp_mask & blockers) {
                break;
            }
        }
    }
    return result;
}

struct SquareAttackPermutation {
    const std::uint64_t blocker_mask;
    const std::uint64_t attack_mask;
};

std::vector<SquareAttackPermutation> square_attack_permutations(
    const std::uint64_t square_mask,
    const Piece piece,
    const std::uint64_t blockers
) {
    const int pop_count { std::popcount(blockers) };
    std::vector<SquareAttackPermutation> rv;
    rv.reserve(1 << pop_count);
    const auto subset_it = utility::Subsets(blockers);
    std::transform(subset_it.begin(), subset_it.end(), std::back_inserter(rv),
      [square_mask, piece] (const std::uint64_t blocker_permutation) {
        return SquareAttackPermutation {
            blocker_permutation, 
            square_blockers_attacks(square_mask, piece, blocker_permutation)};
    });
    return rv;
}

Magic Magic::init(const Square square, const Piece piece) {
    assert(piece == ROOK || piece == BISHOP);
    const std::uint64_t sq_mask { 1ul << square };
    const std::uint64_t raw_block_mask {
        piece == ROOK ? ROOK_MASKS[square] : BISHOP_MASKS[square]
    };
    const std::vector<SquareAttackPermutation> permutations {
        square_attack_permutations(sq_mask, piece, raw_block_mask)
    };
    std::vector<std::uint64_t> attacks(permutations.size());
    const int shift { 64 - std::popcount(raw_block_mask) };
    std::uint64_t magic {};
    while (true) {
        magic = sparse_random();
        bool failed { false };
        for (const SquareAttackPermutation permutation : permutations) {
            const std::size_t idx { index_from_blockers(permutation.blocker_mask,
                                                        magic, shift) };
            
            // No collision
            if (attacks[idx] == 0) {
                attacks[idx] = permutation.attack_mask;
            } else if (attacks[idx] == permutation.attack_mask) {
                // We don't care about collisions if they map to the same result
                continue;
            } else {
                // collision, try again :(
                std::fill(attacks.begin(), attacks.end(), 0ull);
                failed = true;
                break;
            }
        }
        if (!failed) {
            break;
        }
    }
    return Magic(magic, raw_block_mask, shift, attacks);
}

template <std::size_t... Ns>
static std::array<Magic, sizeof...(Ns)> make_magic(const Piece piece, 
                                                   std::index_sequence<Ns...>) {
    return { (Magic::init(static_cast<Square>(Ns), piece))... };
}

static std::array<Magic, NUM_SQUARES> init_magics(const Piece piece) {
    return make_magic(piece, std::make_index_sequence<NUM_SQUARES>());
}

SlidingPieceAttacks::SlidingPieceAttacks() :
    rook_attacks(init_magics(ROOK)),
    bishop_attacks(init_magics(BISHOP))
{}

std::uint64_t SlidingPieceAttacks::lookup(const Square square, const Piece piece,
                                          const std::uint64_t blockers) const {
    assert(piece == BISHOP || piece == ROOK || piece == QUEEN);
    if (piece == BISHOP) {
        return bishop_attacks[square].get_attacks(blockers);
    } else if (piece == ROOK) {
        return rook_attacks[square].get_attacks(blockers);
    } else {
        return bishop_attacks[square].get_attacks(blockers)
             | rook_attacks[square].get_attacks(blockers);
    }
}