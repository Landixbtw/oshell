# oshell
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

The codebase looks like a warzone, but the code works. Maybe ill clean up in some time.

--- 
## Test output
```
running 10 tests
test test_env_var ... ok
test test_cd_home ... ok
test test_echo_hello ... ok
test test_cd_documents ... ok
test test_append ... ok
test test_command_chaining ... ok
test test_single_pipe ... ok
test test_multi_pipe ... ok
test test_kill_by_name ... ok
test test_kill_by_pid ... ok

test result: ok. 10 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 6.76s
```

This explain file descriptors and all really good 
https://barsky.ca/marina/SR/c2018/lectures/7.pipe/06.01.FileDescriptorsPipes.pdf  

- [ ] fix memory leaks
