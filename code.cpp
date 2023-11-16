#include <iostream>
#include <fstream>
#include <queue>
#include <vector>

using namespace std;

char ACTIONS[6] = {'E', 'W', 'N', 'S', 'U', 'D'};           // Actions represented by Characters
int total_Nodes = 1;                                        // Start Node already present in the frontier

struct Position {                                           // Structure denoting Co-ordinates of a tile
    int x;
    int y;
    int z;
};

struct Positional_Array {                                   // Array of tile positions when indexed by their values, assists with calculating and computing Sum of the Manhattan Distances easily
    Position Positions[27];
};

struct Tile {                                               // Structure for a Tile with a Position and a Value
    Position P;
    int value;
};

struct Row {                                                // Structure for a Row consisting of 3 Tiles
    Tile T[3];
};

struct Layer {                                              // Structure for a Layer consisting of 3 Rows
    Row R[3];
};

struct PuzzleBoard {                                        // Structure for a Puzzle Board consisting of 3 Layers and some additional parameters
    Layer L[3];
    Positional_Array Array;                                 // Each Puzzle Board is appended with its Positional Array to easily calculate the Positional Arrays of its Children in the Tree
    PuzzleBoard* Parent;                                    // Pointer to the Parent of Puzzle Board in the Tree
    char action;                                            // Action taken to reach to this node from previous Puzzle Board represented by its Character
    int g_value;                                            // g(n) computed as number of steps in the Tree
    int h_value;                                            // h(n) computed as the Sum of Manhattan distances of all the numbered Tiles Basiny Goal
} Start_Board, Goal_Board;                                  // Global variables for the Start Board and the Goal Board are hence initialised;

class Compare {                                             // Function to order the Priority Queue "frontier" based on the f(n) values [ = g(n) + h(n)]
public:
    bool operator()(PuzzleBoard* A, PuzzleBoard* B)
    {
        return A->g_value + A->h_value > B->g_value + B->h_value;
    }
};

priority_queue<PuzzleBoard*, vector<PuzzleBoard*>, Compare> frontier;                   // The Priority Queue frontier is hence created with elements pointing to the Puzzle Boards
vector<PuzzleBoard> reached;                                                            // A Vector "reached" is hence created to store the Puzzle Boards already visited

int computeManhattanDistance (Position current, Position final) {                       // Function which computes the Manhattan Distance Between two given tiles//Positions in the Positional Array// in a Given Puzzle Board
    return abs (current.x - final.x) + abs (current.y - final.y) + abs (current.z - final.z);
}

int computeManhattanSum (Positional_Array Current, Positional_Array Final) {            // Function which computes the Sum of the Manhattan Distances by calling the above function iteratively
    int sum = 0;
    for (int i = 1; i <= 26; i++) {
        sum += computeManhattanDistance(Current.Positions[i], Final.Positions[i]);
    }
    return sum;
}

bool performsAction (PuzzleBoard* New_Board, PuzzleBoard* Current_Board, char action) {  // Function to perform One Single Action for the list of Actions, and assign the proper values to the New Board passed as pointer
    *New_Board = *Current_Board;
    Position blank = Current_Board->Array.Positions[0];                         // Blank Position in the Current Board
    Position swap = blank;                                                      // Position of the tile to be swapped temporarily assigned the position of Blank itself 

    switch (action) {
        case 'E':
            if (blank.x == 2)                                                   // East can only work when x = 1 or x = 0
                return false;
            swap.x = blank.x + 1;                                               // x Co-ordinate in the Swap Position is 1 more than that of the Blank Position, y and z are constant
            break;

        case 'W':
            if (blank.x == 0)                                                   // West can only work when x = 1 or x = 2
                return false;
            swap.x = blank.x - 1;                                               // x Co-ordinate in the Swap Position is 1 less than that of the Blank Position, y and z are constant
            break;

        case 'N':
            if (blank.y == 2)                                                   // North can only work when y = 1 or y = 0
                return false;
            swap.y = blank.y + 1;                                               // y Co-ordinate in the Swap Position is 1 more than that of the Blank Position, x and z are constant
            break;

        case 'S':
            if (blank.y == 0)                                                   // South can only work when y = 1 or y = 2
                return false;
            swap.y = blank.y - 1;                                               // y Co-ordinate in the Swap Position is 1 less than that of the Blank Position, x and z are constant
            break;

        case 'U':
            if (blank.z == 2)                                                   // Up can only work when z = 1 or z = 0
                return false;
            swap.z = blank.z + 1;                                               // z Co-ordinate in the Swap Position is 1 more than that of the Blank Position, x and y are constant
            break;

        case 'D':
            if (blank.z == 0)                                                   // Down can only work when z = 1 or z = 2
                return false;
            swap.z = blank.z - 1;                                               // z Co-ordinate in the Swap Position is 1 less than that of the Blank Position, x and y are constant
            break;
        
        default:
            return false;
    }

    int value = Current_Board->L[swap.z].R[swap.y].T[swap.x].value;             // Storing the Value of the swap Position Tile of the Current Board in a temporary variable
    New_Board->L[swap.z].R[swap.y].T[swap.x].value = 0;                         // Swapping the Values of Blank Position and the Swap Position Tiles in the New Board
    New_Board->L[blank.z].R[blank.y].T[blank.x].value = value;
    New_Board->Array.Positions[value] = blank;                                  // Swapping the Positions of the Blank and Swap Tiles in the Positional Array of the New Board
    New_Board->Array.Positions[0] = swap;
    New_Board->action = action;                                                 // Assigning the Action performed above to the action parameters

    return true;
}

bool hasReached (PuzzleBoard* Current_Board) {                                          // Function to verify the presence of a Given Board in the "reached" vector iteratively
    for (auto i = reached.begin(); i != reached.end(); ++i)
        if (computeManhattanSum(Current_Board->Array, i->Array) == 0)                   // The Sum of Manhattan Distances of All The Numbered Tiles in the Two Boards has to be '0' for Boards to be Same
            return true;
    return false;
}

PuzzleBoard* Astar_Algorithm(PuzzleBoard* Start_Board) {                                // Function which Performs the A_Star (A*) Algorithm on the pointer to a Given Start Board and returns the pointer to a Specific Solutionised Goal Board
    frontier.push(Start_Board);
    reached.push_back(*Start_Board);
    while (!frontier.empty()) {
        PuzzleBoard* Current_Board = frontier.top();
        if (computeManhattanSum(Current_Board->Array, Goal_Board.Array) == 0)           // If the Sum of the Manhattan Distances Between The Two Given Boards is 0, then We are at the Goal Board in Tree
            return Current_Board;
        for (int i = 0; i < 6; i++) {                                                   // Iterating over the 6 Actions available
            PuzzleBoard* New_Board = new PuzzleBoard;
            if(!performsAction(New_Board,  Current_Board, ACTIONS[i]))                  // If an action cannot be performed because of the Position of the Blank Tile on an edge, continue to the next iteration
                continue;
            New_Board->Parent = Current_Board;                                          // If the action was performed successfully
            New_Board->g_value = Current_Board->g_value + 1;                            // The g(n) value is updated, h(n) value is computed separately, and the New Board's Parent is initialised to the Current Board
            New_Board->h_value = computeManhattanSum(New_Board->Array, Goal_Board.Array);
            if (!hasReached(New_Board)) {                                               // If the New Board hence generated is not present in the reached vector, add it to both reached vector and frontierl priority queue
                reached.push_back(*New_Board);
                frontier.push(New_Board);
                total_Nodes += 1;                                                   // total_Nodes is updated only here to keep the track of the number of nodes generated in total in A*
            }
        }
        frontier.pop();
    }
    return NULL;
}

int main () {
    string fileName;                                                                // "fileName" variable consists the name of the input file to be initialised by user
    cin >> fileName;
    ifstream inputFile(fileName);
    ofstream outputFile("output.txt");
    PuzzleBoard* Boards[2] = {&Start_Board, &Goal_Board};
    
    for (int b = 0; b <= 1; b++) {                                                  // Iterating over the Start Board and Goal Board while inputting the respective Tile Values from the given input file and assigning the Positions and other parameters In The Two Puzzle Boards!
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

    PuzzleBoard* Solution_Board = Astar_Algorithm(&Start_Board);                        // Solution Board consists of the pointer to the optimised shallow Goal Node reached through the A* Algorithm

    for (int b = 0; b <= 1; b++) {                                                      // Writing the Tile Values of the Start and Goal Boards into the Output File "output.txt"
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
    
    outputFile << endl << Solution_Board->g_value << endl << total_Nodes << endl;           // Writing the values of depth of the Shallow Goal and the total Number of Nodes to the Output File above
    vector <char> path;                                                                     // Vector consisting of the Path to the Goal Node from the Start Node
    vector <int> F_Values;                                                                  // Vector consisting of the F Values of the Nodes from the Start Node to the Goal Nodes
    PuzzleBoard* Current_Board = Solution_Board; 
    while (Current_Board->Parent != NULL ) {                                                // Iterating until the Parent of the Current Board is not NULL (i.e., until before the Start Board)
        path.push_back(Current_Board->action);
        F_Values.push_back(Current_Board->g_value + Current_Board->h_value);
        Current_Board = Current_Board->Parent;
    }
    
    for (auto ir = path.rbegin(); ir != path.rend(); ++ir)                                  // Writing the path in Characters to the Above Output Files
        outputFile << *ir << " ";  
    outputFile << endl;
    
    for (auto ir = F_Values.rbegin(); ir != F_Values.rend(); ++ir)                          // Writing the F_Values in Numbers to the Above Output Files
        outputFile << *ir << " ";               
    outputFile << Start_Board.h_value;                                                      // The Start Board's F_Value (f(n)) is equal to its H_Value (h(n)) and that is also written to the Above Output Files

    outputFile.close();
         
    return 0;
}
