#pragma once
#include "Game.h"
#include "ChessSquare.h"

const int pieceSize = 64;

enum ChessPiece {
    NoPiece = 0,
    Pawn = 1,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

//
// the main game class
//
class Chess : public Game
{
public:
    Chess();
    ~Chess();

    // set up the board
    void        setUpBoard() override;

    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void FENtoBoard(std::string FEN);
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder& holder) override;
    bool        canBitMoveFrom(Bit& bit, BitHolder& src) override;
    bool        canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    void        bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void        setBoardPiece(BitHolder *holder,ChessPiece piece,int color,int posI, int posJ);

    void        stopGame() override;
    BitHolder& getHolderAt(const int x, const int y) override { return _grid[x][y]; }

	void        updateAI() override;
    bool        gameHasAI() override { return true; }
private:
    bool enpassant = false;
    bool enpassantF = false;
    Bit* lastBitWhite;
    Bit* lastBitBlack;
    Bit *       PieceForPlayer(const int playerNumber, ChessPiece piece);
    const char  bitToPieceNotation(int row, int column) const;

    std::vector<std::array<int,4>>       generateMoves();
    std::vector<std::array<int,4>>       generateKingMoves(int playerNumber);
    void returnPawnMoves(int positionY, int positionX, std::vector<std::array<int,4>> &moves, int color);
    bool kingChecked(int color);

    ChessSquare      _grid[8][8];
};

