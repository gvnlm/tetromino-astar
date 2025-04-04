# About
<div align="center">
  <img src="docs/demo.gif" width="60%"/>
</div>

# Approach
## Representing states 
## Generating successor states
## Heuristic

# Usage
#### 1. Building the program
```zsh
cd build && cmake .. && cmake --build .
```
#### 2. Running the program
Within `build/`,
```zsh
./tetromino_astar <input_file.txt>
```



## Input file
The input file should be a `.txt` file containing a string representation of the initial state, where:
- `'s'` represents the **start** position
- `'t'` represents the **target** position
- `'o'` represents **obstacle** positions
- `'.'` represents **empty** positions

As an example, here is the input file used for the [demonstration](#about).
```txt
s..o....o.......o..o...o
....o...o..oo...o....o..
.........o....o...o.o.o.
...o......o..o...o.o..o.
..o.o..o.o..o...o.......
.o...o.........o..oooooo
o..o...o..o...o.o.......
..o.o....o.o.o....o.o.o.
.o...oooo...o.o..o..o.o.
...o......o.....o.......
.oo.ooooooo...ooo...ooo.
.o.........o.o..o..oo...
.o.o.oo.o...o...o...o...
.o.o.oo.o...o...o.......
oo......o.o...o.ooooooo.
t...o.o.o.o.o.o.........
```

# Future enhancements
