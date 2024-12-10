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
    setAIPlayer(1);
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
    if(isCheckmate(getPlayerAt(0))){
        endTurn();
        return getPlayerAt(1);
    }
    else if(isCheckmate(getPlayerAt(1))){
        endTurn();
        return getPlayerAt(0);
    }
    return nullptr;
}

bool Chess::isCheckmate(Player* playerColor) {
    // First, check if the king is in check
    auto [kingX, kingY] = findKingPosition(playerColor);
    if (!isSquareUnderAttack(kingX, kingY, (playerColor == getPlayerAt(0)) ? getPlayerAt(1) : getPlayerAt(0))) {
        return false; // King is not in check, hence not checkmate
    }

    // If the king is in check, we generate all available moves for the player
    std::vector<std::array<int, 4>> moves = generateMoves();
    return moves.empty(); // If there are no moves left, it's checkmate
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
    std::array<int, 4> bestMove = getBestMove(3);
    //std::cout<<bestMove[0]<<" "<<bestMove[1]<<" "<<bestMove[2]<<bestMove[3]<<std::endl;
    //_grid[bestMove[0]][bestMove[1]].setBit(_grid[bestMove[2]][bestMove[3]].bit());
    //std::cout<<_grid[bestMove[0]][bestMove[1]].bit()<<" "<<bestMove[0]<<" "<<bestMove[1]<<" "<<_grid[bestMove[2]][bestMove[3]].bit()<<" "<<bestMove[2]<<" "<<bestMove[3]<<std::endl;
    int currPieceTag = _grid[bestMove[2]][bestMove[3]].bit()->gameTag();
    setBoardPiece(& _grid[bestMove[0]][bestMove[1]],(currPieceTag>=128?static_cast<ChessPiece>(currPieceTag-128):static_cast<ChessPiece>(currPieceTag)),currPieceTag>=128?1:0,bestMove[0],bestMove[1]);
    _grid[bestMove[2]][bestMove[3]].destroyBit();
    bitMovedFromTo(*_grid[bestMove[0]][bestMove[1]].bit(),_grid[bestMove[0]][bestMove[1]],_grid[bestMove[2]][bestMove[3]]);
}

std::vector<std::array<int,4>> Chess::generateMoves(){
    std::vector<std::array<int, 4>> allMoves, legalMoves;

    // Generate all possible moves
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            ChessSquare* square = &_grid[y][x];
            Bit* piece = square->bit();

            if (piece && piece->getOwner() == getCurrentPlayer()) {
                ChessPiece type = static_cast<ChessPiece>(piece->gameTag() & 127);

                // Determine piece type and add potential moves to allMoves
                switch (type) {
                    case Pawn:
                        generatePawnMoves(x, y, allMoves, piece->gameTag());
                        break;
                    case Knight:
                        generateKnightMoves(x, y, allMoves,getCurrentPlayer());
                        break;
                    case Bishop:
                        generateSlidingPieceMoves(x, y, allMoves, { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} },getCurrentPlayer());
                        break;
                    case Rook:
                        generateSlidingPieceMoves(x, y, allMoves, { {0, -1}, {0, 1}, {-1, 0}, {1, 0} },getCurrentPlayer());
                        break;
                    case Queen:
                        generateSlidingPieceMoves(x, y, allMoves, { 
                            {0, -1}, {0, 1}, {-1, 0}, {1, 0}, 
                            {-1, -1}, {-1, 1}, {1, -1}, {1, 1} 
                        },getCurrentPlayer());
                        break;
                    case King:
                        generateKingMoves(x, y, allMoves,getCurrentPlayer());
                        break;
                    default:
                        break; // NoPiece or invalid type
                }
            }
        }
    }

    // Validate each move for legality
    for (const auto& move : allMoves) {
        if (isMoveLegal(move)) {
            legalMoves.push_back(move);
        }
    }

    return legalMoves;
}

bool Chess::isOnBoard(int x, int y) const {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

void Chess::generateKingMoves(int x, int y, std::vector<std::array<int, 4>>& moves,Player* playerColor) {
    const int kingOffsets[8][2] = {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0},    // Cardinal
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}  // Diagonal
    };

    for (const auto& offset : kingOffsets) {
        int nx = x + offset[0], ny = y + offset[1];
        if (isOnBoard(nx, ny) && (!_grid[ny][nx].bit() || _grid[ny][nx].bit()->getOwner() != playerColor)) {
            moves.push_back({ny, nx, y, x});
        }
    }
}

void Chess::generateSlidingPieceMoves(int x, int y, std::vector<std::array<int, 4>>& moves, const std::vector<std::pair<int, int>>& directions,Player* playerColor) {
    for (const auto& dir : directions) {
        int nx = x, ny = y;

        while (true) {
            nx += dir.first;
            ny += dir.second;

            if (!isOnBoard(nx, ny)) break; // Stop if out of bounds

            ChessSquare* square = &_grid[ny][nx];
            Bit* target = square->bit();
            moves.push_back({ny, nx, y, x});
            if (target) {
                /*if (target->getOwner() != playerColor) {
                    // You can capture an opponent's piece
                    moves.push_back({ny, nx, y, x});
                }
                // Stop sliding if there's a piece here (both ally and enemy)*/
                break;
            }

            // Add this square to attacks
            //moves.push_back({ny, nx, y, x});

        }
    }
}

void Chess::generateKnightMoves(int x, int y, std::vector<std::array<int, 4>>& moves,Player* playerColor) {
    const int knightOffsets[8][2] = {
        {-2, 1}, {-1, 2}, {1, 2}, {2, 1},
        {2, -1}, {1, -2}, {-1, -2}, {-2, -1}
    };

    for (const auto& offset : knightOffsets) {
        int nx = x + offset[0], ny = y + offset[1];
        if (isOnBoard(nx, ny) && (!_grid[ny][nx].bit() || _grid[ny][nx].bit()->getOwner() != playerColor)) {
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

bool Chess::isSquareUnderAttack(int x, int y, Player* attackerColor) {
    // Loop through all board squares to find attackers
    for (int srcY = 0; srcY < 8; srcY++) {
        for (int srcX = 0; srcX < 8; srcX++) {
            ChessSquare* square = &_grid[srcY][srcX];
            Bit* piece = square->bit();

            if (piece && piece->getOwner() == attackerColor) {
                // Generate attacks for the piece
                std::vector<std::array<int, 4>> attacks;
                ChessPiece type = static_cast<ChessPiece>(piece->gameTag() & 127);
                switch (type) {
                    case Pawn:
                        generatePawnMoves(srcX, srcY, attacks, piece->gameTag());
                        break;
                    case Knight:
                        generateKnightMoves(srcX, srcY, attacks,attackerColor);
                        break;
                    case Bishop:
                        generateSlidingPieceMoves(srcX, srcY, attacks, { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} },attackerColor);
                        break;
                    case Rook:
                        generateSlidingPieceMoves(srcX, srcY, attacks, { {0, -1}, {0, 1}, {-1, 0}, {1, 0} },attackerColor);
                        break;
                    case Queen:
                        generateSlidingPieceMoves(srcX, srcY, attacks, { 
                            {0, -1}, {0, 1}, {-1, 0}, {1, 0}, 
                            {-1, -1}, {-1, 1}, {1, -1}, {1, 1} },attackerColor);
                        break;
                    case King:
                        generateKingMoves(srcX, srcY, attacks,attackerColor); // Returns single-step moves
                        break;
                }

                // Check if the target square (x, y) is included in the attacks
                for (const auto& attack : attacks) {
                    if (attack[0] == y && attack[1] == x) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

std::pair<int, int> Chess::findKingPosition(Player* playerColor) {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Bit* piece = _grid[y][x].bit();
            if (piece && (piece->gameTag() == (King)||piece->gameTag() == (King+128))) {
                if(piece->getOwner()==playerColor){
                    return {x, y};
                }
            }
        }
    }
    return {-1, -1}; // King not found (should never happen in valid chess game)
}

bool Chess::isMoveLegal(const std::array<int, 4>& move) {
    int srcX = move[3], srcY = move[2];
    int dstX = move[1], dstY = move[0];

    // Get the piece being moved
    Bit* piece = _grid[srcY][srcX].bit();
    if (!piece) return false; // No piece to move

    // Find the current player's king position
    Player* playerColor = piece->getOwner();
    auto [kingX, kingY] = findKingPosition(playerColor);

    if(_grid[dstY][dstX].bit()&&_grid[dstY][dstX].bit()->getOwner()==playerColor){
        return false;
    }

    // Special case: if the king itself is moving
    if (piece->gameTag() == King || piece->gameTag() == King + 128) {
        kingX = dstX;
        kingY = dstY; // Update king position for the move
        if (isSquareUnderAttack(kingX, kingY, getPlayerAt(0)==playerColor?getPlayerAt(1):getPlayerAt(0))) {
            return false;
        }
    }
    // Check if the king is under attack on the original square
    if (isSquareUnderAttack(kingX, kingY, getPlayerAt(0)==playerColor?getPlayerAt(1):getPlayerAt(0))) {
        std::cerr << "King is already in check!" << std::endl;
        if (!canMoveBlockAttack(kingX, kingY, playerColor)) {
            return false; // No blocking moves available
        }
    }

    // For other pieces, determine if moving the piece exposes the king to attack
    if (wouldExposeKing(srcX, srcY, dstX, dstY)) {
        return false;
    }

    return true; // The move is legal
}

bool Chess::wouldExposeKing(int srcX, int srcY, int dstX, int dstY) {
    // Find the current player's king position
    Player* playerColor = _grid[srcY][srcX].bit()->getOwner();
    auto [kingX, kingY] = findKingPosition(playerColor);

    if (kingX == -1 || kingY == -1) {
        std::cerr << "Error: King not found!" << std::endl;
        return false; // Failsafe
    }

    // Special case: if the piece being moved is the king
    if (srcX == kingX && srcY == kingY) {
        return false; // `isMoveLegal()` will handle king moves separately
    }

    // Check all attack lines around the king
    const int directionOffsets[8][2] = {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0},  // Cardinal: N, S, W, E
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1} // Diagonal
    };

    for (const auto& dir : directionOffsets) {
        int nx = kingX, ny = kingY;
        bool foundFriendlyPiece = false;
        int pinnedPieceX = -1, pinnedPieceY = -1; // Location of a potentially pinned piece

        while (true) {
            nx += dir[0];
            ny += dir[1];

            if (!isOnBoard(nx, ny)) break;

            ChessSquare* square = &_grid[ny][nx];
            Bit* piece = square->bit();

            if (piece) {
                if (piece->getOwner() == playerColor) {
                    // First friendly piece encountered: could be pinned
                    if (foundFriendlyPiece) break; // More than one friendly piece: not a pin
                    foundFriendlyPiece = true;
                    pinnedPieceX = nx;
                    pinnedPieceY = ny;
                } else {
                    // Opposing piece: Check for pinning
                    ChessPiece type = static_cast<ChessPiece>(piece->gameTag() & 127);
                    bool validPinner = false;

                    // Rook-like attacks on cardinals; Bishop-like on diagonals; Queen on both
                    if ((dir[0] == 0 || dir[1] == 0) && (type == Rook || type == Queen)) {
                        validPinner = true;
                    }
                    if ((dir[0] != 0 && dir[1] != 0) && (type == Bishop || type == Queen)) {
                        validPinner = true;
                    }

                    if (!foundFriendlyPiece && validPinner) {
                        break; // No friendly piece between king and attacker: king is directly under attack
                    } else if (foundFriendlyPiece && validPinner) {
                        // Friendly piece is pinned
                        if (pinnedPieceX == srcX && pinnedPieceY == srcY) {
                            // Moving along the line of attack is okay; any other move isn't
                            if (dstX != nx || dstY != ny) {
                                return true;
                            }
                        }
                    }
                    break; // Stop tracing in this direction when an attacker is found
                }
            }
        }
    }

    return false; // The move does not expose the king
}

int Chess::evaluateBoard() {
    int score = 0;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Bit* piece = _grid[y][x].bit();
            if (piece) {
                int value = 0;
                // Value calculation based on piece type
                switch (piece->gameTag() & 127) { // Masking to get type
                    case Pawn: value = 1; break;
                    case Knight: value = 3; break;
                    case Bishop: value = 3; break;
                    case Rook: value = 5; break;
                    case Queen: value = 9; break;
                    case King: value = 0; break; // Kings don't score in a material sense
                }
                // Adjust score based on ownership
                score += (piece->getOwner() == getPlayerAt(0)) ? -value : value; // Black is maximizing
            }
        }
    }
    return score;
}

int Chess::negamax(int depth, int alpha, int beta) {
    //std::cout<<"working"<<std::endl;
    if (depth == 0) {
        return evaluateBoard(); // Return the evaluation for this board state
    }

    int maxEval = -INT_MAX; // Seeking to maximize the evaluation for Black
    std::vector<std::array<int, 4>> moves = generateMoves(); // Get all possible moves

    for (const auto& move : moves) {
        // Use simulateMove to check the legality of this move
        auto [isLegal, boardState] = simulateMove(move);
        if (!isLegal) continue; // Skip illegal moves

        // Negamax evaluation of the resulting position
        int eval = -boardState.negamax(depth - 1, -beta, -alpha);

        maxEval = std::max(maxEval, eval);
        alpha = std::max(alpha, eval);

        // Alpha-Beta Pruning
        if (beta <= alpha) {
            break; // Cutoff
        }
    }
    return maxEval;
}

std::array<int, 4> Chess::getBestMove(int depth) {
    int bestScore = -INT_MAX;
    std::array<int, 4> bestMove = {-1, -1, -1, -1}; // Invalid default

    std::vector<std::array<int, 4>> moves = generateMoves();
    
    for (const auto& move : moves) {
        // Use simulateMove to check the legality of this move
        auto [isLegal, boardState] = simulateMove(move);
        if (!isLegal) continue; // Skip illegal moves

        // Evaluate this position
        int score = -boardState.negamax(depth - 1, -INT_MAX, INT_MAX);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move; // Update best move
        }
    }

    return bestMove; // Return the best move found
}

// Function to simulate a move
std::pair<bool, Chess> Chess::simulateMove(const std::array<int, 4>& move) {
    int srcX = move[3], srcY = move[2];
    int dstX = move[1], dstY = move[0];

    // Create a copy of the current board
    Chess boardCopy;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            boardCopy._grid[y][x] = _grid[y][x]; // Copy ChessSquare
            
            // If the square contains a bit (a piece), make a deep copy of it
            if (_grid[y][x].bit()) {
                Bit* originalBit = _grid[y][x].bit();
                boardCopy._grid[y][x].setBit(new Bit(*originalBit)); // Assuming Bit has a proper copy constructor
            }
        }
    }

    // Fetch the piece and destination holder for move simulation
    ChessSquare* src = &boardCopy._grid[srcY][srcX];
    ChessSquare* dst = &boardCopy._grid[dstY][dstX];

    Bit* movingPiece = src->bit();
    if (!movingPiece) return {false, boardCopy}; // Invalid move

    // Move the piece
    dst->setBit(movingPiece);
    src->setBit(nullptr);

    // Checking if the king is still safe after the move
    auto [kingX, kingY] = boardCopy.findKingPosition(movingPiece->getOwner());
    bool inCheck = boardCopy.isSquareUnderAttack(kingX, kingY, getPlayerAt(0)==getCurrentPlayer()?getPlayerAt(1):getPlayerAt(0));

    // Rollback the move
    src->setBit(movingPiece);
    dst->setBit(nullptr);

    return {!inCheck, boardCopy}; // Return true if the king is safe and copy the board state
}

bool Chess::canMoveBlockAttack(int kingX, int kingY, Player* playerColor) {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            ChessSquare* square = &_grid[y][x];
            Bit* piece = square->bit();
            if (piece && piece->getOwner() == playerColor) {
                std::vector<std::array<int, 4>> moves;
                ChessPiece type = static_cast<ChessPiece>(piece->gameTag() & 127);
                
                // Generate moves based on piece type
                switch (type) {
                    case Rook:
                        generateSlidingPieceMoves(x, y, moves, { {0, -1}, {0, 1}, {-1, 0}, {1, 0} },playerColor);
                        break;
                    case Bishop:
                        generateSlidingPieceMoves(x, y, moves, { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} },playerColor);
                        break;
                    case Queen:
                        generateSlidingPieceMoves(x, y, moves, { 
                            {0, -1}, {0, 1}, {-1, 0}, {1, 0}, 
                            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
                        },playerColor);
                        break;
                    case Knight:
                        generateKnightMoves(x, y, moves,playerColor);
                        break;
                    case Pawn:
                        generatePawnMoves(x,y,moves,piece->gameTag());
                        break;
                }

                // Check if any move from this piece can block the attack on the king
                for (const auto& move : moves) {
                    if (canBlockAttack(move, kingX, kingY)) {
                        std::cout<<"blockable"<<std::endl;
                        return true; 
                    }
                }
            }
        }
    }
    return false; // No blocking moves found
}
bool Chess::canBlockAttack(const std::array<int, 4>& move, int kingX, int kingY) {
    int srcX = move[3]; // Original X position of the piece being moved
    int srcY = move[2]; // Original Y position of the piece being moved
    int dstX = move[1]; // Target X position for the move
    int dstY = move[0]; // Target Y position for the move

    // Identify the piece being moved
    Bit* movingPiece = _grid[srcY][srcX].bit();

    // Check the direction of the move
    int directionX = dstX - srcX;
    int directionY = dstY - srcY;

    // Normalize the step for moving toward the king
    int stepX = (directionX != 0) ? (directionX > 0 ? 1 : -1) : 0; // Step in X
    int stepY = (directionY != 0) ? (directionY > 0 ? 1 : -1) : 0; // Step in Y

    // Move towards the king position to check if any piece will block the attack
    int x = dstX + stepX; // Start immediately after the destination
    int y = dstY + stepY;

    // Capture potential attacking pieces
    while (x != kingX || y != kingY) {
        // Check if we're out of bounds
        if (!isOnBoard(x, y)) {
            break; // Out of bounds
        }

        Bit* attacker = _grid[y][x].bit();
        if (attacker) {
            // If there is an attacking piece, check if it can target the king
            // Generate possible attack squares for the attacker
            std::vector<std::array<int, 4>> attackMoves;

            // Determine attack type of the piece
            ChessPiece type = static_cast<ChessPiece>(attacker->gameTag() & 127);
            switch (type) {
                case Pawn:
                    // Add diagonal attack squares for the opponent's pawn
                    attackMoves.push_back({y + (attacker->getOwner() == getPlayerAt(0) ? -1 : 1), x - 1, 0, 0});
                    attackMoves.push_back({y + (attacker->getOwner() == getPlayerAt(0) ? -1 : 1), x + 1, 0, 0});
                    break;
                case Knight:
                    generateKnightMoves(x, y, attackMoves,attacker->getOwner());
                    break;
                case Bishop:
                    generateSlidingPieceMoves(x, y, attackMoves, { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} },attacker->getOwner());
                    break;
                case Rook:
                    generateSlidingPieceMoves(x, y, attackMoves, { {0, -1}, {0, 1}, {-1, 0}, {1, 0} },attacker->getOwner());
                    break;
                case Queen:
                    generateSlidingPieceMoves(x, y, attackMoves, {
                        {0, -1}, {0, 1}, {-1, 0}, {1, 0},
                        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
                    },attacker->getOwner());
                    break;
                case King:
                    // Generate king's potential moves (though kings shouldn't block)
                    generateKingMoves(x, y, attackMoves,attacker->getOwner());
                    break;
                default:
                    break;
            }

            // If the attack square impacts the king, the move cannot block
            for (const auto& attack : attackMoves) {
                if (attack[0] == kingY && attack[1] == kingX) {
                    return true; // Successful blocking move
                }
            }
            break; // Exit if an attacker piece is hit
        }
        // Move to the next square in the same direction
        x += stepX;
        y += stepY;
    }

    return false; // There were no valid blocking opportunities found
}