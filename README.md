# oshell

### Todo

- [ ] Fix all memory leaks.
- [x] shell fails when e.g. file does not exists for "sort" 
really really bad failure? or exit(). - crashes, handle more gracefully like zsh. check before if file exists.
- [ ] When a command fails when in &&, it should stop executing and return back to shell
- [x] shell stops executing && if command is not found
- [ ]  Just dont support \n as chaining, since && works now. Edit report to include && and remove \ as "chaining"
- [ ] add descriptive error messages? (for user input) Check return statements so that process only gets terminated on
- [x] make README nicer

- https://pdos.csail.mit.edu/6.828/2018/labguide.html  
## This explains fd good, maybe can include something
https://barsky.ca/marina/SR/c2018/lectures/7.pipe/06.01.FileDescriptorsPipes.pdf  

----------------

**oshell** is a simple Unix-based shell following a [specification](specification.pdf) by a Professor
from SFU. It supports the following key-features.

- Show a prompt indicating user input '>'
- Read a command entered by the user e.g. 'ping google.com'
- fork() for every user command
- wait() for the command to finish executing
- builtin commands like 'cd, exit, help' without calling external programms
- redirect output with 'xx > output.txt'
- redirecting input with 'xx < input.txt'  
...

This project was not made in any official capacity, so the project report should be treated as such.
The report can be found in the [projectReport](projectReport/oshellReport) folder.


