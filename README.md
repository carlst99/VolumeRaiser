# VolumeRaiser
Raises the relative volume of all audio sessions to 100%. In other words, sets the volume of all applications to be the same as the volume of your computer.

**Foreword**

My computer has an extremely annoying habit of randomly dropping the volume of inactive audio sessions to one. Yes, 1%. I still haven't found the cause except for that it's correlated with audio ducking, so VolumeRaiser is my slap-on solution. It works a treat if I run it once at startup and every time I leave an audio call.

**Why two projects?**

I'd tried for a bit to get this going in `c#` to no avail, as I didn't understand the APIs. So I bit the bullet and dived in to the Microsoft Core Audio API docs and wrote VolumeRaiser in `c++`. From there I ported it back into `c#`, making use of [CSCore](http://filoe.github.io/cscore/)'s port of the API, to provide a reference to others out there who may be facing the same problem.

### Usage
Simply run the executable `vrp.exe` - see releases. It'll print out some info, then reset the relative volume of all audio sessions to 1.0f, or 100%. This is then multiplied by the master volume scalar (your computer's volume).

If for some reason `vrp.exe` doesn't work for you (I wouldn't be too surprised, I've really still got no idea how `c++` projects compile) please build the project yourself, or better yet create a pull request with a fix :smile:
