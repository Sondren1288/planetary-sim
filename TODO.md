Send message with head of title

Add more planets in the solar system
Realtime simulation as well; updating ALL graphs within the loop at the same time before the next iteration.
Fix axis with 1:1:1 ratio, at least in the spacial domain (Not necessarily the time domain).


Maybe draw objects with the given radius of the object.


Instead of euler, use verlet.
Should it prove necessary, then a eulerstep could be considered.
Create a timestep decrement when bodies get closer to each other, or add some form of eulerstep to ensure that planets do not launch at the speed of light when approaching too close. Or improve physics model, I guess



### Notes to self
Slows down significantly when it reaches 2000+ iterations. 1000 is still manageable, but drawing 2k+ seems to be unnessesary
