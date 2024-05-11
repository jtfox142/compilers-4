storage=global

Link to github: https://github.com/jtfox142/compilers-4

I apologize in advance to anyone who tries to read this code base. It is so messy. Literal spaghetti code. 

Outside of looking like a teenager's room, the code has a few pitfalls I'm aware of. The "..." operator does not work. The <RO>
token is processed... but it's not pulled into a function like it should be. I was in a rush due to finals season, so I hard coded it into loop1, loop2, and if individually. 

As far as I have tested it, it creates assembly code that runs pretty well, albeit with far, far too many temporary variables.

I fixed a bug with my parser that did not allow assignment without the "set" keyword.