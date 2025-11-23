#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

using Board = uint64_t;

constexpr Board RANK_MASK = 0xFF;
constexpr Board FILE_MASK = 0x0101010101010101;
constexpr Board DIAGONAL_LEFT_MASK = 0x8040201008040201;
constexpr Board DIAGONAL_RIGHT_MASK = 0x0102040810204080;

const Board DIAGONAL_LEFT_MASKS[15] = {
    DIAGONAL_LEFT_MASK << 56, DIAGONAL_LEFT_MASK << 48,
    DIAGONAL_LEFT_MASK << 40, DIAGONAL_LEFT_MASK << 32,
    DIAGONAL_LEFT_MASK << 24, DIAGONAL_LEFT_MASK << 16,
    DIAGONAL_LEFT_MASK << 8,  DIAGONAL_LEFT_MASK,
    DIAGONAL_LEFT_MASK >> 8,  DIAGONAL_LEFT_MASK >> 16,
    DIAGONAL_LEFT_MASK >> 24, DIAGONAL_LEFT_MASK >> 32,
    DIAGONAL_LEFT_MASK >> 40, DIAGONAL_LEFT_MASK >> 48,
    DIAGONAL_LEFT_MASK >> 56,
};

const Board DIAGONAL_RIGHT_MASKS[15] = {
    DIAGONAL_RIGHT_MASK << 56, DIAGONAL_RIGHT_MASK << 48,
    DIAGONAL_RIGHT_MASK << 40, DIAGONAL_RIGHT_MASK << 32,
    DIAGONAL_RIGHT_MASK << 24, DIAGONAL_RIGHT_MASK << 16,
    DIAGONAL_RIGHT_MASK << 8,  DIAGONAL_RIGHT_MASK,
    DIAGONAL_RIGHT_MASK >> 8,  DIAGONAL_RIGHT_MASK >> 16,
    DIAGONAL_RIGHT_MASK >> 24, DIAGONAL_RIGHT_MASK >> 32,
    DIAGONAL_RIGHT_MASK >> 40, DIAGONAL_RIGHT_MASK >> 48,
    DIAGONAL_RIGHT_MASK >> 56,
};

std::string board_format(Board board) {
    std::string fmt = "   0 1 2 3 4 5 6 7\n   ----------------";

    for (uint8_t i = 0; i < 8; ++i) {
        fmt += "\n" + std::to_string(i) + "|";

        for (uint8_t j = 0; j < 8; ++j) {
            fmt += " ";
            fmt += (board >> ((8 * i) + j)) & 1 ? "#" : ".";
        }
    }

    return fmt + "\n";
}

//
Board board_calc_attacks(uint8_t pos) {
    assert(pos < 64);

    uint8_t rank = pos >> 3;
    uint8_t file = pos & 7;
    Board rank_board = RANK_MASK << (rank << 3);
    Board file_board = FILE_MASK << file;
    Board left_diagonal_board = DIAGONAL_LEFT_MASKS[7 - rank + file];
    Board right_diagonal_board = DIAGONAL_RIGHT_MASKS[14 - (rank + file)];

    return rank_board | file_board | left_diagonal_board | right_diagonal_board;
}

int main() {
    auto time_start = std::chrono::high_resolution_clock::now();

    std::vector<Board> attacks_src;
    std::vector<Board> attacks_dest;

    attacks_src.reserve(16349760);
    attacks_dest.reserve(16349760);

    std::vector<Board> boards_src;
    std::vector<Board> boards_dest;

    boards_src.reserve(16349760);
    boards_dest.reserve(16349760);

    for (uint8_t i = 0; i < 64; ++i) {
        attacks_src.push_back(board_calc_attacks(i));
        boards_src.push_back((Board)1 << i);
    }

    while (attacks_src.size() != 0) {
        attacks_dest.clear();
        boards_dest.clear();

        for (size_t i = 0; i < attacks_src.size(); ++i) {
            Board attack = attacks_src[i];
            Board board = boards_src[i];

            for (uint8_t j = 0; j < 64; ++j) {
                if (!(attack & ((Board)1 << j))) {
                    attacks_dest.push_back(attack | board_calc_attacks(j));
                    boards_dest.push_back(board | ((Board)1 << j));
                }
            }
        }

        std::swap(attacks_src, attacks_dest);
        std::swap(boards_src, boards_dest);
    }

    std::sort(boards_dest.begin(), boards_dest.end());
    std::vector<Board>::iterator boards_dest_uiter =
        std::unique(boards_dest.begin(), boards_dest.end());

    boards_dest.resize(std::distance(boards_dest.begin(), boards_dest_uiter));

    auto time_end = std::chrono::high_resolution_clock::now();
    double time = std::chrono::duration_cast<std::chrono::nanoseconds>(
                      time_end - time_start)
                      .count();

    time *= 1e-9;

    for (const Board board : boards_dest) {
        std::cout << board_format(board);
    }

    std::cout << "Execution time (s) -> " << time << "\n";
    std::cout << "Number of possible boards -> " << boards_dest.size() << "\n";

    return 0;
}
