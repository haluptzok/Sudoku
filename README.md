# Sudoku
Sudoku Game Solver - command line - Windows10 VisualStudio17 source code

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

See Sudoku.cpp comment header for more information

Created: 01-Apr-2019 15:48:25
