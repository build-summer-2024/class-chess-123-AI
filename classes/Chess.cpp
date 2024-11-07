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
            /*_grid[i][j].setGameTag(0);
            piece[0] = bitToPieceNotation(i,j);
            _grid[i][j].setNotation(piece);*/
            if((i == 0 && j == 0)||(i == 7 && j == 0)||(i == 7&&j==7)||(i==0&&j==7)){
                setBoardPiece(&_grid[i][j],j,i,Rook);
            }else if((i == 2 && j == 0)||(i == 5 && j == 0)||(i == 5&&j==7)||(i==2&&j==7)){
                setBoardPiece(&_grid[i][j],j,i,Bishop);
            }
            else if((i == 1 && j == 0)||(i == 6 && j == 0)||(i == 6&&j==7)||(i==1&&j==7)){
                setBoardPiece(&_grid[i][j],j,i,Knight);
            }
            else if((i == 3 && j == 0)||(i == 3 && j == 7)){
                setBoardPiece(&_grid[i][j],j,i,Queen);
            }
            else if((i == 4 && j == 0)||(i == 4&& j == 7)){
                setBoardPiece(&_grid[i][j],j,i,King);
            }
            else if(j == 1 || j == 6){
                setBoardPiece(&_grid[i][j],j==1?0:1,i,Pawn);
            }
            
        }
    }
    startGame();
    auto moves = generateMoves();
    std::cout<<moves.size()<<" starting array size"<<std::endl;
}

void Chess::setBoardPiece(BitHolder *holder,int pos,int posx,ChessPiece piece){
    Bit* newBit = PieceForPlayer(pos <=0 ? 1 : 0,piece);
    newBit->setPosition(holder->getPosition());
    newBit->setOwner(getPlayerAt(pos <=0 ? 1 : 0));
    newBit->setParent(&_grid[posx][pos]);
    newBit->setGameTag(piece+(pos <=0 ? 0 : 128));
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
    for(int x = 0; x < possMoves.size();x++){
        if(&dst == &(getHolderAt(possMoves[x][0],possMoves[x][1]))&& &src==&(getHolderAt(possMoves[x][2],possMoves[x][3]))){
            return true;
        }
    }

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
    int directionOffesets[8][2] = {{0,-1},{0,1},{1,0},{-1,0},{1,-1},{-1,1},{-1,-1},{1,1}};//The possible transformations
    std::vector<std::array<int,4>> moves;
    int edges[8][8][8];
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            int numNorth = x;
            int numSouth = 7-x;
            int numWest = 7-y;
            int numEast = y;
            
            edges[y][x][0] = numNorth;
            edges[y][x][1] = numSouth;
            edges[y][x][2] = numWest;
            edges[y][x][3] = numEast;
            edges[y][x][4] = std::min(numNorth,numWest);
            edges[y][x][5] = std::min(numSouth,numEast);
            edges[y][x][6] = std::min(numNorth,numEast);
            edges[y][x][7] = std::min(numSouth,numWest);
        }
    }
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            ChessSquare *piece = &_grid[y][x];//Get current piece
            //std::cout<<piece->bit()<<" gridPiece: " <<_grid[y][x].bit()<<std::endl;
            if(piece->bit()&&piece->bit()->getOwner()==getCurrentPlayer()){//If piece is owned by current player, otherwise we don't care
                if(piece->bit()->gameTag()==King||piece->bit()->gameTag()==King+128){
                    auto notPoss = generateKingMoves(getPlayerAt(0)==getCurrentPlayer()?1:0);
                    for(int i = 0;i<8;i++){
                        bool poss = true;
                        if(y+directionOffesets[i][0]>=0&&y+directionOffesets[i][0]<8&&x+directionOffesets[i][1]<8&&x+directionOffesets[i][8]>=0){
                        for(int j =0; j < notPoss.size();j++){
                            //std::cout<<notPoss[j][2]<<" "<<notPoss[j][3]<<std::endl;
                            if(_grid[notPoss[j][2]][notPoss[j][3]].bit()->getOwner()!=getCurrentPlayer()&&notPoss[j][0]==y+directionOffesets[i][0]&&notPoss[j][1]==x+directionOffesets[i][1]){
                                //std::cout<<"working"<<std::endl;
                                poss = false;
                            }
                        }
                        if(poss){
                            if(_grid[y+directionOffesets[i][0]][x+directionOffesets[i][1]].bit()&&_grid[y+directionOffesets[i][0]][x+directionOffesets[i][1]].bit()->getOwner()!=getCurrentPlayer()){
                                moves.push_back({y+directionOffesets[i][0],x+directionOffesets[i][1],y,x});
                            }
                        }
                        }
                    }
                }
                if(piece->bit()->gameTag()==Pawn||piece->bit()->gameTag()==Pawn+128){
                    //std::cout<<y<<" "<<x<<" slots"<<std::endl;
                    if(piece->bit()->gameTag()==Pawn+128){
                        if(x-1>=0){
                            if(x==6){
                                moves.push_back({y,x-2,y,x});
                            }
                            if(!_grid[y][x-1].bit()){
                                moves.push_back({y,x-1,y,x});
                            }
                            if(y-1>=0&&_grid[y-1][x-1].bit()&&_grid[y-1][x-1].bit()->getOwner()!=getCurrentPlayer()){
                                moves.push_back({y-1,x-1,y,x});
                            }
                            if(y+1<8&&_grid[y+1][x-1].bit()&&_grid[y+1][x-1].bit()->getOwner()!=getCurrentPlayer()){
                                moves.push_back({y+1,x-1,y,x});
                            }
                        }
                    }else{
                        if(x+1<8){
                            if(x==1){
                                moves.push_back({y,x+2,y,x});
                            }
                            if(!_grid[y][x+1].bit()){
                                moves.push_back({y,x+1,y,x});
                            }
                            if(y-1>=0&&_grid[y-1][x+1].bit()&&_grid[y-1][x+1].bit()->getOwner()!=getCurrentPlayer()){
                                moves.push_back({y-1,x+1,y,x});
                            }
                            if(y+1<8&&_grid[y+1][x+1].bit()&&_grid[y+1][x+1].bit()->getOwner()!=getCurrentPlayer()){
                                moves.push_back({y+1,x+1,y,x});
                            }
                        }
                    }
                }
                if(piece->bit()->gameTag()==Knight||piece->bit()->gameTag()==Knight+128){
                    int KnMoves[8][2] = {{-2,1},{-2,-1},{2,1},{2,-1,},{1,-2},{1,2},{-1,2},{-1,-2}};
                    for(int i = 0; i < 8;i++){
                        if(y+KnMoves[i][0]>=0&&y+KnMoves[i][0]<8&&x+KnMoves[i][1]>=0&&x+KnMoves[i][1]<8&&(!_grid[y+KnMoves[i][0]][x+KnMoves[i][1]].bit()||_grid[y+KnMoves[i][0]][x+KnMoves[i][1]].bit()->getOwner()!=getCurrentPlayer())){
                            moves.push_back({y+KnMoves[i][0],x+KnMoves[i][1],y,x});
                        }
                    }
                }
                int start = 0;
                int end = 8;
                if(piece->bit()->gameTag()==Bishop||piece->bit()->gameTag()==Bishop+128){
                    start = 4;

                }
                else if(piece->bit()->gameTag()==Rook || piece->bit()->gameTag()==Rook+128){
                    end = 4;
                }
                if(piece->bit()->gameTag()==Rook || piece->bit()->gameTag()==Rook+128||piece->bit()->gameTag()==Bishop||piece->bit()->gameTag()==Bishop+128||piece->bit()->gameTag()==Queen+128||piece->bit()->gameTag()==Queen){
                    for(int i = start; i< end;i++){
                        for(int n = 0; n<edges[y][x][i];n++){
                            int targetSquare[2] = {(y+(directionOffesets[i][0]*(n+1))),(x+(directionOffesets[i][1]*(n+1)))};
                            ChessSquare *pieceOnSquare = &_grid[targetSquare[0]][targetSquare[1]];
                            //std::cout<<targetSquare[0]<<" "<<targetSquare[1]<<" Target square "<<directionOffesets[i][0]<<" "<< directionOffesets[i][1]<<" offset "<<i <<" i "<<n<<"n "<<y<<" "<<x<<" startSquares "<<edges[y][x][i]<<" edges"<<std::endl;
                            if(pieceOnSquare->bit()&&pieceOnSquare->bit()->getOwner()==getCurrentPlayer()){
                                break;
                            }
                            moves.push_back({targetSquare[0],targetSquare[1],y,x});
                            if(pieceOnSquare->bit()&&pieceOnSquare->bit()->getOwner()!= getCurrentPlayer()){
                                break;
                            }
                    }
                }
            }
            }
        }
    }

    return moves;
}

std::vector<std::array<int,4>> Chess::generateKingMoves(int playerNumber){
   int directionOffesets[8][2] = {{0,-1},{0,1},{1,0},{-1,0},{1,-1},{-1,1},{-1,-1},{1,1}};//The possible transformations
    std::vector<std::array<int,4>> moves;
    int edges[8][8][8];
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            int numNorth = x;
            int numSouth = 7-x;
            int numWest = 7-y;
            int numEast = y;
            
            edges[y][x][0] = numNorth;
            edges[y][x][1] = numSouth;
            edges[y][x][2] = numWest;
            edges[y][x][3] = numEast;
            edges[y][x][4] = std::min(numNorth,numWest);
            edges[y][x][5] = std::min(numSouth,numEast);
            edges[y][x][6] = std::min(numNorth,numEast);
            edges[y][x][7] = std::min(numSouth,numWest);
        }
    }
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            ChessSquare *piece = &_grid[y][x];//Get current piece
            //std::cout<<piece->bit()<<" gridPiece: " <<_grid[y][x].bit()<<std::endl;
            if(piece->bit()&&piece->bit()->getOwner()==getPlayerAt(playerNumber)){//If piece is owned by current player, otherwise we don't care
                
                if(piece->bit()->gameTag()==Pawn||piece->bit()->gameTag()==Pawn+128){
                    //std::cout<<y<<" "<<x<<" slots"<<std::endl;
                    if(piece->bit()->gameTag()==Pawn+128){
                        if(x-1>=0){
                            
                            if(y-1>=0){
                                //std::cout<<"working"<<std::endl;
                                moves.push_back({y-1,x-1,y,x});
                            }
                            if(y+1<8){
                                //std::cout<<"working"<<std::endl;
                                moves.push_back({y+1,x-1,y,x});
                            }
                        }
                    }else{
                        if(x+1<8){
                            
                            if(y-1>=0){
                                //std::cout<<"working"<<std::endl;
                                moves.push_back({y-1,x+1,y,x});
                            }
                            if(y+1<8){
                                //std::cout<<"working"<<std::endl;
                                moves.push_back({y+1,x+1,y,x});
                            }
                        }
                    }
                }
                if(piece->bit()->gameTag()==Knight||piece->bit()->gameTag()==Knight+128){
                    int KnMoves[8][2] = {{-2,1},{-2,-1},{2,1},{2,-1,},{1,-2},{1,2},{-1,2},{-1,-2}};
                    for(int i = 0; i < 8;i++){
                        if(y+KnMoves[i][0]>=0&&y+KnMoves[i][0]<8&&x+KnMoves[i][1]>=0&&x+KnMoves[i][1]<8&&(!_grid[y+KnMoves[i][0]][x+KnMoves[i][1]].bit()||_grid[y+KnMoves[i][0]][x+KnMoves[i][1]].bit()->getOwner()!=getPlayerAt(playerNumber))){
                            //std::cout<<"working"<<std::endl;
                            moves.push_back({y+KnMoves[i][0],x+KnMoves[i][1],y,x});
                        }
                    }
                }
                int start = 0;
                int end = 8;
                if(piece->bit()->gameTag()==Bishop||piece->bit()->gameTag()==Bishop+128){
                    start = 4;

                }
                else if(piece->bit()->gameTag()==Rook || piece->bit()->gameTag()==Rook+128){
                    end = 4;
                }
                if(piece->bit()->gameTag()==Rook || piece->bit()->gameTag()==Rook+128||piece->bit()->gameTag()==Bishop||piece->bit()->gameTag()==Bishop+128||piece->bit()->gameTag()==Queen+128||piece->bit()->gameTag()==Queen){
                    for(int i = start; i< end;i++){
                        for(int n = 0; n<edges[y][x][i];n++){
                            int targetSquare[2] = {(y+(directionOffesets[i][0]*(n+1))),(x+(directionOffesets[i][1]*(n+1)))};
                            ChessSquare *pieceOnSquare = &_grid[targetSquare[0]][targetSquare[1]];
                            //std::cout<<targetSquare[0]<<" "<<targetSquare[1]<<" Target square "<<directionOffesets[i][0]<<" "<< directionOffesets[i][1]<<" offset "<<i <<" i "<<n<<"n "<<y<<" "<<x<<" startSquares "<<edges[y][x][i]<<" edges"<<std::endl;
                            if(pieceOnSquare->bit()&&pieceOnSquare->bit()->getOwner()==getPlayerAt(playerNumber)){
                                break;
                            }
                            moves.push_back({targetSquare[0],targetSquare[1],y,x});
                            if(pieceOnSquare->bit()&&pieceOnSquare->bit()->getOwner()!= getPlayerAt(playerNumber)){
                                break;
                            }
                    }
                }
            }
            }
        }
    }

    return moves;
}
