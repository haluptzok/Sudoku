/******************************Module*Header*******************************\
* Module Name: sudoku.cpp

This solves a Sudoku puzzle.

It was built on Windows10 with VisualStudio 17 - but it could easily be 
made to run on Linux.

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
on guesses if the guess still doesn't yeild unique solution.

It works on all the puzzles we got on the plane ride, but puzzles can
be made more difficult such that more sopisticated constraints are needed
or this program will just run too long enumerating through possible solutions.

https://www.inf.tu-dresden.de/content/institutes/ki/cl/study/winter06/fcp/fcp/sudoku.pdf
extreme7 and extreme8 are examples from the web that claimed to be the 
world's hardest puzzles, but in this pdf paper I see other constraints
that I could add (like 5.1, 5.2, 5.3), and ensuring if a cell is the only one that
contains a possible number in a row, column or block, then that cell has to be that number.
And my guessing enumeration should probably start with the most 
constrained cells first (those with smallest number of possibilities left) to
dead end more quickly.

Note most puzzles are solved by just propagating constraints to a unique solution, extreme6
is one that actually has to do some guessing to find the answer.

Thoutput below shows the initial board with possible
numbers allowed in each cell, then the numbers possible in each cell after propagating
my constraints and then the final answer, made by guessing recursively till one
works.

C:\Sudoku\Release>Sudoku < extreme6.txt
Sudoku
123456789|123456789|123456789|      7  |   4     |123456789|        9|123456789| 2       |
123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|    5    |
 2       |123456789|123456789|123456789|123456789|  3      |123456789|      7  |123456789|
123456789|   4     |123456789|       8 |123456789|123456789|      7  |123456789|123456789|
    5    |123456789|      7  |123456789|123456789|123456789|  3      |123456789|        9|
123456789|123456789| 2       |123456789|123456789|        9|123456789|       8 |123456789|
123456789|        9|123456789| 2       |123456789|123456789|123456789|123456789|1        |
   4     |123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|
  3      |123456789|    5    |123456789|1        |       8 |123456789|123456789|123456789|

1    6 8 |1 3 56 8 |1 3  6 8 |      7  |   4     |1   56   |        9|1 3  6   | 2       |
1    6789|1 3  678 |1 34 6 89|1    6  9| 2   6 89|12   6   |1  4 6   |1 34 6   |    5    |
 2       |1   56   |1  4 6  9|1   56  9|    56  9|  3      |1  4 6   |      7  |       8 |
1       9|   4     |1 3     9|       8 | 23 5    |12  5    |      7  |12  5    |     6   |
    5    |1    6 8 |      7  |1  4 6   | 2   6   |12 4 6   |  3      |12       |        9|
1    6   |1 3  6   | 2       |1 3 56   |  3 567  |        9|1   5    |       8 |   4     |
     678 |        9|     6 8 | 2       |  3 567  |   4567  |   456 8 |   456   |1        |
   4     |12   678 |1    6 8 |    56  9|    567 9|    567  | 2  56 8 | 2  56  9|  3      |
  3      | 2   6   |    5    |   4 6  9|1        |       8 | 2 4 6   | 2 4 6  9|      7  |

       8 |  3      |1        |      7  |   4     |    5    |        9|     6   | 2       |
     6   |      7  |        9|1        |       8 | 2       |   4     |  3      |    5    |
 2       |    5    |   4     |     6   |        9|  3      |1        |      7  |       8 |
        9|   4     |  3      |       8 |    5    |1        |      7  | 2       |     6   |
    5    |       8 |      7  |   4     | 2       |     6   |  3      |1        |        9|
1        |     6   | 2       |  3      |      7  |        9|    5    |       8 |   4     |
      7  |        9|     6   | 2       |  3      |   4     |       8 |    5    |1        |
   4     |1        |       8 |    5    |     6   |      7  | 2       |        9|  3      |
  3      | 2       |    5    |        9|1        |       8 |     6   |   4     |      7  |


Total time taken: 47
Guesss made 5

* Created: 01-Apr-2019 15:48:25
* Author: Patrick Haluptzok
\**************************************************************************/
// Sudoku.cpp : Defines the entry point for the console application.
//

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

#define TICK_DIFF(start,end) ((DWORD) ((end >= start) ? (end - start) : (0xffffffff - start + end)))

int gcGuess;		   // How many guesses we made to find the answer

class Board
{
public:
	short Cell[9][9];  // Cell[iRow][iColumn]

	Board() 
	{
		;
	}

	void DisplayBoard(void);
	bool bConstrain(void);
	bool bDone(void);
	bool bSolveIt(bool);
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

// Apply constraints to other positions

bool Board::bConstrain(void)
{
	bool bUpdated;

	do
	{
		bUpdated = false;

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

#if 0  // These 2 constraints are special cases of above rules

				// If there are only 3 valid numbers in a grid row - then none of the other rows can have those 3

				int uTriple = 0;

				for (int k = 0; k < 3; k++)
				{
					int iOffRow = i;
					int iOffCol = k + iGridCol * 3;

					uTriple |= Cell[iOffRow][iOffCol];
				}

				if (TripleBit(uTriple))
				{
					for (int k = 0; k < 9; k++)
					{
						if (k / 3 != iGridCol)
						{
							unsigned short uNew = Cell[i][k] & ~uTriple;

							if (uNew == Cell[i][k])
								continue;

							if (uNew == 0)  // check for over constrained such that no valid solution allowed 
							{
								//cout << "Constraint at " << i << " " << j << "conflicts with " << i << " " << k << "\n";
								return(false);
							}

							bUpdated = true;
							Cell[i][k] = uNew;
						}
					}
				}

				// If there are only 3 valid numbers in a grid column - then none of the other grid columns can have those 3

				uTriple = 0;

				for (int k = 0; k < 3; k++)
				{
					int iOffRow = k + iGridRow * 3;
					int iOffCol = j;

					uTriple |= Cell[iOffRow][iOffCol];
				}

				if (TripleBit(uTriple))
				{
					for (int k = 0; k < 9; k++)
					{
						if (k / 3 != iGridRow)
						{
							unsigned short uNew = Cell[k][j] & ~uTriple;

							if (uNew == Cell[k][j])
								continue;

							if (uNew == 0)  // check for over constrained such that no valid solution allowed 
							{
								//cout << "Constraint at " << i << " " << j << "conflicts with " << i << " " << k << "\n";
								return(false);
							}

							bUpdated = true;
							Cell[k][j] = uNew;
						}
					}
				}

#endif

			}
		}
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

bool Board::bSolveIt(bool bDisplay)
{	
	if (bConstrain())  // Propagate constraints - return false if there are conflicts
	{
		if (bDone())  // Check if that worked - easy Sudoku puzzles usually are solved 
		{
			return(true);
		}

		if (bDisplay)
			DisplayBoard();

		// Enumerate through possible constraints and see which ones work out

		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				if (!SingleBit(Cell[i][j]))
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
								cout << "Guess made " << gcGuess << "\n";

							if (gameNew.bSolveIt(false))  // If we succeed - we are done.   Don't return true to enumerate all solutions, pass bSolveIt true to print them all out
							{
								*this = gameNew; // return to print out.
								return(true);
							}
						}
					}
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

	DWORD dwStart = GetTickCount();
	game.bSolveIt(true);
	DWORD dwEnd = GetTickCount();    
	DWORD cTicks = TICK_DIFF(dwStart, dwEnd);

	game.DisplayBoard();
	cout << "\nTotal time taken: " << cTicks << "\n";
	cout << "Total Guesses made: " << gcGuess << "\n";
}