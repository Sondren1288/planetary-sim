# planetary-sim
Simulation of objects with mass in a 3 dimensional setting.

### Building
Building the project requires ROOT and GNU Make.
Simply change directory to `src` and run make; the default should generate an executable `Project.out`.
This file can be run directly.
It also generates a `libRootLib.so` file.
This file can be loaded into ROOT, and then run with `runner()` command, or by creating a variable of the `Main` class, and running `m.main`.

```cpp
Main m = Main();
m.main();
```


### Interactive environment
There is a file in the `src` directory called `rootLoader`.
To easily load all classes and functions into ROOT, cat-ing this file, or copying it to clipboard in some other way, and then pasting it into ROOT is the easiest way to get all classes and functions up and running in a easy way.
