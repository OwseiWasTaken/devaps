# DevAps

## Apps for my everyday coding :)

<!-- TODO: remake todos.go (maybe in c) so it doesn't use gc.py -->
## todos.go
TODOs finder in go

<!-- it should read directories -->
usage: todos src/*.go

```go
//TODO(3) test: desc // will create a TODO with 3° severity, named "test" with the description "desc"
//TODO test: desc // will create a TODO with 8° (default) severity, named "test" with the descrition "desc"
//TODO: desc // will create a TODO with 8° (default) severity, named "desc" with no description
```

##### however, todos.go doesn't print descriptions

<!-- TODO: remake timer.go (maybe in c) so it doesn't use gc.py -->
## timer.go
timer scheduler in go

usage: timer timers[:label] [label]<br>
adding a label with ':' or as an argument means the same.<br>
however with ':' the label can be a number.<br>
the labes added without ':' can be concatenated to the last defined timer.<br>
```shell
$ timer 10 hi
timer 1 hi: 10.0 seconds left

$ timer 10:hi
timer 1 hi: 10.0 seconds left

$ timer 10 a b 5 c 3
timer 1 a b c: 10.0 seconds left
timer 2 c: 5.0 seconds left
timer 3: 3.0 seconds left
```

<!-- TODO: make ctc again -->

## hottie.c
hot reloader in C

usage: hottie \<command\> file[s]

this will execute _command_ everytime any of the files are modified.
if the process created by _command_ doesn't halt before another modification is made, SIGTERM is sent to it
