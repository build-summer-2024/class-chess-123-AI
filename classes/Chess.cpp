#include "Chess.h"

const int AI_PLAYER = 1;
const int HUMAN_PLAYER = -1;

Chess::Chess()
{
}

Chess::~Chess()
{
}

//
// make a chess piece for the player
//
Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char* pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };

    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit* bit = new Bit();
    // should possibly be cached from player class?
    const char* pieceName = pieces[piece - 1];
    std::string spritePath = std::string("chess/") + (playerNumber == 0 ? "w_" : "b_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);

    return bit;
}

void Chess::FENtoBoard(std::string FEN){
    int posI = 0;
    int posJ = 0;
    std::unordered_map<char, std::pair<int, int>> pieceMap = {
        {'r', {Rook, 0}}, {'R', {Rook, 128}},
        {'p', {Pawn, 0}}, {'P', {Pawn, 128}},
        {'q', {Queen, 0}}, {'Q', {Queen, 128}},
        {'n', {Knight, 0}}, {'N', {Knight, 128}},
        {'k', {King, 0}}, {'K', {King, 128}},
        {'b', {Bishop, 0}}, {'B', {Bishop, 128}},
    };
    for (char& i : FEN) {
        if(i == '/'){
            continue;
        }
        if((int(i)-48) <8){
            posI +=int(i)-48;
            if(posI==8){
                posI = 0;
                posJ++;
            }
            continue;
        }else if((int(i)-48) ==8){
            posI = 0;
            posJ++;
            continue;
        }
        if (pieceMap.count(i)) {
            auto [pieceType, color] = pieceMap[i];
            setBoardPiece(&_grid[posI][posJ], static_cast<ChessPiece>(pieceType), color, posI, posJ);
        }
        if(posI == 7){
            posI = 0;
            posJ++;
        }
        else{
            posI++;
        }
    }
}

void Chess::setUpBoard()
{
    setNumberOfPlayers(2);//Set players to 2
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;
    //char piece[2];
    //piece[1] = 0;
    BitHolder theBit = BitHolder();
    for (int i=0; i<_gameOptions.rowX; i++) {
        for (int j=0; j<_gameOptions.rowY; j++) {
            _grid[i][j].initHolder(ImVec2((float)(64*i + 100), (float)(64*j + 100)), "boardsquare.png", i, j);//Initalize the starting squares 
        }
    }
    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    startGame();
}

void Chess::setBoardPiece(BitHolder *holder,ChessPiece piece,int color,int posI, int posJ){
    Bit* newBit = PieceForPlayer(color ==0 ? 1 : 0,piece);
    newBit->setPosition(holder->getPosition());
    newBit->setOwner(getPlayerAt(color ==0 ? 1: 0));
    newBit->setParent(&_grid[posI][posJ]);
    newBit->setGameTag(piece+color);
    holder->setBit(newBit);
}

//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool Chess::actionForEmptyHolder(BitHolder &holder)
{
    return false;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // you can't move anything in tic tac toe
    return true;
}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst)
{
    auto possMoves = generateMoves();
    //std::cout<<possMoves.size()<<std::endl;
    for(int x = 0; x < possMoves.size();x++){
        //std::cout<<possMoves[x][0]<<" "<< possMoves[x][1]<<" "<<possMoves[x][2]<<" "<<possMoves[x][3]<<std::endl;
        if(&dst == &(getHolderAt(possMoves[x][0],possMoves[x][1]))&& &src==&(getHolderAt(possMoves[x][2],possMoves[x][3]))){
            return true;
        }
    }
    //std::cout<<"done"<<std::endl;

    return false;
}

void Chess::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    endTurn();
}

//
// free all the memory used by the game on the heap
//
void Chess::stopGame()
{
}

Player* Chess::checkForWinner()
{
    // check to see if either player has won
    return nullptr;
}

bool Chess::checkForDraw()
{
    // check to see if the board is full
    return false;
}

//
// add a helper to Square so it returns out FEN chess notation in the form p for white pawn, K for black king, etc.
// this version is used from the top level board to record moves
//
const char Chess::bitToPieceNotation(int row, int column) const {
    if (row < 0 || row >= 8 || column < 0 || column >= 8) {
        return '0';
    }

    const char* wpieces = { "?PNBRQK" };
    const char* bpieces = { "?pnbrqk" };
    unsigned char notation = '0';
    Bit* bit = _grid[row][column].bit();
    if (bit) {
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag() & 127];
    } else {
        notation = '0';
    }
    return notation;
}

//
// state strings
//
std::string Chess::initialStateString()
{
    return stateString();
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string Chess::stateString()
{
    std::string s;
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            s += bitToPieceNotation(x, y);
        }
    }
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void Chess::setStateString(const std::string &s)
{
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            int index = y * _gameOptions.rowX + x;
            int playerNumber = s[index] - '0';
            if (playerNumber) {
                _grid[y][x].setBit(PieceForPlayer(playerNumber - 1, Pawn));
            } else {
                _grid[y][x].setBit(nullptr);
            }
        }
    }
}


//
// this is the function that will be called by the AI
//
void Chess::updateAI() 
{
}

std::vector<std::array<int,4>> Chess::generateMoves(){
    std::vector<std::array<int, 4>> moves;

    // Loop through the board to find all pieces owned by the current player
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            ChessSquare* square = &_grid[y][x];
            Bit* piece = square->bit();

            if (piece && piece->getOwner() == getCurrentPlayer()) {
                ChessPiece type = static_cast<ChessPiece>(piece->gameTag() & 127); // Treat as enum
                // Generate moves based on piece type
                switch (type) {
                    case Pawn:
                        generatePawnMoves(x, y, moves, piece->gameTag());
                        break;
                    case Knight:
                        generateKnightMoves(x, y, moves);
                        break;
                    case Bishop:
                        generateSlidingPieceMoves(x, y, moves, { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} }); // Diagonal moves
                        break;
                    case Rook:
                        generateSlidingPieceMoves(x, y, moves, { {0, -1}, {0, 1}, {-1, 0}, {1, 0} }); // Cardinal moves
                        break;
                    case Queen:
                        generateSlidingPieceMoves(x, y, moves, { 
                            {0, -1}, {0, 1}, {-1, 0}, {1, 0},     // Cardinal
                            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}   // Diagonal
                        });
                        break;
                    case King:
                        generateKingMoves(x, y, moves);
                        break;
                    default:
                        break; // NoPiece or invalid type
                }
            }
        }
    }
    return moves;
}

bool Chess::isOnBoard(int x, int y) const {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

void Chess::generateKingMoves(int x, int y, std::vector<std::array<int, 4>>& moves) {
    const int kingOffsets[8][2] = {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0},    // Cardinal
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}  // Diagonal
    };

    for (const auto& offset : kingOffsets) {
        int nx = x + offset[0], ny = y + offset[1];
        if (isOnBoard(nx, ny) && (!_grid[ny][nx].bit() || _grid[ny][nx].bit()->getOwner() != getCurrentPlayer())) {
            moves.push_back({ny, nx, y, x});
        }
    }
}

void Chess::generateSlidingPieceMoves(int x, int y, std::vector<std::array<int, 4>>& moves, const std::vector<std::pair<int, int>>& directions) {
    for (const auto& dir : directions) {
        int nx = x, ny = y;

        while (true) {
            nx += dir.first;
            ny += dir.second;

            if (!isOnBoard(nx, ny)) break;

            Bit* target = _grid[ny][nx].bit();
            if (!target) {
                moves.push_back({ny, nx, y, x}); // Empty square
            } else {
                if (target->getOwner() != getCurrentPlayer()) {
                    moves.push_back({ny, nx, y, x}); // Capture opponent
                }
                break; // Stop at first collision
            }
        }
    }
}

void Chess::generateKnightMoves(int x, int y, std::vector<std::array<int, 4>>& moves) {
    const int knightOffsets[8][2] = {
        {-2, 1}, {-1, 2}, {1, 2}, {2, 1},
        {2, -1}, {1, -2}, {-1, -2}, {-2, -1}
    };

    for (const auto& offset : knightOffsets) {
        int nx = x + offset[0], ny = y + offset[1];
        if (isOnBoard(nx, ny) && (!_grid[ny][nx].bit() || _grid[ny][nx].bit()->getOwner() != getCurrentPlayer())) {
            moves.push_back({ny, nx, y, x});
        }
    }
}

void Chess::generatePawnMoves(int x, int y, std::vector<std::array<int, 4>>& moves, int color) {
    int direction = (color == Pawn) ? 1 : -1; // White moves up the board, Black moves down

    // Forward moves
    if (isOnBoard(x+ direction, y) && !_grid[y][x+ direction].bit()) {
        moves.push_back({y, x+ direction, y, x});

        // Two-step move from starting rank
        if ((color == Pawn && x == 1) || (color != Pawn && x == 6)) {
            if (!_grid[y][x+ 2 * direction].bit()) {
                moves.push_back({y, x+ 2 * direction, y, x});
            }
        }
    }

    // Captures (diagonals)
    if (isOnBoard(x + direction, y -1) && _grid[y -1][x + direction].bit() && _grid[y -1][x+ direction].bit()->getOwner() != getCurrentPlayer()) {
        moves.push_back({y -1, x+ direction, y, x});
    }
    if (isOnBoard(x + direction, y +1) && _grid[y + 1][x + direction].bit() && _grid[y + 1][x + direction].bit()->getOwner() != getCurrentPlayer()) {
        moves.push_back({y +1, x + direction, y, x});
    }
}