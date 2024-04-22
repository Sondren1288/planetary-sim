# planetary-sim
Simulation of objects with mass in a 3 dimensional setting.

### Running
The simplest way is to cat the `src/rootLoader` file to a clipboard, and paste the resulting into ROOT, without sanitizing. 
This makes ROOT load all the files, in the correct order, and you don't have to `.L` each file manually.
Then, still in ROOT, run the `main` function by typing
```
main()
```
.
