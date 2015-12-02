#Tiny Database
We finished the SQL parsing mostly using Flex. The where clauses are parsed by the Shunting-yard algorithm, which is used to parse infix expressions to generate Reverse Polish Form (RPF). In our implementation, we generated a expression tree rather than the RPF.
Compile
------
Run `$ make`
Usage
------
To use the database with an SQL query txt file

`$ ./isql < TinySQL_linux.txt`

To run an interactive SQL environment

`$ ./isql`


