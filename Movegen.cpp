#include "Movegen.h"

#include <assert.h>
#include <stdlib.h>

#include "bitfunc.h"
#include "Board.h"
#include "directions.h"

// ctor
Movegen::Movegen(Board *board) {
  this->board = board;

  initmagicmoves();
  initBetweenTable();

  pinned_pieces = 0;
}

void Movegen::initBetweenTable() {
  for (int from = 0; from != 64; ++from) {
    for (int to = 0; to != 64; ++to) {
      bitboard between_sqs = 0;
      int tmp_from = from;
      int tmp_to = to;
      // always move from smaller to bigger square
      if (tmp_from < tmp_to) {
        tmp_to = from;
        tmp_from = to;
      }

      // moving vertical
      if ((from - to) % NORT == 0) {
        tmp_to += NORT;
        while (tmp_to < tmp_from) {
          between_sqs |= ((bitboard)1 << (tmp_to));
          tmp_to += NORT;
        }
      }

      // moving diagonal (NOEA)
      if ((from - to) % NOEA == 0) {
        tmp_to += NOEA;
        while (tmp_to < tmp_from) {
          between_sqs |= ((bitboard)1 << (tmp_to));
          tmp_to += NOEA;
        }
      }

      // moving diagonal (NOWE)
      if ((from - to) % NOWE == 0) {
        tmp_to += NOWE;
        while (tmp_to < tmp_from) {
          between_sqs |= ((bitboard)1 << (tmp_to));
          tmp_to += NOWE;
        }
      }
      between[from][to] = between_sqs;
    }
    // horizontal moves

    bitboard between_sqs = 0;
    square leftborder = from - (from % 8);
    square rightborder = from + (7 - (from % 8));
    int to = from - 1;
    while (to > leftborder) {
      between_sqs |= ((bitboard)1 << (to));
      to--;
      between[from][to] = between_sqs;
    }
    between_sqs = 0;
    to = from + 1;
    while (to < rightborder) {
      between_sqs |= ((bitboard)1 << (to));
      to++;
      between[from][to] = between_sqs;
    }
  }
}

// encodes a move
inline Move Movegen::encodeMove(const square &from, const square &to,
                                const Piece &piece,
                                const Piece &capture = EMPTY,
                                const Piece &promotion = EMPTY,
                                const square &EP = 0,
                                const Movetype &type = NONE) {
  return encodeFrom(from) | encodeTo(to) | encodePiece(piece) |
         encodeCapture(capture) | encodePromotion(promotion) | encodeEP(EP) |
         encodeMovetype(type);
}

void Movegen::genAllMoves(Movelist &list) {
  pinned_pieces = pinnedPieces(board->us);
  if (!board->in_check) {
    if (board->us == WHITE) {
      genWhitePawnMoves(list);
    } else {
      genBlackPawnMoves(list);
    }
    genQueenMoves(list);
    genRookMoves(list);
    genBishopMoves(list);
    genKnightMoves(list);
    genKingMoves(list);
    genCastles(list);
    // add en passants
    for (int i = 0; i != 2; ++i) {
      if (board->en_passant[i] != NULLMOVE) {
        list.moves[list.count++] = board->en_passant[i];
      }
    }
  } else {
    genEvades(list, board->us);
  }
}

void Movegen::genCastles(Movelist &list) {
  if (board->us == WHITE) {
    if (board->castling_rights[WKINGSIDE]) {
      if (board->squares[5] == EMPTY && board->squares[6] == EMPTY) {
        if (!board->isAttacked(board->us, 5) &&
            !board->isAttacked(board->us, 6)) {
          list.moves[list.count++] = board->castle_moves[WKINGSIDE];
        }
      }
    }
    if (board->castling_rights[WQUEENSIDE]) {
      if (board->squares[1] == EMPTY && board->squares[2] == EMPTY &&
          board->squares[3] == EMPTY) {
        if (!board->isAttacked(board->us, 2) &&
            !board->isAttacked(board->us, 3)) {
          list.moves[list.count++] = board->castle_moves[WQUEENSIDE];
        }
      }
    }
  }
  if (board->us == BLACK) {
    if (board->castling_rights[BKINGSIDE]) {
      if (board->squares[61] == EMPTY && board->squares[62] == EMPTY) {
        if (!board->isAttacked(board->us, 61) &&
            !board->isAttacked(board->us, 62)) {
          list.moves[list.count++] = board->castle_moves[BKINGSIDE];
        }
      }
    }
    if (board->castling_rights[BQUEENSIDE]) {
      if (board->squares[57] == EMPTY && board->squares[58] == EMPTY &&
          board->squares[59] == EMPTY) {
        if (!board->isAttacked(board->us, 58) &&
            !board->isAttacked(board->us, 59)) {
          list.moves[list.count++] = board->castle_moves[BQUEENSIDE];
        }
      }
    }
  }
}

void Movegen::genEvades(Movelist &list, const Color &color) {
  assert(board->getKings(color));
  square king_sq = bitScanForward(board->getKings(color));
  bitboard moves = board->king_moves[king_sq] & ~board->getAllPieces(color);

  while (moves) {
    square to = pop_lsb(moves);
    if (!board->isAttacked(color, to)) {
      list.moves[list.count++] =
          encodeMove(king_sq, to, KING, board->squares[to]);
    }
  }
  bitboard attackers = getAttackers(color, king_sq);
  // if there is only one attacker, we can get out of check by taking the
  // attacker,
  // or by moving a piece between the king and the attacker

  if (popCount(attackers) == 1) {
    square attacker_sq = pop_lsb(attackers);
    // capture the attacking piece
    bitboard defenders =
        getAttackers(board->them, attacker_sq) & ~board->getKings(color);

    while (defenders) {
      square from = pop_lsb(defenders);
      Piece defender = board->squares[from];

      if (defender == PAWN) {
        // check for a promotion
        if (attacker_sq > 55 || attacker_sq < 8) {
          genPromotion(list, from, attacker_sq);
        } else {
          list.moves[list.count++] = encodeMove(from, attacker_sq, defender,
                                                board->squares[attacker_sq]);
        }
      }
      if (defender != PAWN) {
        list.moves[list.count++] = encodeMove(from, attacker_sq, defender,
                                              board->squares[attacker_sq]);
      }
    }

    Piece attacker_piece = board->squares[attacker_sq];
    if (attacker_piece == QUEEN || attacker_piece == BISHOP ||
        attacker_piece == ROOK) {
      bitboard interposing_sqs = between[king_sq][attacker_sq];
      while (interposing_sqs) {
        square to = pop_lsb(interposing_sqs);
        bitboard interposing_pieces =
            getMovers(color, to) & ~board->getKings(color) & ~pinned_pieces;
        while (interposing_pieces) {
          square from = pop_lsb(interposing_pieces);
          Piece interposing_piece = board->squares[from];
          if (interposing_piece == PAWN) {
            if (to > 55 || to < 8) {
              genPromotion(list, from, to);
            } else {
              list.moves[list.count++] =
                  encodeMove(from, to, interposing_piece, board->squares[to]);
            }
          }
          if (interposing_piece != PAWN) {
            list.moves[list.count++] =
                encodeMove(from, to, interposing_piece, board->squares[to]);
          }
        }
      }
    }
    for (int i = 0; i != 2; ++i) {
      if (extractEP(board->en_passant[i]) == attacker_sq &&
          board->en_passant[i] != NULLMOVE) {
        list.moves[list.count++] = board->en_passant[i];
      }
    }
  }
}

// generate pseudolegal moves for all white pawns
void Movegen::genWhitePawnMoves(Movelist &list) {
  bitboard white_pawns = board->getPawns(WHITE);
  bitboard white_pawn_push_fst = (white_pawns << NORT) & ~board->getAllPieces();
  // if a piece got to rank 3 from rank 2, check if its possible to move again
  bitboard white_pawn_push_snd =
      ((white_pawn_push_fst & RANK_3) << NORT) & ~board->getAllPieces();

  bitboard white_pawn_left_attack = (white_pawns & CLEAR_FILE_A) << NOWE;
  bitboard white_pawn_right_attack = (white_pawns & CLEAR_FILE_H) << NOEA;
  // only Attack occupied squares
  white_pawn_left_attack = white_pawn_left_attack & board->getBlackPieces();
  white_pawn_right_attack = white_pawn_right_attack & board->getBlackPieces();

  // spool Moves to the movelist
  while (white_pawn_push_fst) {
    square to = pop_lsb(white_pawn_push_fst);
    if (to > 55) {
      genPromotion(list, to - NORT, to);
    } else {
      list.moves[list.count++] = encodeMove(to - NORT, to, PAWN);
    }
  }

  while (white_pawn_push_snd) {
    square to = pop_lsb(white_pawn_push_snd);
    list.moves[list.count++] = encodeMove(to - NORT * 2, to, PAWN);
  }

  while (white_pawn_left_attack) {
    square to = pop_lsb(white_pawn_left_attack);
    if (to > 55) {
      genPromotion(list, to - NOWE, to);
    } else {
      list.moves[list.count++] =
          encodeMove(to - NOWE, to, PAWN, board->squares[to]);
    }
  }

  while (white_pawn_right_attack) {
    square to = pop_lsb(white_pawn_right_attack);
    if (to > 55) {
      genPromotion(list, to - NOEA, to);
    } else {
      list.moves[list.count++] =
          encodeMove(to - NOEA, to, PAWN, board->squares[to]);
    }
  }
}

// generate pseudolegal moves for all black pawns, using similiar logic to the
// white pawns, but shifting the bitboards to the right.
void Movegen::genBlackPawnMoves(Movelist &list) {
  bitboard black_pawns = board->getPawns(BLACK);
  bitboard black_pawn_push_fst = (black_pawns >> SOUT) & ~board->getAllPieces();
  // if a piece got to rank 3 from rank 2, check if its possible to move again
  bitboard black_pawn_push_snd =
      ((black_pawn_push_fst & RANK_6) >> SOUT) & ~board->getAllPieces();

  bitboard black_pawn_left_attack = (black_pawns & CLEAR_FILE_A) >> SOWE;
  bitboard black_pawn_right_attack = (black_pawns & CLEAR_FILE_H) >> SOEA;
  // only Attack occupied squares
  black_pawn_left_attack = black_pawn_left_attack & board->getWhitePieces();
  black_pawn_right_attack = black_pawn_right_attack & board->getWhitePieces();

  // spool Moves to the movelist
  while (black_pawn_push_fst) {
    square to = pop_lsb(black_pawn_push_fst);
    if (to < 8) {
      genPromotion(list, to + SOUT, to);
    } else {
      list.moves[list.count++] = encodeMove(to + SOUT, to, PAWN);
    }
  }
  while (black_pawn_push_snd) {
    square to = pop_lsb(black_pawn_push_snd);
    list.moves[list.count++] = encodeMove(to + SOUT * 2, to, PAWN);
  }

  while (black_pawn_left_attack) {
    square to = pop_lsb(black_pawn_left_attack);
    if (to < 8) {
      genPromotion(list, to + SOWE, to);
    } else {
      list.moves[list.count++] =
          encodeMove(to + SOWE, to, PAWN, board->squares[to]);
    }
  }

  while (black_pawn_right_attack) {
    square to = pop_lsb(black_pawn_right_attack);
    if (to < 8) {
      genPromotion(list, to + SOEA, to);
    } else {
      list.moves[list.count++] =
          encodeMove(to + SOEA, to, PAWN, board->squares[to]);
    }
  }
}

void Movegen::genPromotion(Movelist &list, const square &from,
                           const square &to) {
  list.moves[list.count++] =
      encodeMove(from, to, PAWN, board->squares[to], KNIGHT);
  list.moves[list.count++] =
      encodeMove(from, to, PAWN, board->squares[to], BISHOP);
  list.moves[list.count++] =
      encodeMove(from, to, PAWN, board->squares[to], ROOK);
  list.moves[list.count++] =
      encodeMove(from, to, PAWN, board->squares[to], QUEEN);
}

// generate pseudolegal moves for the Bishops of one color, using BuzzOS magic
// bitboard implementation
void Movegen::genBishopMoves(Movelist &list) {
  bitboard bishops = board->getBishops(board->us);
  while (bishops) {
    square from = pop_lsb(bishops);
    bitboard moves =
        Bmagic(from, board->getAllPieces()) & ~board->getAllPieces(board->us);
    while (moves) {
      square to = pop_lsb(moves);
      list.moves[list.count++] =
          encodeMove(from, to, BISHOP, board->squares[to]);
    }
  }
}

// generate legal moves for the king of one color
void Movegen::genKingMoves(Movelist &list) {
  square from = bitScanForward(board->getKings(board->us));
  bitboard moves = board->king_moves[from] & ~board->getAllPieces(board->us);
  while (moves) {
    square to = pop_lsb(moves);
    if (!board->isAttacked(board->us, to)) {
      list.moves[list.count++] = encodeMove(from, to, KING, board->squares[to]);
    }
  }
}

// generate pseudolegal moves for all knights of one color
void Movegen::genKnightMoves(Movelist &list) {
  bitboard knights = board->getKnights(board->us);
  while (knights) {
    square from = pop_lsb(knights);
    bitboard moves =
        board->knight_moves[from] & ~board->getAllPieces(board->us);
    while (moves) {
      square to = pop_lsb(moves);
      list.moves[list.count++] =
          encodeMove(from, to, KNIGHT, board->squares[to]);
    }
  }
}

// generate pseudolegal moves for the Rooks of one color, using BuzzOS magic
// bitboard implementation
void Movegen::genRookMoves(Movelist &list) {
  bitboard rooks = board->getRooks(board->us);
  while (rooks) {
    square from = pop_lsb(rooks);
    bitboard moves =
        Rmagic(from, board->getAllPieces()) & ~board->getAllPieces(board->us);
    while (moves) {
      square to = pop_lsb(moves);
      list.moves[list.count++] = encodeMove(from, to, ROOK, board->squares[to]);
    }
  }
}

// generate pseudolegal moves for the Queen of one color, using BuzzOS magic
// bitboard implementation
void Movegen::genQueenMoves(Movelist &list) {
  bitboard queen = board->getQueens(board->us);
  while (queen) {
    square from = pop_lsb(queen);
    bitboard moves = (Bmagic(from, board->getAllPieces()) |
                      Rmagic(from, board->getAllPieces())) &
                     ~board->getAllPieces(board->us);
    while (moves) {
      square to = pop_lsb(moves);
      list.moves[list.count++] =
          encodeMove(from, to, QUEEN, board->squares[to]);
    }
  }
}

// returns a bitboard containing all the pieces of a color that can move to a
// square (to include pawn pushes)
bitboard Movegen::getMovers(const Color &color, const square &sq) {
  return getAttackers(board->flipColor(color), sq) | getPawnMovers(color, sq);
}

// returns all pawns that can move to a square
bitboard Movegen::getPawnMovers(const Color &color, const square &sq) {
  bitboard pawns = 0;
  bitboard pawn_push_fst = 0;
  bitboard pawn_push_snd = 0;

  if (color == WHITE) {
    pawns = board->getPawns(WHITE);
    pawn_push_fst = (pawns << NORT) & ~board->getAllPieces();
    if (pawn_push_fst & board->square_mask[sq]) {
      return board->square_mask[sq - NORT];
    }
    // if a piece got to rank 3 from rank 2, check if its possible to move again
    pawn_push_snd = ((pawn_push_fst & RANK_3) << NORT) & ~board->getAllPieces();
    if (pawn_push_snd & board->square_mask[sq]) {
      return board->square_mask[sq - NORT * 2];
    }
  }

  if (color == BLACK) {
    pawns = board->getPawns(BLACK);
    pawn_push_fst = (pawns >> SOUT) & ~board->getAllPieces();
    if (pawn_push_fst & board->square_mask[sq]) {
      return board->square_mask[sq + NORT];
    }
    // if a piece got to rank 3 from rank 2, check if its possible to move again
    pawn_push_snd = ((pawn_push_fst & RANK_6) >> SOUT) & ~board->getAllPieces();
    if (pawn_push_snd & board->square_mask[sq]) {
      return board->square_mask[sq + NORT * 2];
    }
  }
  return 0;
}

bitboard Movegen::getAttackers(const Color &color, const square &sq) {
  bitboard attackset = 0;

  if (color == BLACK) {
    bitboard white_pawn_left_attack = (board->getPawns(WHITE) & CLEAR_FILE_A)
                                      << NOWE;
    bitboard white_pawn_right_attack = (board->getPawns(WHITE) & CLEAR_FILE_H)
                                       << NOEA;
    // only Attack occupied squares
    white_pawn_left_attack = white_pawn_left_attack & board->getBlackPieces();
    white_pawn_right_attack = white_pawn_right_attack & board->getBlackPieces();
    // add Piece to attackset by shifting back
    if (board->square_mask[sq] & white_pawn_left_attack) {
      attackset |= (board->square_mask[sq] & CLEAR_FILE_H) >> SOEA;
    }
    if (board->square_mask[sq] & white_pawn_right_attack) {
      attackset |= (board->square_mask[sq] & CLEAR_FILE_A) >> SOWE;
    }
  }

  if (color == WHITE) {
    bitboard black_pawn_left_attack =
        (board->getPawns(BLACK) & CLEAR_FILE_A) >> SOWE;
    bitboard black_pawn_right_attack =
        (board->getPawns(BLACK) & CLEAR_FILE_H) >> SOEA;
    // only Attack occupied squares
    black_pawn_left_attack = black_pawn_left_attack & board->getWhitePieces();
    black_pawn_right_attack = black_pawn_right_attack & board->getWhitePieces();
    if (board->square_mask[sq] & black_pawn_left_attack) {
      attackset |= (board->square_mask[sq] & CLEAR_FILE_H) << NOEA;
    }
    if (board->square_mask[sq] & black_pawn_right_attack) {
      attackset |= (board->square_mask[sq] & CLEAR_FILE_A) << NOWE;
    }
  }

  attackset |=
      (board->knight_moves[sq] & board->getKnights(board->flipColor(color)));

  // bishop,queen attacks
  bitboard sliding_attacks =
      Bmagic(sq, board->getAllPieces()) & ~board->getAllPieces(color);
  attackset |= (sliding_attacks & (board->getBishops(board->flipColor(color)) |
                                   board->getQueens(board->flipColor(color))));

  // rook,queen attacks
  sliding_attacks =
      Rmagic(sq, board->getAllPieces()) & ~board->getAllPieces(color);
  attackset |= (sliding_attacks & (board->getRooks(board->flipColor(color)) |
                                   board->getQueens(board->flipColor(color))));

  // king attacks
  attackset |=
      (board->king_moves[sq] & board->getKings(board->flipColor(color)));
  return attackset;
}

bitboard Movegen::pinnedPieces(const Color &color) {
  bitboard king = board->getKings(color);
  square king_sq = pop_lsb(king);

  bitboard pinned = EMPTYBOARD;

  bitboard pinners =
      Rmagic(king_sq, EMPTYBOARD) & (board->getQueens(board->flipColor(color)) |
                                     board->getRooks(board->flipColor(color)));
  while (pinners) {
    square pinner_sq = pop_lsb(pinners);
    bitboard pinnedpieces = between[king_sq][pinner_sq] & board->getAllPieces();
    if (popCount(pinnedpieces) == 1) {
      pinned |= pinnedpieces & board->getAllPieces(color);
    }
  }
  pinners = Bmagic(king_sq, EMPTYBOARD) &
            (board->getQueens(board->flipColor(color)) |
             board->getBishops(board->flipColor(color)));
  while (pinners) {
    square pinner_sq = pop_lsb(pinners);
    bitboard pinnedpieces = between[king_sq][pinner_sq] & board->getAllPieces();
    if (popCount(pinnedpieces) == 1) {
      pinned |= pinnedpieces & board->getAllPieces(color);
    }
  }

  return pinned;
}
