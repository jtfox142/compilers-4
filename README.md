storage=global

Link to github: https://github.com/jtfox142/compilers-4

I apologize in advance to anyone who tries to read this code base. It is so messy. Literal spaghetti code. 

Outside of looking like a teenager's room, the code has a few pitfalls I'm aware of. The "..." operator does not work. I have not yet created the supporting code for the "pick" keyword, nor for functions. The <RO> token is processed properly... but it's not pulled into a function like it should be. I was in a rush due to finals season, so I hard coded it into loop1, loop2, and if individually. I also haven't yet gone back through to delete large sections of code I've commented out. I'm very sorry about that, I know you hate that professor Hauschild. 

As far as I have tested it, it creates assembly code that runs pretty well, albeit with far, far too many temporary variables. Also, the scanner crumbles to pieces if there is any extra whitespace.

I fixed a bug with my parser that did not allow assignment without the "set" keyword.