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
    Bit *       PieceForPlayer(const int playerNumber, ChessPiece piece);
    const char  bitToPieceNotation(int row, int column) const;

    bool isCheckmate(Player* playerColor);
    std::vector<std::array<int,4>>       generateMoves();
    void generateKingMoves(int x, int y, std::vector<std::array<int, 4>>& moves,Player* playerColor);
    void generatePawnMoves(int x, int y, std::vector<std::array<int, 4>>& moves, int color) ;
    bool isOnBoard(int x, int y) const ;
    void generateSlidingPieceMoves(int x, int y, std::vector<std::array<int, 4>>& moves, const std::vector<std::pair<int, int>>& directions,Player* playerColor);
    void generateKnightMoves(int x, int y, std::vector<std::array<int, 4>>& moves,Player* playerColor);
    bool isSquareUnderAttack(int x, int y, Player* attackerColor);
    std::pair<int, int> findKingPosition(Player* playerColor);
    bool isMoveLegal(const std::array<int, 4>& move);
    bool wouldExposeKing(int srcX, int srcY, int dstX, int dstY);
    int evaluateBoard();
    int negamax(int depth, int alpha, int beta);
    bool canMoveBlockAttack(int kingX, int kingY, Player* playerColor);
    bool canBlockAttack(const std::array<int, 4>& move, int kingX, int kingY);
    std::pair<bool, Chess> simulateMove(const std::array<int, 4>& move);
    std::array<int, 4> getBestMove(int depth);
    std::vector<std::array<int,4>> kingChecked(int color);

    ChessSquare      _grid[8][8];
};

