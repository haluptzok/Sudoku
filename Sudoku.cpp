/******************************Module*Header*******************************\
* Module Name: sudoku.cpp

This solves a Sudoku puzzle.

It was built on Windows10 with VisualStudio 17 - but it could easily be
compiled to run on Linux - it's just a command line app.

Enter a Sudoku board on the command line or read it in from a txt file like this:

Sudoku.exe < easy0.txt
Sudoku.exe < extreme7.txt

The input format is 0 means unknown, otherwise 1-9 specifies what is in that
square.

The code isn't very tricky or smart, I was just tired of my wife solving
them faster than me on the airplane seatbacks and wanted to quickly write
a program to do it so I could compete with her.

The program just propagates the simple constraints I
knew about to remove possible numbers from other unconstrained cells
and then if that doesn't solve it, the program just starts guessing by
enumerating the possible values for the under constrained cells
and sees if adding that constraint will solve the puzzle, and then recursing
on guesses if the guess still doesn't yeild a solution.

It works on all the puzzles I have tried. Maybe puzzles can
be made more difficult such that more sopisticated constraints are needed
or this program will just run too long enumerating through possible solutions,
but I haven't found any puzzles this doesn't quickly solve.

http://www.angusj.com/sudoku/hints.php
https://www.inf.tu-dresden.de/content/institutes/ki/cl/study/winter06/fcp/fcp/sudoku.pdf
I see other constraints that I could add in the attached links.
Hidden Pairs, Hidden Triples, Hidden Quads, and Naked Triples and Naked Quads would be good.
And my enumeration of guesses should probably start with the most
constrained cells first (those with smallest number of possibilities left) to
dead end more quickly.

The output below shows extreme8.txt, with the initial board with possible
numbers allowed in each cell, then the numbers possible in each cell after propagating
the constraints and then the final answer, made by guessing recursively till one
works.

C:\arbnet\Sudoku\x64\Release>Sudoku.exe < extreme9.txt
Sudoku.exe
123456789|123456789|      7  |123456789|123456789|123456789|  3      |123456789|123456789|
123456789|   4     |123456789|123456789|123456789|123456789|123456789|123456789|      7  |
  3      |123456789|123456789|123456789|123456789|123456789|123456789|1        |123456789|
     6   |123456789|123456789|123456789|123456789|   4     |123456789|123456789|123456789|
123456789|1        |123456789|123456789|       8 |123456789|123456789|123456789| 2       |
123456789|123456789|    5    |  3      |123456789|123456789|        9|123456789|123456789|
123456789|123456789|        9|     6   |123456789|123456789|    5    |123456789|123456789|
123456789|  3      |123456789|123456789| 2       |123456789|123456789|123456789|       8 |
1        |123456789|123456789|123456789|123456789|      7  |123456789|        9|123456789|

 2  5  89| 2  56 89|      7  |12 45  89|1  456  9|12  56 89|  3      | 2 456 8 |   456  9|
 2  5  89|   4     |1        | 2  5  89|  3 56  9| 23 56 89| 2   6 8 | 2  56 8 |      7  |
  3      | 2  56 89| 2   6 8 | 2 45 789|   4567 9| 2  56 89| 2 4 6 8 |1        |   456  9|
     6   | 2    789| 23    8 |12  5 7 9|1   5 7 9|   4     |1     78 |  3 5 78 |1 3 5    |
   4  7 9|1        |  34     |    5 7 9|       8 |    56  9|   4 67  |  34567  | 2       |
 2 4  78 | 2    78 |    5    |  3      |1    67  |12   6   |        9|   4 678 |1  4 6   |
 2 4  78 | 2    78 |        9|     6   |1 34     |1 3    8 |    5    | 234  7  |1 34     |
   45 7  |  3      |   4 6   |1  45   9| 2       |1   5   9|1  4 67  |   4 67  |       8 |
1        | 2  56 8 | 2 4 6 8 |   45  8 |  345    |      7  | 2 4 6   |        9|  34 6   |

       8 |        9|      7  | 2       |     6   |1        |  3      |    5    |   4     |
 2       |   4     |1        |        9|  3      |    5    |       8 |     6   |      7  |
  3      |    5    |     6   |   4     |      7  |       8 | 2       |1        |        9|
     6   | 2       |       8 |      7  |        9|   4     |1        |  3      |    5    |
        9|1        |  3      |    5    |       8 |     6   |      7  |   4     | 2       |
   4     |      7  |    5    |  3      |1        | 2       |        9|       8 |     6   |
      7  |       8 |        9|     6   |   4     |  3      |    5    | 2       |1        |
    5    |  3      |   4     |1        | 2       |        9|     6   |      7  |       8 |
1        |     6   | 2       |       8 |    5    |      7  |   4     |        9|  3      |

Total Guesses made: 221
Wall clock time passed: 25.23 ms

* Created: 01-Apr-2019 15:48:25
* Author: Patrick Haluptzok
\**************************************************************************/

#include "stdafx.h"

using namespace std;

// Which values are possible in a cell given constraints imposed so far.
#define P1 0x0001
#define P2 0x0002
#define P3 0x0004
#define P4 0x0008
#define P5 0x0010
#define P6 0x0020
#define P7 0x0040
#define P8 0x0080
#define P9 0x0100
#define PALL 0x01FF

std::chrono::high_resolution_clock::time_point t_last; // Used to time execution

__int64 gcGuess;           // How many guesses we made to find the answer

class Board
{
public:
    short Cell[9][9];  // Cell[iRow][iColumn]

    Board()
    {
        ;
    }

    void DisplayBoard(void);
    bool bConstrain(int);
    bool bDone(void);
    bool bSolveIt(int);
};

void Board::DisplayBoard()
{
    // echo current game board to the screen

    for (auto i = 0; i < 9; i++) // every row
    {
        for (auto j = 0; j < 9; j++) // every column
        {
            char input;

            for (int k = 0; k < 9; k++) // every char
            {
                if (Cell[i][j] & (P1 << k))
                {
                    input = '1' + k;
                    cout << input;
                }
                else
                {
                    cout << ' ';
                }
            }

            cout << '|';
        }

        cout << '\n';
    }

    cout << '\n';
}

inline bool SingleBit(unsigned short x)
{
    assert(x); // shouldn't come with 0
    return ((x & (x - 1)) == 0);
}

inline bool DoubleBit(unsigned short x)
{
    assert(x); // shouldn't come with 0
    unsigned short y = (x & (x - 1));
    if (y == 0) return false;
    return ((y & (y - 1)) == 0);
}

inline bool DoubleOrTripleBit(unsigned short x)
{
    assert(x); // shouldn't come with 0 bits
    unsigned short y = (x & (x - 1));
    if (y == 0) return false; // 1 bit is false
    unsigned short z = (y & (y - 1));
    if (z == 0) return true;  // 2 bits OK
    return ((z & (z - 1)) == 0); // 3 bits OK
}

inline bool TripleBit(unsigned short x)
{
    assert(x); // shouldn't come with 0 bits
    unsigned short y = (x & (x - 1));
    if (y == 0) return false; // 1 bit is false
    unsigned short z = (y & (y - 1));
    if (z == 0) return false;  // 2 bits is false
    return ((z & (z - 1)) == 0); // 3 bits OK
}

inline int BitSet(unsigned short x)
{
    assert(x);
    assert(SingleBit(x));
    switch (x)
    {
    case P1:
        return 1;
    case P2:
        return 2;
    case P3:
        return 3;
    case P4:
        return 4;
    case P5:
        return 5;
    case P6:
        return 6;
    case P7:
        return 7;
    case P8:
        return 8;
    case P9:
        return 9;
    }
    assert(0);  // What?
    return 0;
}

// Apply constraints to other positions
// cRecurse is how many times we have called bConstrain from bConstrain
// cRecurse == 0 means means we haven't guessed yet, so no invalid board constraints should happen, like they will when we guess.
bool Board::bConstrain(int cRecurse)
{
    bool bUpdated;

    do
    {
        bUpdated = false;

#if 0  // It actually works fine just propagating singleton constraints only and guessing.  More guesses - but still instantanteously solved.

        for (auto i = 0; i < 9; i++)
        {
            for (auto j = 0; j < 9; j++)
            {
                const int iGridRow = i / 3;
                const int iGridCol = j / 3;

                if (SingleBit(Cell[i][j]))   // Only 1 number is set - remove it from same column & row & grid
                {
                    // remove singletons from each row

                    for (int k = 0; k < 9; k++)
                    {
                        if (k != i)  // Every cell other than the current cell can't have the same number
                        {
                            unsigned short uNew = Cell[k][j] & ~(Cell[i][j]);

                            if (uNew == Cell[k][j])
                            {
                                continue;  // nothing changed
                            }

                            Cell[k][j] = uNew;

                            bUpdated = true;  // something changed - try propagating another time after this one.

                            if (uNew == 0)  // check for over constrained such that no valid solution allowed
                            {
                                // cout << "Constraint at " << i << " " << j << "conflicts with " << k << " " << j << "\n";
                                if (cRecurse == 0) cout << "Shouldn't happen on valid start";
                                return(false);
                            }
                        }
                    }

                    // remove singletons from each column

                    for (int k = 0; k < 9; k++)
                    {
                        if (k != j)  // Every cell other than the current cell can't have the same number
                        {
                            unsigned short uNew = Cell[i][k] & ~(Cell[i][j]);

                            if (uNew == Cell[i][k])
                            {
                                continue;  // nothing changed
                            }

                            Cell[i][k] = uNew;

                            bUpdated = true;  // something changed - try propagating another time after this one.

                            if (uNew == 0)  // check for over constrained such that no valid solution allowed
                            {
                                //cout << "Constraint at " << i << " " << j << "conflicts with " << i << " " << k << "\n";
                                if (cRecurse == 0) cout << "Shouldn't happen on valid start";
                                return(false);
                            }
                        }
                    }

                    // remove singletons from each grid

                    for (int k = 0; k < 9; k++)
                    {
                        int iOffRow = k / 3 + iGridRow * 3;
                        int iOffCol = k % 3 + iGridCol * 3;

                        if ((iOffRow != i) || (iOffCol != j)) // Every cell other than the current cell can't have the same number
                        {
                            unsigned short uNew = Cell[iOffRow][iOffCol] & ~(Cell[i][j]);

                            if (uNew == Cell[iOffRow][iOffCol])
                            {
                                continue;  // nothing changed
                            }

                            Cell[iOffRow][iOffCol] = uNew;

                            bUpdated = true;  // something changed - try propagating another time after this one.

                            if (uNew == 0)  // check for over constrained such that no valid solution allowed
                            {
                                //cout << "Constraint at " << i << " " << j << "conflicts with " << i << " " << k << "\n";
                                if (cRecurse == 0) cout << "Shouldn't happen on valid start";
                                return(false);
                            }
                        }
                    }
                }
            }
        }

        if (bUpdated)
            continue;
#else
        // Look for a number that occurs only 1 time in a row, column, 3x3 grid.
        // If the number has other numbers possible in that cell, make it just that number.
        // 9 (rows/columns/grids) X 10 possible values (well the zero value doesn't really happen).
        // Count how many times each number appears when it isn't already a singleton in a cell.
        // We don't need to fix singletons - so increment more so we don't see the cell as interesting to process later.
        int cRow[9][10];
        int cCol[9][10];
        int cGrd[3][3][10];
        memset(&cRow, 0, sizeof(cRow));
        memset(&cCol, 0, sizeof(cCol));
        memset(&cGrd, 0, sizeof(cGrd));

        for (auto i = 0; i < 9; i++)
        {
            for (auto j = 0; j < 9; j++)
            {
                const int iGridRow = i / 3;
                const int iGridCol = j / 3;

                short sCell = Cell[i][j];

                if (SingleBit(sCell))   // Just 1 number is set, increment by 2 so we don't post-process this cell
                {
                    int iValue = BitSet(sCell);
                    cRow[i][iValue] += (sCell << 1);
                    cCol[j][iValue] += (sCell << 1);
                    cGrd[iGridRow][iGridCol][iValue] += (sCell << 1);
                }
                else
                {
                    // Incrementing at bit offset == 1 for each iNum, we undo that below when checking counts
                    cRow[i][1] += sCell & P1;
                    cRow[i][2] += sCell & P2;
                    cRow[i][3] += sCell & P3;
                    cRow[i][4] += sCell & P4;
                    cRow[i][5] += sCell & P5;
                    cRow[i][6] += sCell & P6;
                    cRow[i][7] += sCell & P7;
                    cRow[i][8] += sCell & P8;
                    cRow[i][9] += sCell & P9;

                    cCol[j][1] += sCell & P1;
                    cCol[j][2] += sCell & P2;
                    cCol[j][3] += sCell & P3;
                    cCol[j][4] += sCell & P4;
                    cCol[j][5] += sCell & P5;
                    cCol[j][6] += sCell & P6;
                    cCol[j][7] += sCell & P7;
                    cCol[j][8] += sCell & P8;
                    cCol[j][9] += sCell & P9;

                    cGrd[iGridRow][iGridCol][1] += sCell & P1;
                    cGrd[iGridRow][iGridCol][2] += sCell & P2;
                    cGrd[iGridRow][iGridCol][3] += sCell & P3;
                    cGrd[iGridRow][iGridCol][4] += sCell & P4;
                    cGrd[iGridRow][iGridCol][5] += sCell & P5;
                    cGrd[iGridRow][iGridCol][6] += sCell & P6;
                    cGrd[iGridRow][iGridCol][7] += sCell & P7;
                    cGrd[iGridRow][iGridCol][8] += sCell & P8;
                    cGrd[iGridRow][iGridCol][9] += sCell & P9;
                }
            }
        }

        // Now check if any numbers in any cells occurred only once (in a row,col,Grd), and also were not singletons already
        for (auto i = 0; i < 9; i++)
        {
            for (auto iNum = 1; iNum <= 9; iNum++)
            {
                int cCount = cRow[i][iNum] >> (iNum - 1);  // We added the bits above - shift over.
                assert(cCount < 11);
                if (cCount <= 0)
                {
                    if (cRecurse == 0)
                        cout << "Every number must appear possible in every row " << cCount << "\n";
                    return(false);
                }
                else if (cCount == 1)
                {
                    // Only cell this number is listed as possible in - so must be this number

                    short sNewValue = 0x0001 << (iNum - 1);

                    for (auto iCol = 0; iCol < 9; iCol++)
                    {
                        if (Cell[i][iCol] & sNewValue)
                        {
                            Cell[i][iCol] = sNewValue;
                            // cout << "Only possible number in row1\n";
                            bUpdated = true;
                            break;  // Only happens once
                        }
                    }
                }

                cCount = cCol[i][iNum] >> (iNum - 1);  // We added the bits above - shift over.
                assert(cCount < 11);
                if (cCount <= 0)
                {
                    if (cRecurse == 0)
                        cout << "Every number must appear possible in every col " << cCount << "\n";
                    return(false);
                }
                else if (cCount == 1)
                {
                    // Only cell this number is listed as possible in - so must be this number

                    short sNewValue = 0x0001 << (iNum - 1);

                    for (auto iRow = 0; iRow < 9; iRow++)
                    {
                        if (Cell[iRow][i] & sNewValue)
                        {
                            Cell[iRow][i] = sNewValue;
                            // cout << "Only possible number in col1\n";
                            bUpdated = true;
                            break;  // Only happens once
                        }
                    }
                }

                cCount = cGrd[i/3][i%3][iNum] >> (iNum - 1);  // We added the bits above - shift over.
                assert(cCount < 11);
                if (cCount <= 0)
                {
                    if (cRecurse == 0)
                        cout << "Every number must appear possible in every grd " << cCount << "\n";
                    return(false);
                }
                else if (cCount == 1)
                {
                    // Only cell this number is listed as possible in - so must be this number

                    short sNewValue = 0x0001 << (iNum - 1);

                    for (auto iGrd = 0; iGrd < 9; iGrd++)
                    {
                        if (Cell[3 * (i/3) + (iGrd/3)][3 * (i%3) + (iGrd%3)] & sNewValue)
                        {
                            Cell[3 * (i/3) + (iGrd/3)][3 * (i%3) + (iGrd%3)] = sNewValue;
                            // cout << "Only possible number in grd\n";
                            bUpdated = true;
                            break;  // Only happens once
                        }
                    }
                }
            }
        }

        // All 3X3 grids have 3 rows through them. Any numbers not possible in the part of a 3x3 grid that isn't intersecting
        // the row, those numbers can't show up in the non-interescting part of the row - and vice versa.  Also
        // same for columns.

        for (auto iGridRow = 0; iGridRow < 3; iGridRow++)
        {
            for (auto iGridCol = 0; iGridCol < 3; iGridCol++)
            {
                // For each row that intersects a 3x3 grid, make sure the non-intersecting part of the
                // row doesn't have numbers that aren't in the non-intersecting part of the 3x3 grid
                // and vice versa.

                for (auto iRow = 0; iRow < 3; iRow++)
                {
                    // OR together all the cells in the Row outside the 3X3 grid box to see what numbers are possible

                    short cellRow = 0;

                    for (auto iCol = 0; iCol < 9; iCol++)
                    {
                        if ((iCol > (3 * iGridCol + 2)) || (iCol < (3 * iGridCol)))
                        {
                            cellRow |= Cell[iRow + iGridRow * 3][iCol];
                        }
                    }

                    // OR together all the cells in the 3X3 grid box outside the row to see what numbers are possible

                    short cellGrid = 0;

                    for (auto iCol = 0; iCol < 9; iCol++)
                    {
                        for (auto iY = 0; iY < 3; iY++)
                        {
                            for (auto iX = 0; iX < 3; iX++)
                            {
                                if (iY != iRow)
                                {
                                    cellGrid |= Cell[iY + iGridRow * 3][iX + iGridCol * 3];
                                }
                            }
                        }
                    }

                    // Now AND out any numbers not possible on the other side of the row outside the 3X3 grid box

                    for (auto iCol = 0; iCol < 9; iCol++)
                    {
                        if ((iCol >(3 * iGridCol + 2)) || (iCol < (3 * iGridCol)))
                        {
                            short cellNew = Cell[iRow + iGridRow * 3][iCol] & cellGrid;

                            if (cellNew != Cell[iRow + iGridRow * 3][iCol])
                            {
                                Cell[iRow + iGridRow * 3][iCol] = cellNew;
                                bUpdated = true;
                            }

                            if (cellNew == 0)
                            {
                                if (cRecurse == 0)
                                    cout << "Shouldn't happen on valid start";
                                return false;
                            }
                        }
                    }

                    // Now AND out any numbers not possible on the other side of the 3X3 grid box outside the row

                    for (auto iCol = 0; iCol < 9; iCol++)
                    {
                        for (auto iY = 0; iY < 3; iY++)
                        {
                            for (auto iX = 0; iX < 3; iX++)
                            {
                                if (iY != iRow)
                                {
                                    short cellNew = Cell[iY + iGridRow * 3][iX + iGridCol * 3] & cellRow;

                                    if (cellNew != Cell[iY + iGridRow * 3][iX + iGridCol * 3])
                                    {
                                        Cell[iY + iGridRow * 3][iX + iGridCol * 3] = cellNew;
                                        bUpdated = true;
                                    }

                                    if (cellNew == 0)
                                    {
                                        if (cRecurse == 0)
                                            cout << "Shouldn't happen on valid start";
                                        return false;
                                    }
                                }
                            }
                        }
                    }
                }

                // Do what we did for rows above for columns now

                for (auto iCol = 0; iCol < 3; iCol++)
                {
                    // OR together all the cells in the colum outside the 3X3 grid box to see what numbers are possible

                    short cellCol = 0;

                    for (auto iRow = 0; iRow < 9; iRow++)
                    {
                        if ((iRow >(3 * iGridRow + 2)) || (iRow < (3 * iGridRow)))
                        {
                            cellCol |= Cell[iRow][iCol + iGridCol * 3];
                        }
                    }

                    // OR together all the cells in the 3X3 grid box outside the column to see what numbers are possible

                    short cellGrid = 0;

                    for (auto iRow = 0; iRow < 9; iRow++)
                    {
                        for (auto iY = 0; iY < 3; iY++)
                        {
                            for (auto iX = 0; iX < 3; iX++)
                            {
                                if (iX != iCol)
                                {
                                    cellGrid |= Cell[iY + iGridRow * 3][iX + iGridCol * 3];
                                }
                            }
                        }
                    }

                    // Now AND out any numbers not possible on the other side of the column outside the 3X3 grid box

                    for (auto iRow = 0; iRow < 9; iRow++)
                    {
                        if ((iRow >(3 * iGridRow + 2)) || (iRow < (3 * iGridRow)))
                        {
                            short cellNew = Cell[iRow][iCol + iGridCol * 3] & cellGrid;

                            if (cellNew != Cell[iRow][iCol + iGridCol * 3])
                            {
                                Cell[iRow][iCol + iGridCol * 3] = cellNew;
                                bUpdated = true;
                            }

                            if (cellNew == 0)
                            {
                                if (cRecurse == 0)
                                    cout << "Shouldn't happen on valid start";
                                return false;
                            }
                        }
                    }

                    // Now AND out any numbers not possible on the other side of the 3X3 grid box outside the column

                    for (auto iRow = 0; iRow < 9; iRow++)
                    {
                        for (auto iY = 0; iY < 3; iY++)
                        {
                            for (auto iX = 0; iX < 3; iX++)
                            {
                                if (iX != iCol)
                                {
                                    short cellNew = Cell[iY + iGridRow * 3][iX + iGridCol * 3] & cellCol;

                                    if (cellNew != Cell[iY + iGridRow * 3][iX + iGridCol * 3])
                                    {
                                        Cell[iY + iGridRow * 3][iX + iGridCol * 3] = cellNew;
                                        bUpdated = true;
                                    }

                                    if (cellNew == 0)
                                    {
                                        if (cRecurse == 0)
                                            cout << "Shouldn't happen on valid start";
                                        return false;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // Search for funky constraints to propagate

        for (auto i = 0; i < 9; i++)
        {
            for (auto j = 0; j < 9; j++)
            {
                const int iGridRow = i / 3;
                const int iGridCol = j / 3;

                if (SingleBit(Cell[i][j]))   // Only 1 number is set - remove it from same column & row & grid
                {
                    // remove singletons from each row

                    for (int k = 0; k < 9; k++)
                    {
                        if (k != i)  // Every cell other than the current cell can't have the same number
                        {
                            unsigned short uNew = Cell[k][j] & ~(Cell[i][j]);

                            if (uNew == Cell[k][j])
                            {
                                continue;  // nothing changed
                            }

                            Cell[k][j] = uNew;

                            bUpdated = true;  // something changed - try propagating another time after this one.

                            if (uNew == 0)  // check for over constrained such that no valid solution allowed
                            {
                                // cout << "Constraint at " << i << " " << j << "conflicts with " << k << " " << j << "\n";
                                if (cRecurse == 0) cout << "Shouldn't happen on valid start";
                                return(false);
                            }
                        }
                    }

                    // remove singletons from each column

                    for (int k = 0; k < 9; k++)
                    {
                        if (k != j)  // Every cell other than the current cell can't have the same number
                        {
                            unsigned short uNew = Cell[i][k] & ~(Cell[i][j]);

                            if (uNew == Cell[i][k])
                            {
                                continue;  // nothing changed
                            }

                            Cell[i][k] = uNew;

                            bUpdated = true;  // something changed - try propagating another time after this one.

                            if (uNew == 0)  // check for over constrained such that no valid solution allowed
                            {
                                //cout << "Constraint at " << i << " " << j << "conflicts with " << i << " " << k << "\n";
                                if (cRecurse == 0) cout << "Shouldn't happen on valid start";
                                return(false);
                            }
                        }
                    }

                    // remove singletons from each grid

                    for (int k = 0; k < 9; k++)
                    {
                        int iOffRow = k / 3 + iGridRow * 3;
                        int iOffCol = k % 3 + iGridCol * 3;

                        if ((iOffRow != i) || (iOffCol != j)) // Every cell other than the current cell can't have the same number
                        {
                            unsigned short uNew = Cell[iOffRow][iOffCol] & ~(Cell[i][j]);

                            if (uNew == Cell[iOffRow][iOffCol])
                            {
                                continue;  // nothing changed
                            }

                            Cell[iOffRow][iOffCol] = uNew;

                            bUpdated = true;  // something changed - try propagating another time after this one.

                            if (uNew == 0)  // check for over constrained such that no valid solution allowed
                            {
                                //cout << "Constraint at " << i << " " << j << "conflicts with " << i << " " << k << "\n";
                                if (cRecurse == 0) cout << "Shouldn't happen on valid start";
                                return(false);
                            }
                        }
                    }
                }
                // Look for 2 cells that match - with only 2 valid numbers.  No other cells can have those numbers
                else if (DoubleBit(Cell[i][j]))
                {
                    // if 2 cells with only 2 valid numbers match - remove from the rest of the row

                    for (int k = 0; k < 9; k++)
                    {
                        if (k != i)  // Every cell other than the current cell
                        {
                            if (Cell[i][j] == Cell[k][j])  // Find a matching cell (with 2 valid numbers left)
                            {
                                for (int m = 0; m < 9; m++)
                                {
                                    if ((m != k) && (m != i))
                                    {
                                        unsigned short uNew = Cell[m][j] & ~(Cell[i][j]);

                                        if (uNew == Cell[m][j])
                                        {
                                            continue;  // nothing changed
                                        }

                                        Cell[m][j] = uNew;

                                        bUpdated = true;  // something changed - try propagating another time after this one.

                                        if (uNew == 0)  // check for over constrained such that no valid solution allowed
                                        {
                                            // cout << "Constraint at " << i << " " << j << "conflicts with " << k << " " << j << "\n";
                                            if (cRecurse == 0) cout << "Shouldn't happen on valid start";
                                            return(false);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // if 2 cells with only 2 valid numbers match - remove from the rest of the column

                    for (int k = 0; k < 9; k++)
                    {
                        if (k != j)  // Every cell other than the current cell
                        {
                            if (Cell[i][j] == Cell[i][k])  // Find a matching cell (with 2 valid numbers left)
                            {
                                for (int m = 0; m < 9; m++)
                                {
                                    if ((m != k) && (m != j))
                                    {
                                        unsigned short uNew = Cell[i][m] & ~(Cell[i][j]);

                                        if (uNew == Cell[i][m])
                                        {
                                            continue;  // nothing changed
                                        }

                                        Cell[i][m] = uNew;

                                        bUpdated = true;  // something changed - try propagating another time after this one.

                                        if (uNew == 0)  // check for over constrained such that no valid solution allowed
                                        {
                                            // cout << "Constraint at " << i << " " << j << "conflicts with " << k << " " << j << "\n";
                                            if (cRecurse == 0) cout << "Shouldn't happen on valid start";
                                            return(false);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // if 2 cells with only 2 valid numbers match - remove from the rest of the 3x3 grid it's in

                    for (int k = 0; k < 9; k++)
                    {
                        const int iOffRow = k / 3 + iGridRow * 3;
                        const int iOffCol = k % 3 + iGridCol * 3;

                        if ((iOffRow != i) || (iOffCol != j)) // Every cell other than the current cell can't have the same number
                        {
                            if (Cell[i][j] == Cell[iOffRow][iOffCol])  // Find a matching cell (with 2 valid numbers left)
                            {
                                for (int m = 0; m < 9; m++)
                                {
                                    const int iOffRowSet = m / 3 + iGridRow * 3;
                                    const int iOffColSet = m % 3 + iGridCol * 3;

                                    if (((iOffRowSet != i) ||
                                         (iOffColSet != j)) &&
                                        ((iOffRowSet != iOffRow) ||
                                         (iOffColSet != iOffCol)))
                                    {
                                        unsigned short uNew = Cell[iOffRowSet][iOffColSet] & ~(Cell[i][j]);

                                        if (uNew == Cell[iOffRowSet][iOffColSet])
                                        {
                                            continue;  // nothing changed
                                        }

                                        Cell[iOffRowSet][iOffColSet] = uNew;

                                        bUpdated = true;  // something changed - try propagating another time after this one.

                                        if (uNew == 0)  // check for over constrained such that no valid solution allowed
                                        {
                                            // cout << "Constraint at " << i << " " << j << "conflicts with " << k << " " << j << "\n";
                                            if (cRecurse == 0) cout << "Shouldn't happen on valid start";
                                            return(false);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (DoubleOrTripleBit(Cell[i][j]))
                {

                    // if 3 cells with only 3 valid numbers match - remove those 3 values from the rest of the column

                    for (int k = 0; k < 9; k++)
                    {
                        if (k != i)  // Every cell other than the current cell
                        {
                            unsigned short uMaskBase = Cell[i][j] | Cell[k][j];

                            if (DoubleOrTripleBit(uMaskBase))  // Find a matching cell (with 2 or 3 valid numbers left)
                            {
                                for (int h = 0; h < 9; h++)
                                {
                                    if ((h != i) && (h != k))
                                    {
                                        unsigned short uMask = uMaskBase | Cell[h][j];

                                        if (DoubleOrTripleBit(uMask))
                                        {
                                            for (int m = 0; m < 9; m++)
                                            {
                                                if ((m != k) && (m != i) && (m != h))
                                                {
                                                    unsigned short uNew = Cell[m][j] & ~uMask;

                                                    if (uNew == Cell[m][j])
                                                    {
                                                        continue;  // nothing changed
                                                    }

                                                    Cell[m][j] = uNew;

                                                    bUpdated = true;  // something changed - try propagating another time after this one.

                                                    if (uNew == 0)  // check for over constrained such that no valid solution allowed
                                                    {
                                                        // cout << "Constraint at " << i << " " << j << "conflicts with " << k << " " << j << "\n";
                                                        if (cRecurse == 0) cout << "Shouldn't happen on valid start";
                                                        return(false);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // if 3 cells with only 3 valid numbers match - remove those 3 values from the rest of the row

                    for (int k = 0; k < 9; k++)
                    {
                        if (k != j)  // Every cell other than the current cell
                        {
                            unsigned short uMaskBase = Cell[i][j] | Cell[i][k];

                            if (DoubleOrTripleBit(uMaskBase))  // Find a matching cell (with 2 or 3 valid numbers left)
                            {
                                for (int h = 0; h < 9; h++)
                                {
                                    if ((h != j) && (h != k))
                                    {
                                        unsigned short uMask = uMaskBase | Cell[i][h];

                                        if (DoubleOrTripleBit(uMask))
                                        {
                                            for (int m = 0; m < 9; m++)
                                            {
                                                if ((m != k) && (m != j) && (m != h))
                                                {
                                                    unsigned short uNew = Cell[i][m] & ~uMask;

                                                    if (uNew == Cell[i][m])
                                                    {
                                                        continue;  // nothing changed
                                                    }

                                                    Cell[i][m] = uNew;

                                                    bUpdated = true;  // something changed - try propagating another time after this one.

                                                    if (uNew == 0)  // check for over constrained such that no valid solution allowed
                                                    {
                                                        // cout << "Constraint at " << i << " " << j << "conflicts with " << k << " " << j << "\n";
                                                        if (cRecurse == 0)
                                                            cout << "Shouldn't happen on valid start";
                                                        return(false);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // if 3 cells with only 3 valid numbers match - remove from the rest of the 3x3 grid it's in

                    for (int k = 0; k < 9; k++)   // Find 1st different cell
                    {
                        const int iOffRow = k / 3 + iGridRow * 3;
                        const int iOffCol = k % 3 + iGridCol * 3;

                        if ((iOffRow != i) || (iOffCol != j)) // Is it Different Cell location
                        {
                            unsigned short uMaskBase = Cell[i][j] | Cell[iOffRow][iOffCol];

                            if (DoubleOrTripleBit(uMaskBase))  // Found a matching cell (with 2 or 3 valid numbers left when combined)
                            {
                                for (int h = 0; h < 9; h++)  // Find 2nd different cell   !!! could start with k + 1 I think
                                {
                                    const int iOffRow2 = h / 3 + iGridRow * 3;
                                    const int iOffCol2 = h % 3 + iGridCol * 3;

                                    if (((iOffRow2 != i) ||
                                         (iOffCol2 != j)) &&
                                        ((iOffRow2 != iOffRow) ||
                                         (iOffCol2 != iOffCol)))
                                    {
                                        unsigned short uMask = uMaskBase | Cell[iOffRow2][iOffCol2];

                                        if (DoubleOrTripleBit(uMask))   // 3rd different cell that only has 3 valid numbers when all combined together.
                                        {
                                            for (int m = 0; m < 9; m++)  // Remove it from all the other cells in that grid
                                            {
                                                const int iOffRowSet = m / 3 + iGridRow * 3;
                                                const int iOffColSet = m % 3 + iGridCol * 3;

                                                if (((iOffRowSet != i) ||
                                                     (iOffColSet != j)) &&
                                                    ((iOffRowSet != iOffRow) ||
                                                     (iOffColSet != iOffCol)) &&
                                                    ((iOffRowSet != iOffRow2) ||
                                                    (iOffColSet != iOffCol2)))
                                                {
                                                    unsigned short uNew = Cell[iOffRowSet][iOffColSet] & ~uMask;

                                                    if (uNew == Cell[iOffRowSet][iOffColSet])
                                                    {
                                                        continue;  // nothing changed
                                                    }

                                                    Cell[iOffRowSet][iOffColSet] = uNew;

                                                    bUpdated = true;  // something changed - try propagating another time after this one.

                                                    if (uNew == 0)  // check for over constrained such that no valid solution allowed
                                                    {
                                                        // cout << "Constraint at " << i << " " << j << "conflicts with " << k << " " << j << "\n";
                                                        if (cRecurse == 0) cout << "Shouldn't happen on valid start";
                                                        return(false);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
#endif
    } while (bUpdated);

    return(true);
}

bool Board::bDone(void)
{
    // Check if just a single value is left in every cell, if yes we have solved the puzzle.

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (!SingleBit(Cell[i][j]))
            {
                return(false);
            }
        }
    }
    return true;
}

// cRecurse tells how many levels deep in recursion we are
bool Board::bSolveIt(int cRecurse)
{
    if (bConstrain(cRecurse))  // Propagate constraints - return false if there are conflicts
    {
        if (bDone())  // Check if that worked - easy Sudoku puzzles usually are solved
        {
            return(true);
        }

        if (cRecurse == 0)  // Show board before we start guessing
            DisplayBoard();

        // Enumerate through guesses and see which ones work out

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (!SingleBit(Cell[i][j]))  // If it's not already a single value
                {
                    unsigned short iCurrent = Cell[i][j];
                    for (int k = 0; k < 9; k++)
                    {
                        unsigned short mask = 0x0001 << k;

                        if (iCurrent & mask)
                        {
                            Board gameNew;  // Make a Copy
                            gameNew = *this;
                            gameNew.Cell[i][j] = mask; // Add Constraint, we guessed it.
                            gcGuess += 1;

                            if (gcGuess % 1000000 == 0)
                            {
                                std::chrono::high_resolution_clock::time_point t_now = std::chrono::high_resolution_clock::now();
                                cout << "Guesses made: " << gcGuess << " time since last " << std::chrono::duration<double, std::milli>(t_now - t_last).count() << " ms\n";
                                t_last = t_now;
                            }

                            // Debugging stuff
                            // cout << "Guess recursion " << cRecurse << " X " << i << " Y " << j << " value " << k + 1 << " Guesses " << gcGuess << "\n";
                            // gameNew.DisplayBoard();

                            if (gameNew.bSolveIt(cRecurse + 1))  // If we succeed - we are done.   Don't return true to enumerate all solutions, pass bSolveIt true to print them all out
                            {
                                *this = gameNew; // return to print out.
                                return(true);
                            }
                        }
                    }

                    return false; // If you can't find a valid value for this spot, something previously guessed is wrong
                }
            }
        }
    }

    return false;
}

int main(int argc, char* argv[])
{
    // echo command line

    for (auto i = 0; i < argc; i++)
    {
        cout << argv[i] << " ";
    }
    cout << "\n";

    // Initialize empty game board

    Board game;
    gcGuess = 0;

    // Read in game board from user

    for (auto i = 0; i < 9; i++)
    {
        for (auto j = 0; j < 9; j++)
        {
            char input;

            cin >> input;

            switch (input)
            {
            case '0':
                game.Cell[i][j] = PALL;
                break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                game.Cell[i][j] = 0x0001 << (input - '1');
                break;

            default:
                cout << "That character is unknown - try again!\n";
                exit(1);
            }
        }
    }

    game.DisplayBoard();

    std::chrono::high_resolution_clock::time_point t_start = t_last = std::chrono::high_resolution_clock::now();
    game.bSolveIt(0);
    std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();

    game.DisplayBoard();
    cout << "Total Guesses made: " << gcGuess << "\n";
    std::cout << std::fixed << std::setprecision(2) << "Wall clock time passed: "
        << std::chrono::duration<double, std::milli>(t_end - t_start).count()
        << " ms\n";
}
