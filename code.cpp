#include <iostream>
#include <fstream>

using namespace std;

char ACTIONS[6] = {'E', 'W', 'N', 'S', 'U', 'D'};

struct Position {
    int x;
    int y;
    int z;
};

struct Positional_Array {
    Position Positions[27];
} Start_Positions, Goal_Positions;

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
    int g_value;
    int h_value;
} Start_Board, Goal_Board;

int computeManhattanDistance (Position current, Position final) {
    return abs (current.x - final.x) + abs (current.y - final.y) + abs (current.z - final.z);
}

int computeManhattanSum (Positional_Array Current_Positions, Positional_Array Goal_Positions) {
    int sum = 0;
    for (int i = 1; i <= 26; i++) {
        sum += computeManhattanDistance(Current_Positions.Positions[i], Goal_Positions.Positions[i]);
    }

    return sum;
}

int Astar_Algorithm (PuzzleBoard Current_Board) {
    return 0;
}

// void setPositions (PuzzleBoard Board, Positional_Array* current_Positions) {
//     for (int l = 2; l >= 0; l--) {
//         for (int r = 2; r >= 0; r--) {
//             for (int t = 0; t <= 2; t++) {
//                 Tile* tile = &Board.L[l].R[r].T[t];
//                 current_Positions->Positions[tile->value] = tile->P;
//             }
//         }
//     }
// }

int main () {
    ifstream inputFile("input.txt");
    ofstream outputFile("output.txt");
    PuzzleBoard* Boards[2] = {&Start_Board, &Goal_Board};
    Positional_Array* Arrays[2] = {&Start_Positions, &Goal_Positions};

    for (int b = 0; b <= 1; b++) {
        for (int l = 2; l >= 0; l--) {
            for (int r = 2; r >= 0; r--) {
                for (int t = 0; t <= 2; t++) {
                    Tile* tile = &Boards[b]->L[l].R[r].T[t];
                    inputFile >> tile->value;
                    tile->P.x = t;
                    tile->P.y = r;
                    tile->P.z = l;
                    Arrays[b]->Positions[tile->value] = tile->P;
                }
            }
        }
    }
    Start_Board.g_value = 0;
    Start_Board.h_value = computeManhattanSum(Start_Positions, Goal_Positions);

    inputFile.close();
    

    // Astar_Algorithm(Start_Board, Goal_Board);

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
    outputFile.close();
         
    return 0;
}
