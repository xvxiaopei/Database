#Tiny Database
We finished the SQL parsing mostly using Flex. The where clauses are parsed by the Shunting-yard algorithm, which is used to parse infix expressions to generate Reverse Polish Notation (RPN). In our implementation, we generated a expression tree rather than the RPF.
Compile
------
Note that the lexical analyzer flex and the corresponding library is required to compile the program. If they have been installed, simply execute 

`$ make`
Usage
------
To use the database with an SQL query txt file

`$ ./tsql < TinySQL_linux.txt`

or

`$ ./tsql TinySQL_linux.txt`

To run an interactive SQL environment

`$ ./tsql`


