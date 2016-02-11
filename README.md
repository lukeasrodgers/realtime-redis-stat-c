This is an experimental, work-in-progress tool/set of tools to massage the output of Redis' `MONITOR`
command into a more digestible format. It is also a "learn how to program in C" project,
so there are many things that are wrong/suboptimal. You probably don't want to run
it anywhere, let alone on production servers.

The idea was inspired by a situation where a redis instance was undergoing steady linear growth in RAM usage,
and it was difficult to discern the source of this growth. (Turned out it was a list that was being repeatedly
`lpush`ed to with ~30-byte elements.)

The basic idea is to provide a couple commandline tools that will:
* read piped output from `redis-cli monitor`, e.g. `redis-cli monitor | this-redis-program-whatever-it's-called`
* periodically (per N command count? per N seconds?) write to STDOUT some information about what's happening
* maybe one tool to just print a running sum of operations for a given class (e.g. all SET operations)
* maybe another tool to track additions/deletions to sets, lists, and provide a running delta
* it would be great if we could periodically query redis and do something like get the size of a growing list,
or the number of elements in a growing set, and provide some estimations about its growth.

## The code

C99

## Compiling

For now, just run `make debug`. Assumes GCC is installed.

## Running

`redis-cli monitor | bin/redis-cmd-aggregate COMMAND_GROUP`

`COMMAND_GROUP` must (currently) be one of:
* key
* set
* list
* string
