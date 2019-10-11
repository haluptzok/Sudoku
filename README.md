# Sudoku
Sudoku Game Solver - command line - Windows10 VisualStudio17 source code

This solves a Sudoku puzzle.

It was built on Windows10 with VisualStudio 17 - but it could easily be 
made to run on Linux.

Enter a Sudoku board on the command line or read it in from a txt file like this:

Sudoku.exe < easy0.txt

Sudoku.exe < extreme7.txt

The input format is 0 means unknown, otherwise 1-9 specifies what is in that
square.

The code isn't very tricky or smart, I was just tired of my wife solving
them faster than me on the airplane seatbacks and wanted to write a program
to do it so I could compete with her.

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
that I could add (like 5.2), and my guessing enumeration should probably start with the most 
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
