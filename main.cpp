#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <ctime>
using namespace std;

class Cell;
class Board;
class Game;

class Cell{
    public:
        Cell();
        Cell(bool bomb, Board* board);
        char getValue() const;
        void setValue(char value);
        bool getFlag() const;
        void setFlag(bool value);
        bool isBomb() const;
        bool isOpened() const;
        void openCell();
        char getDisplayValue();
        void setDisplayValue(char val);
    private: 
        Board* m_board;
        char m_value;
        char m_display_value;
        bool m_opened;
        bool m_flagged;
        bool m_bomb;
};


class Board{
    public:
        Board(Game* game, int rows, int cols, int bombs);
        ~Board();
        void calculateValue(int row, int col);
        int getRows();
        int getCols();
        void bombOpened();
        void displayBoard();
        void flagCell(int row, int col);
        void processMove(int row, int col);
        bool unopened(int row, int col) const;
        void handleFirstClick(int row, int col);
        void floodFill(int row, int col);
        bool checkGameStatus();
        bool isNeighbor(int r1, int c1, int r2, int c2);
    private: 
        Game* m_game;
        Cell** m_grid;
        int m_bombs;
        int m_rows;
        int m_cols;
        int bombsFlagged;
};

class Game{
   public:
        Game();
        void endGame(bool won);
        bool getGameVal();
        void move(int row, int col);
        bool checkMove(int row, int col);
        void play();
    private: 
        Board* m_board;
        bool m_gameOver;//might not need this
};


int main(){
    cout << "Welcome to Minesweeper!" << endl;
    cout << "-----------------------" << endl;
    Game g;
    g.play();
}

Cell::Cell()
{
    m_display_value = 'O';
    m_flagged = false;
}

Cell::Cell(bool bomb, Board *board)
{
    m_display_value = 'O';
    m_bomb = bomb;
    if(m_bomb) m_value = 'X';
    else m_value = '0';
    m_opened = false;
    m_flagged = false;
    m_board = board;
}

char Cell::getValue() const
{
    return m_value;
}

void Cell::setValue(char value)
{
    m_value = value;
}

bool Cell::getFlag() const
{
    return m_flagged;
}

void Cell::setFlag(bool value)
{
    m_flagged = value;
}

bool Cell::isBomb() const
{
    return m_bomb;
}

bool Cell::isOpened() const
{
    return m_opened;
}

char Cell::getDisplayValue()
{
    return m_display_value;
}

void Cell::setDisplayValue(char val)
{
    m_display_value = val;
}


void Cell::openCell()
{
    if(m_flagged) return;
    m_opened = true;
    if(isBomb()){
        m_board->bombOpened();
    }
    else{
        setDisplayValue(m_value);
    }
}

Board::Board(Game* game, int rows, int cols, int bombs)
{
    m_bombs = bombs;
    m_game = game;
    m_rows = rows;
    m_cols = cols;
    m_grid = new Cell*[rows];
    for(int i = 0; i < rows; i++){
        m_grid[i] = new Cell[cols];
        for(int j = 0; j < cols; j++){
            m_grid[i][j] = Cell(false, this);
        }
    }
    bombsFlagged = 0;
}

Board::~Board()
{
    for(int i = 0; i < m_rows; i++){
        delete[] m_grid[i];
    }

    delete[] m_grid;
}

void Board::calculateValue(int row, int col)
{
    int valueCounter = 0;
    if(col + 1 < m_cols && m_grid[row][col+1].isBomb()) valueCounter++;
    if(row + 1 < m_rows && m_grid[row+1][col].isBomb()) valueCounter++;
    if(row + 1 < m_rows && col + 1 < m_cols && m_grid[row+1][col+1].isBomb()) valueCounter++;
    if(row > 0 && m_grid[row-1][col].isBomb()) valueCounter++;
    if(col > 0 && m_grid[row][col-1].isBomb()) valueCounter++;
    if(row > 0 && col > 0 && m_grid[row-1][col-1].isBomb()) valueCounter++;
    if(row > 0 && col + 1 < m_cols && m_grid[row-1][col+1].isBomb()) valueCounter++;
    if(col > 0 && row + 1 < m_rows && m_grid[row+1][col-1].isBomb()) valueCounter++;
    if (m_grid[row][col].isBomb()) m_grid[row][col].setValue('X');
    else if(valueCounter > 0) m_grid[row][col].setValue(valueCounter + '0');
    else m_grid[row][col].setValue(' ');

}

int Board::getRows()
{
    return m_rows;
}

int Board::getCols()
{
    return m_cols;
}

void Board::bombOpened()
{
    m_game->endGame(false);
}

void Board::displayBoard()
{
    cout << "-----------------------" << endl << "     ";
    for(int i = -1; i < m_rows; i++){
        if(i > 9) cout << i << " | ";
        else if(i != -1) cout << i << "  | ";
        for(int j = 0; j < m_cols; j++){
            if(i == -1){
                if(j > 9) cout << j << " ";
                else cout << j << "  ";
            }
            else if(!m_game->getGameVal()){
                if(m_grid[i][j].getFlag()) cout << "F" << "  ";
                else cout << m_grid[i][j].getDisplayValue() << "  ";

            }
            else cout << m_grid[i][j].getValue() << "  ";
        }
        cout << "\n";
    }
    cout << "\n" << "Bombs Flagged: " << bombsFlagged << " / " << m_bombs << endl;
    cout << "-----------------------" << endl;
}

void Board::flagCell(int row, int col)
{
    if(!m_grid[row][col].isOpened()) {
        if(m_grid[row][col].getFlag()){
            m_grid[row][col].setFlag(false);
            bombsFlagged--;
        }
        else{
            m_grid[row][col].setFlag(true);
            bombsFlagged++;
        }
    }
    else cout << "Don't flag this. It's already open!" << endl;
}

void Board::processMove(int row, int col)
{
    if(m_grid[row][col].getFlag()) cout << "You can't open this. It is flagged!" << endl;
    else if(m_grid[row][col].getValue() == ' ') floodFill(row, col);
    else m_grid[row][col].openCell();

}

bool Board::unopened(int row, int col) const
{
    return !m_grid[row][col].isOpened();
}


void Board::handleFirstClick(int row, int col){
    int r = 0;
    int c = 0;
    srand(time(0));
    while(!m_game->checkMove(row, col)){
        cout << "Not the right format (row number, space, then column number)" << endl;
        cout << "Try again: ";
        cin >> row >> col;
    }

    bool** bombMatrix;
    bombMatrix = new bool*[m_rows];
    for(int i = 0; i < m_rows; i++){
        bombMatrix[i] = new bool[m_cols]();
    }

    int bombsPlaced = 0;
    for(int i = 0; i < m_bombs; i++){
        r = rand() % m_rows;
        c = rand() % m_cols;
        while(isNeighbor(r,c,row,col) || bombMatrix[r][c]){
            r = rand() % m_rows;
            c = rand() % m_cols;
        }
        bombsPlaced++;
        bombMatrix[r][c] = true;
    }


    for(int i = 0; i < m_rows; i++){
        for(int j = 0; j < m_cols; j++){
            m_grid[i][j] = Cell(bombMatrix[i][j], this);
        }
    }
    for(int i = 0; i < m_rows; i++){
        for(int j = 0; j < m_cols; j++){
            calculateValue(i, j);
        }
    }
    
    floodFill(row, col);

    for (int i = 0; i < m_rows; i++) {
        delete[] bombMatrix[i];
    }
    delete[] bombMatrix;
}



void Board::floodFill(int row, int col)
{
    if(row < 0 || col < 0 || row >= m_rows || col >= m_cols || !unopened(row, col) ||  m_grid[row][col].getFlag()) return;
    m_grid[row][col].openCell();
    if(m_grid[row][col].getValue() != ' ') return;
    floodFill(row+1,col);
    floodFill(row,col+1);
    floodFill(row+1,col+1);
    floodFill(row-1,col-1);   
    floodFill(row,col-1); 
    floodFill(row-1,col); 
    floodFill(row-1,col+1);
    floodFill(row+1,col-1); 
}



bool Board::checkGameStatus()
{
    for(int i = 0; i < m_rows; i++){
        for(int j = 0; j < m_cols; j++){
            if(!m_grid[i][j].isOpened() && !m_grid[i][j].isBomb()){
                return false;
            }
        }
    }
    return true;
}

bool Board::isNeighbor(int r1, int c1, int r2, int c2)
{
    return (abs(r1 - r2) <= 1) && (abs(c1 - c2) <= 1);
}

Game::Game()
{
    m_gameOver = false;
    cout << "Choose a level of difficulty" << endl;
    string inputchar;
    do{
        cout << "E, M, or H: ";
        cin >> inputchar;
    }while(inputchar != "E" && inputchar != "M" && inputchar != "H");
    if(inputchar == "E" || inputchar == "e"){
        m_board = new Board(this, 10, 10, 10);
    }
    else if (inputchar == "M" || inputchar == "m"){
        m_board = new Board(this, 18, 18, 40);
    }
    else{
        m_board = new Board(this, 24, 24, 99);
    }
}

void Game::endGame(bool won)
{
    m_gameOver = true;
    if(won){
        cout << "Congrats! You Won!" << endl;
    }
    else{
        cout << "You Lost!" << endl;
    }
}

bool Game::getGameVal()
{
    return m_gameOver;
}

bool Game::checkMove(int row, int col){
    return (row >= 0 && row < m_board->getRows() && col >= 0 && col < m_board->getCols() && m_board->unopened(row, col));
}

void Game::move(int row, int col)
{
    if(checkMove(row, col)){
        m_board->processMove(row, col);
    }
    else{
        cout << "Invalid Move" << endl;
    }
}


void Game::play(){
    m_board->displayBoard();
    cout << "Make your move (row# col#): " ;
    int r, c;
    string a;
    cin >> r >> c;
    m_board->handleFirstClick(r, c);
    m_board->displayBoard();
    while(!m_gameOver){
        cout << "Make your move: row# col#: ";
        cin >> r >> c;
        cout << "What action do you want to do? (O)pen/(F)lag: ";
        cin >> a;
        if(a == "F" || a == "f") m_board->flagCell(r, c);
        else if(a == "O" || a == "o") move(r,c);
        else cout << "Invalid action!" << endl;
        if(m_board->checkGameStatus()) endGame(true);
        m_board->displayBoard();
    }
}