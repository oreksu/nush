# CS3650 HW05: Shell Commands Tokens

In this assignment, you will write a Unix shell like "bash" called "nush".

A shell operates as follows:

-   Accept an optional script file as the first command line argument.
-   If there's no script, show a prompt. Your prompt should be "nush$ ". Command input is accepted on the same line after the prompt.
-   Read one line of commands from either stdin or the script.
-   Execute that line of commands.
-   Repeat until EOF or an "exit" command.

Command lines are made up of:

-   Programs to execute.
-   Built-in commands.
-   Command line arguments to programs / builtins.
-   Operators.

Programs to Execute

    nat@masaru:hw05$ ./nush 
    nush$ head -n 2 /usr/share/dict/words
    A
    A's
    nush$ 

Given a program to execute, like "head" above, your shell will do three things:

-   fork(), to create a new process.
-   in the child process, exec(), to execute the requested program, passing through the three command line arguments
-   in the parent process, wait() or waitpid(), to allow the program to finish before executing another command

Built-in Commands

    nush$ pwd
    /home/nat/Teaching/3650/hw05
    nush$ cd tests
    nush$ pwd
    /home/nat/Teaching/3650/hw05/tests
    nush$ exit
    nat@masaru:hw05$

Some commands aren't programs to execute, they're things that the shell should handle itself. You should implement two of these:

-   The "cd" command changes the current directory. This means calling chdir() to change the state of your shell process.
-   The "exit" command exits the shell.

In bash, lots of commands that are available as programs are actually built-in for efficiency, like pwd, echo, etc. Your shell should just run these as programs.

Operators

    nush$ sort tests/sample.txt | tail -n 3
    remorselessly
    revolutionized
    underplays
    nush$ 

Operators either modify the behavior of one command or chain together multiple commands into a single command line.

Your shell should support seven operators:

-   Redirect input: sort < foo.txt
-   Redirect output: sort foo.txt > output.txt
-   Pipe: sort foo.txt | uniq
-   Background: sleep 10 &
-   And: true && echo one
-   Or: true || echo one
-   Semicolon: echo one; echo two

To submit:

-   C Source code.
-   A Makefile that builds your program.
-   In a gzipped tarball, in a "challenge01" directory.

Don't submit:

-   Your compiled binary, object files, or other build artifacts.
-   The "tmp" directory created by the tests.
-   Stray hidden files.

Your source code should pass clang-check with no warnings.

Check your program with valgrind to make sure you have no memory allocation issues.

This is the first challenge assignment. It's not easy. Get a couple of tests to pass, and submit to make sure they pass the autograder. Then get a couple more tests to pass, etc.

The last couple tests are pretty hard. I expect less than half of the class to pass all the tests and get a 100%.

Expected time to complete this assignment with most test passing is 14 hours.

## To Submit

-   A tar.gz archive containing the code for your solution.
-   No stray files (.o files, binaries, hidden files).
-   If you include the tests, don't change them.


Shell commands come in several forms:

Commands without arguments:

      $ pwd

Commands with arguments:

      $ sort tests/sample.txt

Operators with single commands:

      $ sort < foo.txt

Operators joining multiple commands

      $ grep derp foo.txt | wc -l

Seven operators:

-   Redirect input: sort < foo.txt
-   Redirect output: sort foo.txt > output.txt
-   Pipe: sort foo.txt | uniq
-   Background: sleep 10 &
-   And: true && echo one
-   Or: true || echo one
-   Semicolon: echo one; echo two

In this assignment, you will write a program that reads single line shell commands, splits them into a sequence of tokens, reverses the sequence, and prints out one token per line.

Note: You must store the sequence of of tokens in a data structure - either a Variable Length Array or a Linked List - in memory before printing it.

There are three kinds of tokens:

-   Command names.
-   Command arguments.
-   Shell operators.

Spaces that appear in a command are not tokens, although they may separate tokens.

Example interaction:

    bash$ ./tokens
    tokens$ echo one; echo two
    two
    echo
    ;
    one
    echo
    tokens$ sort foo.txt | uniq
    uniq
    |
    foo.txt
    sort
    tokens$ ^D
    bash$

(Note: In the above interaction “^D” means the user has pressed Ctrl+D to send the End-of-File event to the terminal.)

## To Submit

-   A tar.gz archive containing the code for your solution.
-   No stray files (.o files, binaries, hidden files).
-   If you include the tests, don’t change them.
