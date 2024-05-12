storage=global

Link to github: https://github.com/jtfox142/compilers-4

I apologize in advance to anyone who tries to read this code base. It is so messy. Literal spaghetti code. 

Bugs I am aware of:
* The "..." operator does not work
* The "pick" keyword does not work
* Loop2 seems to work as it should, but anything nested within loop1 that also creates a jump statement may loop forever.

I fixed a bug with my parser that did not allow assignment without the "set" keyword. I fixed a bug with my scanner that didn't allow it to properly interpret extra whitespace or comments.