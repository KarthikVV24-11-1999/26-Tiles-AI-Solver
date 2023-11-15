#include <iostream>
#include <fstream>
#include <queue>
#include <vector>

using namespace std;

char ACTIONS[6] = {'E', 'W', 'N', 'S', 'U', 'D'};
int total_Nodes = 1;

struct Position {
    int x;
    int y;
    int z;
};

struct Positional_Array {
    Position Positions[27];
};

struct Tile {
    Position P;
    int value;
};

struct Row {
    Tile T[3];
};

struct Layer {
    Row R[3];
};

struct PuzzleBoard {
    Layer L[3];
    Positional_Array Array;
    PuzzleBoard* Parent;
    char action;
    int g_value;
    int h_value;
} Start_Board, Goal_Board;

class Compare {
public:
    bool operator()(PuzzleBoard* A, PuzzleBoard* B)
    {
        return A->g_value + A->h_value > B->g_value + B->h_value;
    }
};

priority_queue<PuzzleBoard*, vector<PuzzleBoard*>, Compare> frontier;
vector<PuzzleBoard> reached;

int computeManhattanDistance (Position current, Position final) {
    return abs (current.x - final.x) + abs (current.y - final.y) + abs (current.z - final.z);
}

int computeManhattanSum (Positional_Array Current_Positions, Positional_Array Final_Positions) {
    int sum = 0;
    for (int i = 1; i <= 26; i++) {
        sum += computeManhattanDistance(Current_Positions.Positions[i], Final_Positions.Positions[i]);
    }
    return sum;
}

bool performsAction (PuzzleBoard* New_Board, PuzzleBoard* Current_Board, char action) { 
    *New_Board = *Current_Board;
    Position blank = Current_Board->Array.Positions[0];
    Position swap = blank;

    switch (action) {
        case 'E':
            if (blank.x == 2)
                return false;
            swap.x = blank.x + 1;
            break;

        case 'W':
            if (blank.x == 0)
                return false;
            swap.x = blank.x - 1;
            break;

        case 'N':
            if (blank.y == 2)
                return false;
            swap.y = blank.y + 1;
            break;

        case 'S':
            if (blank.y == 0)
                return false;
            swap.y = blank.y - 1;
            break;

        case 'U':
            if (blank.z == 2)
                return false;
            swap.z = blank.z + 1;
            break;

        case 'D':
            if (blank.z == 0)
                return false;
            swap.z = blank.z - 1;
            break;
        
        default:
            return false;
    }

    int value = Current_Board->L[swap.z].R[swap.y].T[swap.x].value;
    New_Board->L[swap.z].R[swap.y].T[swap.x].value = 0;
    New_Board->L[blank.z].R[blank.y].T[blank.x].value = value;
    New_Board->Array.Positions[value] = blank;
    New_Board->Array.Positions[0] = swap;
    New_Board->action = action;

    return true;
}

bool hasReached (PuzzleBoard* Current_Board) {
    for (auto i = reached.begin(); i != reached.end(); ++i)
        if (computeManhattanSum(Current_Board->Array, i->Array) == 0)
            return true;
    return false;
}

PuzzleBoard* Astar_Algorithm(PuzzleBoard* Start_Board) {
    frontier.push(Start_Board);
    reached.push_back(*Start_Board);
    while (!frontier.empty()) {
        PuzzleBoard* Current_Board = frontier.top();
        if (computeManhattanSum(Current_Board->Array, Goal_Board.Array) == 0)
            return Current_Board;
        for (int i = 0; i < 6; i++) {
            PuzzleBoard* New_Board = new PuzzleBoard;
            if(!performsAction(New_Board,  Current_Board, ACTIONS[i]))
                continue;
            New_Board->Parent = Current_Board;
            New_Board->g_value = Current_Board->g_value + 1;
            New_Board->h_value = computeManhattanSum(New_Board->Array, Goal_Board.Array);
            if (!hasReached(New_Board)) {
                reached.push_back(*New_Board);
                frontier.push(New_Board);
                total_Nodes += 1;
            }
        }
        frontier.pop();
    }
    return &Goal_Board;
}

int main () {
    string fileName = "input.txt";
    cin >> fileName;
    ifstream inputFile(fileName);
    ofstream outputFile("output.txt");
    PuzzleBoard* Boards[2] = {&Start_Board, &Goal_Board};
    
    for (int b = 0; b <= 1; b++) {
        for (int l = 2; l >= 0; l--) {
            for (int r = 2; r >= 0; r--) {
                for (int t = 0; t <= 2; t++) {
                    Tile* tile = &Boards[b]->L[l].R[r].T[t];
                    inputFile >> tile->value;
                    tile->P.x = t;
                    tile->P.y = r;
                    tile->P.z = l;
                    Boards[b]->Array.Positions[tile->value] = tile->P;
                }
            }
        }
    }

    Start_Board.action = ' ';
    Start_Board.Parent = NULL;
    Start_Board.g_value = 0;
    Start_Board.h_value = computeManhattanSum(Start_Board.Array, Goal_Board.Array);

    inputFile.close();

    PuzzleBoard* Solution_Board = Astar_Algorithm(&Start_Board);

    for (int b = 0; b <= 1; b++) {
        for (int l = 2; l >= 0; l--) {
            for (int r = 2; r >= 0; r--) {
                for (int t = 0; t <= 2; t++) {
                    Tile tile = Boards[b]->L[l].R[r].T[t];
                    outputFile << tile.value << " ";
                }
                outputFile << endl;
            }
            if (l != 0) outputFile << endl;
        }
        if (b == 0) outputFile << endl;
    }
    
    outputFile << endl << Solution_Board->g_value << endl << total_Nodes << endl;
    vector <char> path;
    vector <int> F_Values;
    PuzzleBoard* Current_Board = Solution_Board; 
    while (Current_Board->Parent != NULL ) {
        path.push_back(Current_Board->action);
        F_Values.push_back(Current_Board->g_value + Current_Board->h_value);
        Current_Board = Current_Board->Parent;
    }
    
    for (auto ir = path.rbegin(); ir != path.rend(); ++ir)
        outputFile << *ir << " ";  
    outputFile << endl;
    
    for (auto ir = F_Values.rbegin(); ir != F_Values.rend(); ++ir)
        outputFile << *ir << " ";               
    outputFile << Start_Board.h_value;    

    outputFile.close();
         
    return 0;
}
