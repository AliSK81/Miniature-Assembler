# Miniature-Assembler
These codes help you translate assembly codes into machine code.  <br><br>
To better understand, the output is decimal. You can add the binary conversion function yourself ;)

## How it works

Do the following steps in order:

1. Clone the repo

    ```
    git clone https://github.com/AliSK81/Miniature-Assembler.git
    ```

2.  Compile the program:
  
    ```
    gcc -o assemble assemble.c
    ```

3. Run the program:

    ```
    assemble test/prog1.as test/prog2.mc
    ```
    **Note:** you may run ``./assemble test/prog1.as test/prog2.mc`` if you are using **linux** :)
