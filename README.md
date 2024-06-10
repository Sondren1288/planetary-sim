# planetary-sim
Simulation of objects with mass in a 3 dimensional setting.

### Building
Building the project requires ROOT and GNU Make.
There is a testfolder, `rootCompileTest`, that can be compiled first if you wish to check compatability.


### Running
There are som discrepancies that makes it hard to run in different instances.
Generally the project should compile fine, and create a useable `libRootLib.so`, but the `Project.out` file does not always work correctly, depending on which version of ROOT and g++ you are running.
If you compiled the `rootCompileTest` folder with `make`, and try to run `rootCompileTest/Project.out`, but get errors similar to `error: use of undeclared identifier`, either when launching or when pressing buttons, the program has to be launched through root.

The most recent version of ROOT (6.32.00) now expects a function with a name that does not seem to be defined anywhere (in my case, it is `some_project()`. Initial guess is that it is based on the name of the root directory of the repository. It resides in `some_project_test`, so if this is correct it removes the `_test` from the folder name).
Should you be affected by this, you have to launch ROOT first, then load the compiled library with `.L libRootLib.so`, before running the `run()` function. 
This should all be testeable in `rootCompileTest` if you are affected. 
The workaround should also be the same in `src`.
If it still won't work, please raise an issue with the error message you recieve, as well as the steps you went through. 



### Interactive environment
There is a file in the `src` directory called `rootLoader`.
To easily load all classes and functions into ROOT, cat-ing this file, or copying it to clipboard in some other way, and then pasting it into ROOT is the easiest way to get all classes and functions up and running in a easy way.
