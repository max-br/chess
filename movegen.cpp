#include <assert.h>
#include <iostream>
#include <stdlib.h>

#include "movegen.h"
#include "directions.h"

// ctor
Movegen::Movegen()
{
	initmagicmoves();
	initBetweenTable();
	for(square i = 0; i != 64; ++i){
		knight_moves[i] = initKnightMoves(i);
		king_moves[i] = initKingMoves(i);
	}

	pinned_pieces = 0;
}

void Movegen::initBetweenTable()
{
	for(int from = 0; from != 64; ++from){
		for(int to = 0; to != 64; ++to){
			bitboard between_sqs = 0;
			int tmp_from = from;
			int tmp_to = to;
			// always move from smaller to bigger square
			if(tmp_from < tmp_to){
				tmp_to = from;
				tmp_from = to;
			}

			//moving vertical
			if((from - to) % NORT == 0){
				tmp_to += NORT;
				while(tmp_to < tmp_from){
					between_sqs |= ((bitboard)1 << (tmp_to));
					tmp_to += NORT;
				}
			}

			//moving diagonal (NOEA)
			if((from - to) % NOEA == 0){
				tmp_to += NOEA;
				while(tmp_to < tmp_from){
					between_sqs |= ((bitboard)1 << (tmp_to));
					tmp_to += NOEA;
				}
			}

			//moving diagonal (NOWE)
			if((from - to) % NOWE == 0){
				tmp_to += NOWE;
				while(tmp_to < tmp_from){
					between_sqs |= ((bitboard)1 << (tmp_to));
					tmp_to += NOWE;
				}
			}
			between[from][to] = between_sqs;
		}
		//horizontal moves

		bitboard between_sqs = 0;
		square leftborder = from - (from % 8);
		square rightborder = from + (7 - (from % 8));
		int to = from - 1;
		while(to > leftborder){
			between_sqs |= ((bitboard)1 << (to));
			to--;
			between[from][to] = between_sqs;
		}
		between_sqs = 0;
		to = from + 1;
		while(to < rightborder){
			between_sqs |= ((bitboard)1 << (to));
			to++;
			between[from][to] = between_sqs;
		}

	}
}

// returns a mask of possible knight moves from a square
bitboard Movegen::initKnightMoves(const square& sq)
{
	bitboard knightBB = ((bitboard)1 << (sq));
	bitboard l1 = (knightBB >> 1) & CLEAR_FILE_H;
	bitboard l2 = (knightBB >> 2) & CLEAR_FILE_G_H;
	bitboard r1 = (knightBB << 1) & CLEAR_FILE_A;
	bitboard r2 = (knightBB << 2) & CLEAR_FILE_A_B;
	bitboard h1 = l1 | r1;
	bitboard h2 = l2 | r2;
	return (h1<<16) | (h1>>16) | (h2<<8) | (h2>>8);
}

// returns a mask of possible king moves from a square
bitboard Movegen::initKingMoves(const square& sq)
{
	bitboard kingBB = ((bitboard)1 << (sq));
	bitboard moves = eastOne(kingBB) | westOne(kingBB);
	kingBB |= moves;
	moves |= nortOne(kingBB) | soutOne(kingBB);
	return moves;
}

// encodes a move
inline Move Movegen::encodeMove(const square& from, const square& to,
		const Piece& piece, const Piece& capture = EMPTY, const Piece& promotion = EMPTY, const square& EP = 0, const Movetype& type = NONE)
{
	return encodeFrom(from) | encodeTo(to) | encodePiece(piece) | encodeCapture(capture) | encodePromotion(promotion) | encodeEP(EP) | encodeMovetype(type);
}

bool searchNullMoves(const Movelist& list){
	for(int i = 0; i < list.count; ++i){
		if(list.moves[i] == NULLMOVE){
			return false;
		}
	}
	return true;
}


void Movegen::genAllMoves(const Board& board, Movelist& list, const Color& color)
{
	pinned_pieces = pinnedPieces(board,color);
	if(!isInCheck(board,color)){
		if(color == WHITE){
			genWhitePawnMoves(board,list);
		}else{
			genBlackPawnMoves(board,list);
		}
		genBishopMoves(board,list,color);
		genKnightMoves(board,list,color);
		genRookMoves(board,list,color);
		genQueenMoves(board,list,color);
		genKingMoves(board,list,color);
		genCastles(board,list,color);
		//add en passants
		for(int i = 0; i != 2; ++i){
			if(board.en_passant[i] != NULLMOVE){
				list.moves[list.count++] = board.en_passant[i];
			}
		}
	}else{
		genEvades(board,list,color);
	}
}

void Movegen::genCastles(const Board& board, Movelist& list, const Color& color){
	if(color == WHITE){
		if(board.castling_rights[WKINGSIDE]){
			if(board.squares[5] == EMPTY && board.squares[6] == EMPTY){
				if(!isAttacked(board,color,5) && !isAttacked(board,color,6)){
					list.moves[list.count++] = board.castle_moves[WKINGSIDE];
				}
			}
		}
		if(board.castling_rights[WQUEENSIDE]){
			if(board.squares[1] == EMPTY && board.squares[2] == EMPTY && board.squares[3] == EMPTY){
				if(!isAttacked(board,color,2) && !isAttacked(board,color,3)){
					list.moves[list.count++] = board.castle_moves[WQUEENSIDE];
				}
			}
		}
	}
	if(color == BLACK){
		if(board.castling_rights[BKINGSIDE]){
			if(board.squares[61] == EMPTY && board.squares[62] == EMPTY){
				if(!isAttacked(board,color,61) && !isAttacked(board,color,62)){
					list.moves[list.count++] = board.castle_moves[BKINGSIDE];
				}
			}
		}
		if(board.castling_rights[BQUEENSIDE]){
			if(board.squares[57] == EMPTY && board.squares[58] == EMPTY && board.squares[59] == EMPTY){
				if(!isAttacked(board,color,58) && !isAttacked(board,color,59)){
					list.moves[list.count++] = board.castle_moves[BQUEENSIDE];
				}
			}
		}
	}
}

void Movegen::genEvades(const Board& board, Movelist& list, const Color& color)
{
	assert(board.getKings(color));
	square king_sq = bitScanForward(board.getKings(color));
	bitboard moves = king_moves[king_sq] & ~board.getAllPieces(color);

	while(moves){
		square to = pop_lsb(moves);
		if(!isAttacked(board,color,to)){
			list.moves[list.count++] = encodeMove(king_sq,to,KING,board.squares[to]);
		}
	}
	bitboard attackers = getAttackers(board,color,king_sq);
	// if there is only one attacker, we can get out of check by taking the attacker,
	// or by moving a piece between the king and the attacker

	if(popCount(attackers) == 1){
		square attacker_sq = pop_lsb(attackers);
		//capture the attacking piece
		bitboard defenders = getAttackers(board, board.flipColor(color), attacker_sq)
				& ~board.getKings(color);

		while(defenders){
			square from = pop_lsb(defenders);
			Piece defender = board.squares[from];

			if(defender == PAWN){
				//check for a promotion
				if(attacker_sq > 55 || attacker_sq < 8){
					genPromotion(board,list,from,attacker_sq);
				}else{
					list.moves[list.count++] = encodeMove(from,attacker_sq,defender,board.squares[attacker_sq]);
				}
			}
			if(defender != PAWN){
				list.moves[list.count++] = encodeMove(from,attacker_sq,defender,board.squares[attacker_sq]);
			}
		}

		Piece attacker_piece = board.squares[attacker_sq];
		if(attacker_piece == QUEEN || attacker_piece == BISHOP || attacker_piece == ROOK){
			bitboard interposing_sqs = between[king_sq][attacker_sq];
			while(interposing_sqs){
				square to = pop_lsb(interposing_sqs);
				bitboard interposing_pieces = getMovers(board,color,to) & ~board.getKings(color) & ~pinned_pieces;
				while(interposing_pieces){
					square from = pop_lsb(interposing_pieces);
					Piece interposing_piece = board.squares[from];
					if(interposing_piece == PAWN){
						if(to > 55 || to < 8){
							genPromotion(board,list,from,to);
						}else{
							list.moves[list.count++] = encodeMove(from,to,interposing_piece,board.squares[to]);
						}
					}
					if(interposing_piece != PAWN){
						list.moves[list.count++] = encodeMove(from,to,interposing_piece,board.squares[to]);
					}
				}
			}
		}
		for(int i = 0; i != 2; ++i){
			if(extractEP(board.en_passant[i]) == attacker_sq && board.en_passant[i] != NULLMOVE){
				list.moves[list.count++] = board.en_passant[i];
			}
		}
	}

}

// generate pseudolegal moves for all white pawns
void Movegen::genWhitePawnMoves(const Board& board, Movelist& list)
{
	bitboard white_pawns = board.getPawns(WHITE);
	bitboard white_pawn_push_fst = (white_pawns << NORT) & ~board.getAllPieces();
	//if a piece got to rank 3 from rank 2, check if its possible to move again
	bitboard white_pawn_push_snd = ((white_pawn_push_fst & RANK_3) << NORT) & ~board.getAllPieces();

	bitboard white_pawn_left_attack = (white_pawns & CLEAR_FILE_A) << NOWE;
	bitboard white_pawn_right_attack = (white_pawns & CLEAR_FILE_H) << NOEA;
	// only Attack occupied squares
	white_pawn_left_attack = white_pawn_left_attack & board.getBlackPieces();
	white_pawn_right_attack = white_pawn_right_attack & board.getBlackPieces();

	// spool Moves to the movelist
	while(white_pawn_push_fst){
		square to = pop_lsb(white_pawn_push_fst);
		if(to > 55){
			genPromotion(board,list,to-NORT,to);
		}else{
			list.moves[list.count++] = encodeMove(to-NORT,to,PAWN);
		}
	}

	while(white_pawn_push_snd){
		square to = pop_lsb(white_pawn_push_snd);
		list.moves[list.count++] = encodeMove(to-NORT*2,to,PAWN);
	}

	while(white_pawn_left_attack){
		square to = pop_lsb(white_pawn_left_attack);
		if(to > 55){
			genPromotion(board,list,to-NOWE,to);
		}else{
			list.moves[list.count++] = encodeMove(to-NOWE,to,PAWN,board.squares[to]);
		}
	}

	while(white_pawn_right_attack){
		square to = pop_lsb(white_pawn_right_attack);
		if(to > 55){
			genPromotion(board,list,to-NOEA,to);
		}else{
			list.moves[list.count++] = encodeMove(to-NOEA,to,PAWN,board.squares[to]);
		}
	}
}

// generate pseudolegal moves for all black pawns, using similiar logic to the white pawns, but shifting the bitboards to the right.
void Movegen::genBlackPawnMoves(const Board& board, Movelist& list)
{
	bitboard black_pawns = board.getPawns(BLACK);
	bitboard black_pawn_push_fst = (black_pawns >> SOUT) & ~board.getAllPieces();
	//if a piece got to rank 3 from rank 2, check if its possible to move again
	bitboard black_pawn_push_snd = ((black_pawn_push_fst & RANK_6) >> SOUT) & ~board.getAllPieces();

	bitboard black_pawn_left_attack = (black_pawns & CLEAR_FILE_A) >> SOWE;
	bitboard black_pawn_right_attack = (black_pawns & CLEAR_FILE_H) >> SOEA;
	// only Attack occupied squares
	black_pawn_left_attack = black_pawn_left_attack & board.getWhitePieces();
	black_pawn_right_attack = black_pawn_right_attack & board.getWhitePieces();

	// spool Moves to the movelist
	while(black_pawn_push_fst){
		square to = pop_lsb(black_pawn_push_fst);
		if(to < 8){
			genPromotion(board,list,to+SOUT,to);
		}else{
			list.moves[list.count++] = encodeMove(to+SOUT,to,PAWN);
		}
	}
	while(black_pawn_push_snd){
		square to = pop_lsb(black_pawn_push_snd);
		list.moves[list.count++] = encodeMove(to+SOUT*2,to,PAWN);
	}

	while(black_pawn_left_attack){
		square to = pop_lsb(black_pawn_left_attack);
		if(to < 8){
			genPromotion(board,list,to+SOWE,to);
		}else{
			list.moves[list.count++] = encodeMove(to+SOWE,to,PAWN,board.squares[to]);
		}
	}

	while(black_pawn_right_attack){
		square to = pop_lsb(black_pawn_right_attack);
		if(to < 8){
			genPromotion(board,list,to+SOEA,to);
		}else{
			list.moves[list.count++] = encodeMove(to+SOEA,to,PAWN,board.squares[to]);
		}
	}
}

void Movegen::genPromotion(const Board& board, Movelist& list,const square& from, const square& to)
{
	list.moves[list.count++] = encodeMove(from,to,PAWN,board.squares[to],KNIGHT);
	list.moves[list.count++] = encodeMove(from,to,PAWN,board.squares[to],BISHOP);
	list.moves[list.count++] = encodeMove(from,to,PAWN,board.squares[to],ROOK);
	list.moves[list.count++] = encodeMove(from,to,PAWN,board.squares[to],QUEEN);
}


//generate pseudolegal moves for the Bishops of one color, using BuzzOS magic bitboard implementation
void Movegen::genBishopMoves(const Board& board, Movelist& list, const Color& color)
{
	bitboard bishops = board.getBishops(color);
	while(bishops){
		square from = pop_lsb(bishops);
		bitboard moves =  Bmagic(from, board.getAllPieces()) & ~board.getAllPieces(color);
		while(moves){
			square to = pop_lsb(moves);
			list.moves[list.count++] = encodeMove(from,to,BISHOP,board.squares[to]);
		}
	}
}

//generate legal moves for the king of one color
void Movegen::genKingMoves(const Board& board, Movelist& list,const Color& color)
{
	if(board.getKings(color)){
		square from = bitScanForward(board.getKings(color));
		bitboard moves = king_moves[from] & ~board.getAllPieces(color);
		while(moves){
			square to = pop_lsb(moves);
			if(!isAttacked(board,color,to)){
				list.moves[list.count++] = encodeMove(from,to,KING,board.squares[to]);
			}

		}
	}
}


//generate pseudolegal moves for all knights of one color
void Movegen::genKnightMoves(const Board& board, Movelist& list,const Color& color)
{
	bitboard knights = board.getKnights(color);
	while(knights){
		square from = pop_lsb(knights);
		bitboard moves = knight_moves[from] & ~board.getAllPieces(color);
		while(moves){
			square to = pop_lsb(moves);
			list.moves[list.count++] = encodeMove(from,to,KNIGHT,board.squares[to]);
		}
	}
}

//generate pseudolegal moves for the Rooks of one color, using BuzzOS magic bitboard implementation
void Movegen::genRookMoves(const Board& board, Movelist& list, const Color& color)
{
	bitboard rooks = board.getRooks(color);
	while(rooks){
		square from = pop_lsb(rooks);
		bitboard moves =  Rmagic(from, board.getAllPieces())  & ~board.getAllPieces(color);
		while(moves){
			square to = pop_lsb(moves);
			list.moves[list.count++] = encodeMove(from,to,ROOK,board.squares[to]);
		}
	}
}

//generate pseudolegal moves for the Queen of one color, using BuzzOS magic bitboard implementation
void Movegen::genQueenMoves(const Board& board, Movelist& list, const Color& color)
{
	bitboard queen = board.getQueens(color);
	while(queen){
		square from = pop_lsb(queen);
		bitboard moves =  (Bmagic(from, board.getAllPieces()) | Rmagic(from, board.getAllPieces()))  & ~board.getAllPieces(color);
		while(moves){
			square to = pop_lsb(moves);
			list.moves[list.count++] = encodeMove(from,to,QUEEN,board.squares[to]);
		}
	}
}

// returns a bitboard containing all the pieces of a color that can move to a square (to include pawn pushes)
bitboard Movegen::getMovers(const Board& board, const Color& color, const square& sq)
{
	return getAttackers(board,board.flipColor(color),sq) | getPawnMovers(board,color,sq);
}

// returns all pawns that can move to a square
bitboard Movegen::getPawnMovers(const Board& board, const Color& color, const square& sq)
{
	bitboard pawns = 0;
	bitboard pawn_push_fst = 0;
	bitboard pawn_push_snd = 0;

	if(color == WHITE){
		pawns = board.getPawns(WHITE);
		pawn_push_fst = (pawns << NORT) & ~board.getAllPieces();
		if(pawn_push_fst & board.square_mask[sq]){
			return board.square_mask[sq-NORT];
		}
		//if a piece got to rank 3 from rank 2, check if its possible to move again
		pawn_push_snd = ((pawn_push_fst & RANK_3) << NORT) & ~board.getAllPieces();
		if(pawn_push_snd & board.square_mask[sq]){
			return board.square_mask[sq-NORT*2];
		}
	}

	if(color == BLACK){
		pawns = board.getPawns(BLACK);
		pawn_push_fst = (pawns >> SOUT) & ~board.getAllPieces();
		if(pawn_push_fst & board.square_mask[sq]){
			return board.square_mask[sq+NORT];
		}
		//if a piece got to rank 3 from rank 2, check if its possible to move again
		pawn_push_snd = ((pawn_push_fst & RANK_6) >> SOUT) & ~board.getAllPieces();
		if(pawn_push_snd & board.square_mask[sq]){
			return board.square_mask[sq+NORT*2];
		}

	}
	return 0;
}

bitboard Movegen::getAttackers(const Board& board, const Color& color, const square& sq)
{
	bitboard attackset = 0;

	if(color == BLACK){
		bitboard white_pawn_left_attack = (board.getPawns(WHITE) & CLEAR_FILE_A) << NOWE;
		bitboard white_pawn_right_attack = (board.getPawns(WHITE) & CLEAR_FILE_H) << NOEA;
		// only Attack occupied squares
		white_pawn_left_attack = white_pawn_left_attack & board.getBlackPieces();
		white_pawn_right_attack = white_pawn_right_attack & board.getBlackPieces();
		//add Piece to attackset by shifting back
		if(board.square_mask[sq] & white_pawn_left_attack){
			attackset |= (board.square_mask[sq] & CLEAR_FILE_H) >> SOEA;
		}
		if(board.square_mask[sq] & white_pawn_right_attack){
			attackset |= (board.square_mask[sq] & CLEAR_FILE_A) >> SOWE;
		}
	}

	if(color == WHITE){
		bitboard black_pawn_left_attack = (board.getPawns(BLACK) & CLEAR_FILE_A) >> SOWE;
		bitboard black_pawn_right_attack = (board.getPawns(BLACK) & CLEAR_FILE_H) >> SOEA;
		// only Attack occupied squares
		black_pawn_left_attack = black_pawn_left_attack & board.getWhitePieces();
		black_pawn_right_attack = black_pawn_right_attack & board.getWhitePieces();
		if(board.square_mask[sq] & black_pawn_left_attack){
			attackset |= (board.square_mask[sq] & CLEAR_FILE_H) << NOEA;
		}
		if(board.square_mask[sq] & black_pawn_right_attack){
			attackset |= (board.square_mask[sq] & CLEAR_FILE_A) << NOWE;
		}
	}

	attackset |= (knight_moves[sq] & board.getKnights(board.flipColor(color)));

	//bishop,queen attacks
	bitboard sliding_attacks = Bmagic(sq, board.getAllPieces()) & ~board.getAllPieces(color);
	attackset |= (sliding_attacks & (board.getBishops(board.flipColor(color)) | board.getQueens(board.flipColor(color))));

	//rook,queen attacks
	sliding_attacks = Rmagic(sq, board.getAllPieces()) & ~board.getAllPieces(color);
	attackset |= (sliding_attacks & (board.getRooks(board.flipColor(color)) | board.getQueens(board.flipColor(color))));

	//king attacks
	attackset |= (king_moves[sq] & board.getKings(board.flipColor(color)));
	return attackset;

}

// returns true if a square is attacked by any piece of a color
bool Movegen::isAttacked(const Board& board, const Color& color, const square& sq)
{
	bitboard attacks = 0;

	if(color == BLACK){
		bitboard white_pawn_left_attack = (board.getPawns(WHITE) & CLEAR_FILE_A) << NOWE;
		bitboard white_pawn_right_attack = (board.getPawns(WHITE) & CLEAR_FILE_H) << NOEA;
		if(board.square_mask[sq] & (white_pawn_left_attack | white_pawn_right_attack)){
			return true;
		}
	}
	if(color == WHITE){
		bitboard black_pawn_left_attack = (board.getPawns(BLACK) & CLEAR_FILE_A) >> SOWE;
		bitboard black_pawn_right_attack = (board.getPawns(BLACK) & CLEAR_FILE_H) >> SOEA;
		if(board.square_mask[sq] & (black_pawn_left_attack | black_pawn_right_attack)){
			return true;
		}
	}

	if(knight_moves[sq] & board.getKnights(board.flipColor(color))){
		return true;
	}

	attacks = Bmagic(sq, board.getAllPieces()) & ~board.getAllPieces(color);
	if(attacks & (board.getBishops(board.flipColor(color)) | board.getQueens(board.flipColor(color)))){
		return true;
	}
	attacks = Rmagic(sq, board.getAllPieces()) & ~board.getAllPieces(color);
	if(attacks & (board.getRooks(board.flipColor(color)) | board.getQueens(board.flipColor(color)))){
		return true;
	}
	if(king_moves[sq] & board.getKings(board.flipColor(color))){
		return true;
	}
	return false;
}

bool Movegen::isInCheck(const Board& board, const Color& color)
{
	assert(board.getKings(color));
	square king_sq = bitScanForward(board.getKings(color));
	return isAttacked(board,color,king_sq);
}

bitboard Movegen::pinnedPieces(const Board& board, const Color& color)
{
	bitboard king = board.getKings(color);
	square king_sq = pop_lsb(king);

	bitboard pinned = EMPTYBOARD;

	bitboard pinners = Rmagic(king_sq, EMPTYBOARD) & (board.getQueens(board.flipColor(color)) |
			board.getRooks(board.flipColor(color)));
	while(pinners){
		square pinner_sq = pop_lsb(pinners);
		bitboard pinnedpieces = between[king_sq][pinner_sq] & board.getAllPieces();
		if(popCount(pinnedpieces) == 1){
			pinned |= pinnedpieces & board.getAllPieces(color);
		}
	}
	pinners = Bmagic(king_sq, EMPTYBOARD) & (board.getQueens(board.flipColor(color)) |
			board.getBishops(board.flipColor(color)));
	while(pinners){
		square pinner_sq = pop_lsb(pinners);
		bitboard pinnedpieces = between[king_sq][pinner_sq] & board.getAllPieces();
		if(popCount(pinnedpieces) == 1){
			pinned |= pinnedpieces & board.getAllPieces(color);
		}
	}

	return pinned;
}

