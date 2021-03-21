# das-shiny profiler #

## About ##

DAS-Shiny is the library to support profiling in C or C++ projects
with manual instrumentation originally developed by Aidin Abedi
(see also "Relation to the original project" below).

Below is the list of traits of this library, which makes it different
from similar projects:
 * Very low footprint
 * Implemented in C. So not only can it be easily integrated into
   pure C projects, but it can also be used on systems, where 
   C++ is not supported at all.
 * Intended to be usable with more or less any modern and not so modern
   compilers on more or less any system/OS (If you discover any problems,
   please, report them).
 * Really easy to start with. (TODO: Write instructions for this)

This project is probably a good call for you, if
 * You find yourself manually writing "printf-s" trying to find out, how
   much time execution of this or another region on your code requires
   (especially, if you notice, that pure "printf-s" are no more sufficient).
 * You need to find a bottleneck in the existing code (especially if the
   sourcebase is big and complicated, so that use of automated things like
   valgrind is not really feasible).
 * You are on the system, where other tools do not work.
 * You prefer precise control and flexibility of manual instrumentation
   over sampling or automated instrumentation.
 * You search for a solution to integrate into your project to monitor
   it's performance - either manually or using automated tests, -
   but still have an option to completely compile it out with single
   compiler switch.

This project does NOT provide:
 * Graphical front end (Ogre-based front end from the original project
   was discontinued in this fork).
 * Automated instrumentation or sampling.


## Multithreading ?? ##

TODO

My first impression, that it will not work, but it needs to be prooved.


## Relation to the original project ##

This project is a fork of the
[original Shiny profiler](https://code.google.com/archive/p/shinyprofiler/)
developed by Aidin Abedi (see [README.original](README.original.md) for details)


### Changes ###

Major changes so far:
* The directory structure is changed. 
* Support of Lua and Ogre is dropped.
* Redundant files are removed.
* The source is modified to better fit my taste 
  (I am not claiming that original version was not good
  I just like it more this way).
  
TODO: Provide some hints on useage.



## Acknowledgements ##

1. Even though some changes were done to the build system, organisation of files,
   formatting, naming etc., the actual design of the library and implementation
   of all the algorithms stays virtually unchanged, as it was originally
   implemented by [Aidin Abedi](https://www.linkedin.com/in/aidinabedi/)
   in the original project published on google code: 
   [https://code.google.com/archive/p/shinyprofiler/](https://code.google.com/archive/p/shinyprofiler/)
   
2. For development of this project [CLion](https://www.jetbrains.com/clion/)
   IDE was used. License for it was generously provided by
   [JetBrains](https://www.jetbrains.com/) free of charge as part of their 
   [program to support open source projects](https://www.jetbrains.com/community/opensource/#support).