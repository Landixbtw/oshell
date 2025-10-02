# oshell

### Todo
- [ ] make README nicer
- [ ] add descriptive error messages? Check return statements so that process only gets terminated on 
- [ ] Fix all memory leaks.
- [ ] Properly interpret newline char \n so that command chaining can still work without && (also important for project report
5.1 Getting the input)
- [ ] shell fails when e.g. file does not exists for "sort" 
really really bad failure? or exit(). - crashes, handle more gracefully like zsh. check before if file exists.

# https://pdos.csail.mit.edu/6.828/2018/labguide.html  
## This explains fd good, maybe can include something
https://barsky.ca/marina/SR/c2018/lectures/7.pipe/06.01.FileDescriptorsPipes.pdf  

----------------

**oshell** is a simple Unix-based shell following a specification [1](specification.pdf) by a Professor
from SFU. It supports the following key-features.

- Show a prompt indicating user input '>'
- Read a command entered by the user e.g. 'ping google.com'
- fork() for every user command
- wait() for the command to finish executing
- builtin commands like 'cd, exit, help' without calling external programms
- redirect output with 'xx > output.txt'
- redirecting input with 'xx < input.txt'
...

This project was not made in any official capacity, so the project report should be viewed like that.
The pdf and LaTeX code can be found in the [projectReport](projectReport/oshellReport) folder.


