```mermaid
flowchart TD
    ol["oshell_loop()"] --> orl["oshell_read_line()"]
    orl --> p["parse()"]
    p --> eq["execute_command()"]
    eq --> ol

```
