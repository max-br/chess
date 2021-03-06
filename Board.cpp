#include <assert.h>
#include <bitset>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "Board.h"
#include "bitfunc.h"
#include "directions.h"
#include "magicmoves.h"

Board::Board() {
  // Bitboards have to be initialized to 0
  for (int i = 0; i != PIECE_BB_SIZE; i++) {
    piece_BB[i] = 0;
  }

  for (int i = 0; i != 4; i++) {
    castling_rights[i] = false;
  }

  for (square i = 0; i != 64; i++) {
    square_mask[i] = ((bitboard)1 << (i));
    king_moves[i] = initKingMoves(i);
    knight_moves[i] = initKnightMoves(i);
    squares[i] = EMPTY;
  }
  en_passant[0] = 0;
  en_passant[1] = 0;

  castle_moves[WKINGSIDE] = encodeFrom(4) | encodeTo(6) | encodePiece(KING) |
                            encodeCapture(EMPTY) | encodePromotion(EMPTY) |
                            encodeEP(EMPTY) | encodeMovetype(CASTLE);
  castle_moves[WQUEENSIDE] = encodeFrom(4) | encodeTo(2) | encodePiece(KING) |
                             encodeCapture(EMPTY) | encodePromotion(EMPTY) |
                             encodeEP(EMPTY) | encodeMovetype(CASTLE);
  castle_moves[BKINGSIDE] = encodeFrom(60) | encodeTo(62) | encodePiece(KING) |
                            encodeCapture(EMPTY) | encodePromotion(EMPTY) |
                            encodeEP(EMPTY) | encodeMovetype(CASTLE);
  castle_moves[BQUEENSIDE] = encodeFrom(60) | encodeTo(58) | encodePiece(KING) |
                             encodeCapture(EMPTY) | encodePromotion(EMPTY) |
                             encodeEP(EMPTY) | encodeMovetype(CASTLE);

  setupFEN(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // starting
                                                                   // position
}

// returns a mask of possible king moves from a square
bitboard Board::initKingMoves(const square &sq) {
  bitboard kingBB = ((bitboard)1 << (sq));
  bitboard moves = eastOne(kingBB) | westOne(kingBB);
  kingBB |= moves;
  moves |= nortOne(kingBB) | soutOne(kingBB);
  return moves;
}

// returns a mask of possible knight moves from a square
bitboard Board::initKnightMoves(const square &sq) {
  bitboard knightBB = ((bitboard)1 << (sq));
  bitboard l1 = (knightBB >> 1) & CLEAR_FILE_H;
  bitboard l2 = (knightBB >> 2) & CLEAR_FILE_G_H;
  bitboard r1 = (knightBB << 1) & CLEAR_FILE_A;
  bitboard r2 = (knightBB << 2) & CLEAR_FILE_A_B;
  bitboard h1 = l1 | r1;
  bitboard h2 = l2 | r2;
  return (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
}

bool Board::checkIntegrity() {
  if (getAllPieces() & ~getPawns() & ~getBishops() & ~getKnights() &
      ~getRooks() & ~getQueens() & ~getKings()) {
    return false;
  }
  return true;
}

void Board::checkEnPassant(const square &from, const square &to,
                           const Piece &piece, const Color &color) {
  if (piece == PAWN) {
    if (color == WHITE) {
      if (from >= 8 && from <= 15) {
        if (to >= 24 && to <= 31) { // move is a double pawn push
          if (to - 1 >= 24 && squares[to - 1] == PAWN &&
              (square_mask[to - 1] & piece_BB[BLACK])) {
            en_passant[0] = encodeFrom(to - 1) | encodeTo(to - 8) |
                            encodePiece(PAWN) | encodeCapture(PAWN) |
                            encodePromotion(EMPTY) | encodeEP(to) |
                            encodeMovetype(NONE);
          }
          if (to + 1 <= 31 && squares[to + 1] == PAWN &&
              (square_mask[to + 1] & piece_BB[BLACK])) {
            en_passant[1] = encodeFrom(to + 1) | encodeTo(to - 8) |
                            encodePiece(PAWN) | encodeCapture(PAWN) |
                            encodePromotion(EMPTY) | encodeEP(to) |
                            encodeMovetype(NONE);
          }
        }
      }
    }
    if (color == BLACK) {
      if (from >= 48 && from <= 55) {
        if (to >= 32 && to <= 39) { // move is a double pawn push
          if (to - 1 >= 32 && squares[to - 1] == PAWN &&
              (square_mask[to - 1] & piece_BB[WHITE])) {
            en_passant[0] = encodeFrom(to - 1) | encodeTo(to + 8) |
                            encodePiece(PAWN) | encodeCapture(PAWN) |
                            encodePromotion(EMPTY) | encodeEP(to) |
                            encodeMovetype(NONE);
          }
          if (to + 1 <= 39 && squares[to + 1] == PAWN &&
              (square_mask[to + 1] & piece_BB[WHITE])) {
            en_passant[1] = encodeFrom(to + 1) | encodeTo(to + 8) |
                            encodePiece(PAWN) | encodeCapture(PAWN) |
                            encodePromotion(EMPTY) | encodeEP(to) |
                            encodeMovetype(NONE);
          }
        }
      }
    }
  }
}

bitboard Board::isAttacked(const Color &color, const square &sq) {
  bitboard attacks = 0;

  if (color == BLACK) {
    bitboard white_pawn_left_attack = (getPawns(WHITE) & CLEAR_FILE_A) << NOWE;
    bitboard white_pawn_right_attack = (getPawns(WHITE) & CLEAR_FILE_H) << NOEA;
    if (square_mask[sq] & (white_pawn_left_attack | white_pawn_right_attack)) {
      return true;
    }
  }
  if (color == WHITE) {
    bitboard black_pawn_left_attack = (getPawns(BLACK) & CLEAR_FILE_A) >> SOWE;
    bitboard black_pawn_right_attack = (getPawns(BLACK) & CLEAR_FILE_H) >> SOEA;
    if (square_mask[sq] & (black_pawn_left_attack | black_pawn_right_attack)) {
      return true;
    }
  }

  if (knight_moves[sq] & getKnights(flipColor(color))) {
    return true;
  }

  attacks = Bmagic(sq, getAllPieces()) & ~getAllPieces(color);
  if (attacks & (getBishops(flipColor(color)) | getQueens(flipColor(color)))) {
    return true;
  }
  attacks = Rmagic(sq, getAllPieces()) & ~getAllPieces(color);
  if (attacks & (getRooks(flipColor(color)) | getQueens(flipColor(color)))) {
    return true;
  }
  if (king_moves[sq] & getKings(flipColor(color))) {
    return true;
  }
  return false;
}

bool Board::isInCheck(const Color &color) {
  assert(getKings(color));
  square king_sq = bitScanForward(getKings(color));
  return isAttacked(color, king_sq);
}

// must be called at the end of makemove, so changes to the rook position are
// already done
void Board::updateCastlingRights(const Move &move, const Color &color) {
  if (castling_rights[WKINGSIDE] || castling_rights[WQUEENSIDE] ||
      castling_rights[BKINGSIDE] || castling_rights[BQUEENSIDE]) {
    Piece piece = (Piece)extractPiece(move);
    Piece capture = (Piece)extractCapture(move);
    square from = extractFrom(move);
    square to = extractTo(move);

    if (piece == KING) {
      if (color == WHITE) {
        castling_rights[WKINGSIDE] = false;
        castling_rights[WQUEENSIDE] = false;
      }
      if (color == BLACK) {
        castling_rights[BKINGSIDE] = false;
        castling_rights[BQUEENSIDE] = false;
      }
    }
    if (piece == ROOK) {
      if (color == WHITE) {
        if (from == 0) {
          castling_rights[WQUEENSIDE] = false;
        }
        if (from == 7) {
          castling_rights[WKINGSIDE] = false;
        }
      }
      if (color == BLACK) {
        if (from == 56) {
          castling_rights[BQUEENSIDE] = false;
        }
        if (from == 63) {
          castling_rights[BKINGSIDE] = false;
        }
      }
    }
    if (capture == ROOK) {
      if (to == 0) {
        castling_rights[WQUEENSIDE] = false;
      }
      if (to == 7) {
        castling_rights[WKINGSIDE] = false;
      }
      if (to == 56) {
        castling_rights[BQUEENSIDE] = false;
      }
      if (to == 63) {
        castling_rights[BKINGSIDE] = false;
      }
    }
  }
}

/* Make a move, put the Moverecord on the Movestack.
 * Returns:
 * true: move is legal
 * false: move is illegal
 */
bool Board::makeMove(const Move &move) {
  assert(move != NULLMOVE);

  Moverecord record;

  // copy castling rights and en passant moves to move record
  memcpy(record.castling_rights, castling_rights, sizeof(bool) * 4);
  memcpy(record.en_passant, en_passant, sizeof(Move) * 2);

  en_passant[0] = 0;
  en_passant[1] = 0;

  if (!extractEP(move) && !extractMovetype(move) && !extractPromotion(move)) {
    Piece piece = (Piece)extractPiece(move);
    square from = extractFrom(move);
    square to = extractTo(move);

    // calculate en passants
    checkEnPassant(from, to, piece, us);

    record.move = move;
    record.capture = (Piece)extractCapture(move);
    record.us = us;
    record.them = them;
    move_stack.push(record);

    // enum Piece EMPTY == 0
    if (record.capture) {
      piece_BB[record.capture] &= ~square_mask[to];
      piece_BB[them] &= ~square_mask[to];
    }

    squares[from] = EMPTY;
    squares[to] = piece;

    piece_BB[piece] &= ~square_mask[from];
    piece_BB[us] &= ~square_mask[from];

    piece_BB[piece] |= square_mask[to];
    piece_BB[us] |= square_mask[to];
  }
  if (extractEP(move)) {
    makeEPMove(move, record);
  }
  if (extractMovetype(move) == CASTLE) {
    makeCastleMove(move, record);
  }
  if (extractPromotion(move)) {
    makePromotionMove(move, record);
  }

  // check for illegal moves
  if (isInCheck(us)) {
    switchColor();
    assert(checkIntegrity());
    return false;
  }

  in_check = isInCheck(them);

  updateCastlingRights(move, us);
  switchColor();

  assert(checkIntegrity());
  return true;
}

void Board::makePromotionMove(const Move &move, Moverecord &record) {
  Piece piece = PAWN;
  Piece promotion = (Piece)extractPromotion(move);
  square from = extractFrom(move);
  square to = extractTo(move);

  record.move = move;
  record.capture = (Piece)extractCapture(move);
  record.us = us;
  record.them = them;
  move_stack.push(record);

  // enum Piece EMPTY == 0
  if (record.capture) {
    piece_BB[record.capture] &= ~square_mask[to];
    piece_BB[them] &= ~square_mask[to];
  }

  squares[from] = EMPTY;
  squares[to] = promotion;

  piece_BB[piece] &= ~square_mask[from];
  piece_BB[us] &= ~square_mask[from];

  piece_BB[promotion] |= square_mask[to];
  piece_BB[us] |= square_mask[to];
}

void Board::makeCastleMove(const Move &move, Moverecord &record) {
  record.move = move;
  record.capture = EMPTY;
  record.us = us;
  record.them = them;
  move_stack.push(record);

  square rook_from = 0;
  square rook_to = 0;
  square king_from = 0;
  square king_to = 0;
  if (move == castle_moves[WKINGSIDE]) {
    rook_from = 7;
    rook_to = 5;
    king_from = 4;
    king_to = 6;
  }
  if (move == castle_moves[WQUEENSIDE]) {
    rook_from = 0;
    rook_to = 3;
    king_from = 4;
    king_to = 2;
  }
  if (move == castle_moves[BKINGSIDE]) {
    rook_from = 63;
    rook_to = 61;
    king_from = 60;
    king_to = 62;
  }
  if (move == castle_moves[BQUEENSIDE]) {
    rook_from = 56;
    rook_to = 59;
    king_from = 60;
    king_to = 58;
  }
  piece_BB[us] |= square_mask[rook_to];
  piece_BB[us] |= square_mask[king_to];
  piece_BB[us] &= ~square_mask[rook_from];
  piece_BB[us] &= ~square_mask[king_from];

  piece_BB[ROOK] |= square_mask[rook_to];
  piece_BB[ROOK] &= ~square_mask[rook_from];

  piece_BB[KING] |= square_mask[king_to];
  piece_BB[KING] &= ~square_mask[king_from];

  squares[rook_from] = EMPTY;
  squares[king_from] = EMPTY;

  squares[rook_to] = ROOK;
  squares[king_to] = KING;
}

void Board::makeEPMove(const Move &move, Moverecord &record) {
  record.move = move;
  record.capture = PAWN;
  record.us = us;
  record.them = them;
  move_stack.push(record);

  square from = extractFrom(move);
  square to = extractTo(move);

  square captured_sq = extractEP(move);

  // enum Piece EMPTY == 0
  if (PAWN) {
    piece_BB[PAWN] &= ~square_mask[captured_sq];
    piece_BB[them] &= ~square_mask[captured_sq];
  }

  squares[captured_sq] = EMPTY;
  squares[from] = EMPTY;
  squares[to] = PAWN;

  piece_BB[PAWN] &= ~square_mask[from];
  piece_BB[us] &= ~square_mask[from];

  piece_BB[PAWN] |= square_mask[to];
  piece_BB[us] |= square_mask[to];
  assert(checkIntegrity());
}

void Board::unmakeMove() {
  switchColor();

  Moverecord &lastmove = move_stack.top();

  en_passant[0] = lastmove.en_passant[0];
  en_passant[1] = lastmove.en_passant[1];

  for (int i = 0; i != 4; ++i) {
    castling_rights[i] = lastmove.castling_rights[i];
  }

  if (!extractEP(lastmove.move) && !extractMovetype(lastmove.move) &&
      !extractPromotion(lastmove.move)) {
    square from = extractFrom(lastmove.move);
    square to = extractTo(lastmove.move);
    Piece piece = (Piece)extractPiece(lastmove.move);

    squares[from] = piece;
    squares[to] = lastmove.capture;

    piece_BB[piece] &= ~square_mask[to];
    piece_BB[lastmove.us] &= ~square_mask[to];

    piece_BB[piece] |= square_mask[from];
    piece_BB[lastmove.us] |= square_mask[from];

    piece_BB[lastmove.them] &= ~square_mask[to];

    // enum Piece EMPTY == 0
    if (lastmove.capture) {
      piece_BB[lastmove.capture] |= square_mask[to];
      piece_BB[lastmove.them] |= square_mask[to];
    }
    move_stack.pop();
    assert(checkIntegrity());
    return;
  }
  if (extractEP(lastmove.move)) {
    unmakeEPMove(lastmove);
    move_stack.pop();
    return;
  }
  if (extractMovetype(lastmove.move) == CASTLE) {
    unmakeCastleMove(lastmove);
    move_stack.pop();
    return;
  }
  if (extractPromotion(lastmove.move)) {
    unmakePromotionMove(lastmove);
    move_stack.pop();
    return;
  }
}

void Board::unmakeCastleMove(const Moverecord &lastmove) {
  square rook_from = 0;
  square rook_to = 0;
  square king_from = 0;
  square king_to = 0;
  if (lastmove.move == castle_moves[WKINGSIDE]) {
    rook_from = 5;
    rook_to = 7;
    king_from = 6;
    king_to = 4;
  }
  if (lastmove.move == castle_moves[WQUEENSIDE]) {
    rook_from = 3;
    rook_to = 0;
    king_from = 2;
    king_to = 4;
  }
  if (lastmove.move == castle_moves[BKINGSIDE]) {
    rook_from = 61;
    rook_to = 63;
    king_from = 62;
    king_to = 60;
  }
  if (lastmove.move == castle_moves[BQUEENSIDE]) {
    rook_from = 59;
    rook_to = 56;
    king_from = 58;
    king_to = 60;
  }

  piece_BB[lastmove.us] |= square_mask[rook_to];
  piece_BB[lastmove.us] |= square_mask[king_to];
  piece_BB[lastmove.us] &= ~square_mask[rook_from];
  piece_BB[lastmove.us] &= ~square_mask[king_from];

  piece_BB[ROOK] |= square_mask[rook_to];
  piece_BB[ROOK] &= ~square_mask[rook_from];

  piece_BB[KING] |= square_mask[king_to];
  piece_BB[KING] &= ~square_mask[king_from];

  squares[rook_from] = EMPTY;
  squares[king_from] = EMPTY;

  squares[rook_to] = ROOK;
  squares[king_to] = KING;
  assert(checkIntegrity());
}

void Board::unmakeEPMove(const Moverecord &lastmove) {
  square from = extractFrom(lastmove.move);
  square to = extractTo(lastmove.move);
  square captured_sq = extractEP(lastmove.move);
  Piece piece = PAWN;

  squares[from] = piece;
  squares[to] = EMPTY;
  squares[captured_sq] = PAWN;

  piece_BB[PAWN] &= ~square_mask[to];
  piece_BB[lastmove.us] &= ~square_mask[to];

  piece_BB[PAWN] |= square_mask[from];
  piece_BB[lastmove.us] |= square_mask[from];

  // put capped pawn on board again
  piece_BB[PAWN] |= square_mask[captured_sq];
  piece_BB[lastmove.them] |= square_mask[captured_sq];
  assert(checkIntegrity());
}

void Board::unmakePromotionMove(const Moverecord &lastmove) {
  square from = extractFrom(lastmove.move);
  square to = extractTo(lastmove.move);
  Piece promotion = (Piece)extractPromotion(lastmove.move);

  squares[from] = PAWN;
  squares[to] = lastmove.capture;

  piece_BB[promotion] &= ~square_mask[to];
  piece_BB[lastmove.us] &= ~square_mask[to];

  piece_BB[PAWN] |= square_mask[from];
  piece_BB[lastmove.us] |= square_mask[from];

  piece_BB[lastmove.them] &= ~square_mask[to];

  // enum Piece EMPTY == 0
  if (lastmove.capture) {
    piece_BB[lastmove.capture] |= square_mask[to];
    piece_BB[lastmove.them] |= square_mask[to];
  }
  assert(checkIntegrity());
}

void Board::resetBoard() {
  for (int i = 0; i != PIECE_BB_SIZE; ++i) {
    piece_BB[i] = 0;
  }
  for (square i = 0; i != 64; ++i) {
    squares[i] = EMPTY;
  }

  for (int i = 0; i != 4; ++i) {
    castling_rights[i] = false;
  }
  for (int i = 0; i != 2; ++i) {
    en_passant[i] = 0;
  }
  fullmove_number = 0;
  halfmove_number = 0;
  while (!move_stack.empty()) {
    move_stack.pop();
  }
}

// sets up a position from a fen string, assumes correct input
void Board::setupFEN(const std::string fen) {
  resetBoard();
  square j = 0;
  size_t i = 0;
  int line = 7;
  // for(size_t i = 0; i != fen.size(); ++i,++j){
  while (i != fen.size() && fen[i] != ' ') {
    if (isdigit(fen[i])) {
      j += (fen[i] - '0' - 1);
    }
    switch (fen[i]) {
    case 'p':
      piece_BB[PAWN] |= square_mask[line * 8 + j];
      piece_BB[BLACK] |= square_mask[line * 8 + j];
      squares[line * 8 + j] = PAWN;
      break;
    case 'n':
      piece_BB[KNIGHT] |= square_mask[line * 8 + j];
      piece_BB[BLACK] |= square_mask[line * 8 + j];
      squares[line * 8 + j] = KNIGHT;
      break;
    case 'b':
      piece_BB[BISHOP] |= square_mask[line * 8 + j];
      piece_BB[BLACK] |= square_mask[line * 8 + j];
      squares[line * 8 + j] = BISHOP;
      break;
    case 'r':
      piece_BB[ROOK] |= square_mask[line * 8 + j];
      piece_BB[BLACK] |= square_mask[line * 8 + j];
      squares[line * 8 + j] = ROOK;
      break;
    case 'q':
      piece_BB[QUEEN] |= square_mask[line * 8 + j];
      piece_BB[BLACK] |= square_mask[line * 8 + j];
      squares[line * 8 + j] = QUEEN;
      break;
    case 'k':
      piece_BB[KING] |= square_mask[line * 8 + j];
      piece_BB[BLACK] |= square_mask[line * 8 + j];
      squares[line * 8 + j] = KING;
      break;
    case 'P':
      piece_BB[PAWN] |= square_mask[line * 8 + j];
      piece_BB[WHITE] |= square_mask[line * 8 + j];
      squares[line * 8 + j] = PAWN;
      break;
    case 'N':
      piece_BB[KNIGHT] |= square_mask[line * 8 + j];
      piece_BB[WHITE] |= square_mask[line * 8 + j];
      squares[line * 8 + j] = KNIGHT;
      break;
    case 'B':
      piece_BB[BISHOP] |= square_mask[line * 8 + j];
      piece_BB[WHITE] |= square_mask[line * 8 + j];
      squares[line * 8 + j] = BISHOP;
      break;
    case 'R':
      piece_BB[ROOK] |= square_mask[line * 8 + j];
      piece_BB[WHITE] |= square_mask[line * 8 + j];
      squares[line * 8 + j] = ROOK;
      break;
    case 'Q':
      piece_BB[QUEEN] |= square_mask[line * 8 + j];
      piece_BB[WHITE] |= square_mask[line * 8 + j];
      squares[line * 8 + j] = QUEEN;
      break;
    case 'K':
      piece_BB[KING] |= square_mask[line * 8 + j];
      piece_BB[WHITE] |= square_mask[line * 8 + j];
      squares[line * 8 + j] = KING;
      break;
    case '/':
      j = -1; // j = 0 in next iteration
      line--;
      break;
    }
    ++i;
    ++j;
  }
  ++i;
  while (i != fen.size() && fen[i] != ' ') {
    if (fen[i] == 'b') {
      us = BLACK;
      them = WHITE;
    }
    if (fen[i] == 'w') {
      us = WHITE;
      them = BLACK;
    }
    ++i;
  }
  ++i;
  while (i != fen.size() && fen[i] != ' ') {
    switch (fen[i]) {
    case 'K':
      castling_rights[WKINGSIDE] = true;
      break;
    case 'k':
      castling_rights[BKINGSIDE] = true;
      break;
    case 'Q':
      castling_rights[WQUEENSIDE] = true;
      break;
    case 'q':
      castling_rights[BQUEENSIDE] = true;
      break;
    }
    ++i;
  }
  ++i;
  int ep_sq_row = 0;
  int ep_sq_line = 0;
  while (i != fen.size() && fen[i] != ' ') {
    if (fen[i] == '-') {
      break;
    }
    if (isalpha(fen[i])) {
      ep_sq_row = fen[i] - 96;
    }
    if (isdigit(fen[i])) {
      ep_sq_line = fen[i] - 48;
    }
    ++i;
  }
  if (ep_sq_line && ep_sq_row) {
    square ep_sq = (ep_sq_line - 1) * 8 + ep_sq_row - 1;
    if (us == WHITE) {
      square from = ep_sq + 8;
      square to = ep_sq - 8;
      checkEnPassant(from, to, PAWN, BLACK);
    }
    if (us == BLACK) {
      square from = ep_sq - 8;
      square to = ep_sq + 8;
      checkEnPassant(from, to, PAWN, WHITE);
    }
  }
  ++i;
  while (i != fen.size() && fen[i] != ' ') {
    if (isdigit(fen[i])) {
      halfmove_number = fen[i] - 48;
    }
    ++i;
  }
  ++i;
  while (i != fen.size() && fen[i] != ' ') {
    if (isdigit(fen[i])) {
      fullmove_number = fen[i] - 48;
    }
    ++i;
  }
  in_check = isInCheck(us);
}

void Board::switchColor() {
  Color tmp = us;
  us = them;
  them = tmp;
}

Move Board::stringToMove(std::string move) {
  char col = move[0] - 'a';
  char row = move[1] - '0';
  square from = (row - 1) * 8 + col;

  col = move[2] - 'a';
  row = move[3] - '0';
  square to = (row - 1) * 8 + col;

  Piece promotion = EMPTY;

  if (move.length() > 4) {
    char p = move[4];
    switch (p) {
    case 'q':
      promotion = QUEEN;
      break;
    case 'n':
      promotion = KNIGHT;
      break;
    case 'r':
      promotion = ROOK;
      break;
    case 'b':
      promotion = BISHOP;
      break;
    default:
      promotion = EMPTY;
      break;
    }
  }

  Movetype mt = NONE;
  if (squares[from] == KING) {
    // TODO: MOVETYPE = CASTLE
  }

  bitboard EP = 0;
  if (squares[from] == PAWN) {
    // TODO: encodeEP
  }

  return encodeFrom(from) | encodeTo(to) | encodePiece(squares[from]) |
         encodeCapture(squares[to]) | encodePromotion(promotion) |
         encodeEP(EP) | encodeMovetype(mt);
}

bool Board::setupMoves(std::vector<std::string> moves) {
  for (auto move : moves) {
    Move m = stringToMove(move);
    if (!makeMove(m)) {
      return false;
    }
  }
  return true;
}
