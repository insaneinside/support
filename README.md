# Support library: Amalgamation of indispensable idioms.

This is a collection of self-developed utility functions/classes/etc I've developed over the 10+
years I've been writing C and C++.  Many of the included items are nothing new, while I've found
others quite useful (or perhaps too useful) in day-to-day programming.

If you had absolutely nothing better to do, you could probably trace the development of many of
my current programming practices and habits through the code in this repository.  And on that
note, keep in mind that some files contained herein are used more than others.

So, in short, this repository exists primarily for your amusement/to prove how clever the author
is.  Enjoy!

## TimeUtil

`include/support/timeutil.h`, for instance, provides a microsecond-granularity stopwatch utility
with a dead-simple macro-based interface.

	void my_function() {
	  timeutil_init_mark_variables();  /* Declares and initializes the stopwatch data structure. */ 
	
	  /* ... */

	  timeutil_begin("Doing something"); /* Writes "-- Doing something... " to stderr. */
	  do_something();

	  timeutil_end(); /* Writes "done: (N) microseconds elapsed\n" to stderr, with (N) replaced
  					     with... you get it. */

	  /* ... */
	}

All timeutil macros have been carefully written to avoid including the time taken by their own
I/O calls.

There are additional macros that can be used to customize the type and content of messages
printed by timeutil; see the header file for full details.


## Context-based Hierarchical Logging

Inspired by the fine-grained logging control present in [Wine](http://www.winehq.org/), I set
about building a similar stand-alone system for my own projects.  The end result, however,
proved suited to much larger projects than I typically write.  spt::Context (more properly
spt_context, since it's written in C) implements support for hierarchies of contexts into
which messages are logged, and an inheritance-based approach to controlling them.

spt::Context currently has some bugs related to the manipulation of logging hierarchies'
structure post-creation, and dynamic reparenting of contexts shouldn't be used.  Additionally
`mlog`, the underlying function used for logging, really needs some TLC.

See `include/support/spt-context.h` for the details.


## 'k thx bai

There are other interesting things here, in varying states of bit-rot and and amounts of
documentation.  Maybe I'll write about them eventually.  Maybe I won't.  But whatever I do,
don't you hold your breath on account of it.
