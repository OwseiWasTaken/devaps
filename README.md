# DevAps

## Apps for my everyday coding :)

<!-- TODO: remake todos.go (maybe even in c) so it doesn't use gc.py -->
## todos.go
TODOs finder in go

<!-- it should read directories -->
usage: todos src/*.go


```go
file.go:
// will create a TODO with 3° severity, named "test" with the description "desc"
//TODO(3) test: desc
// will create a TODO with 8° (default) severity, named "test" with the descrition "desc"
//TODO test2: desc
// will create a TODO with 8° (default) severity, named "desc" with no description
//TODO: desc3
```

```shell
$ todos file.go
todos.go:
	3@2: test
	8@4: test2
	8@6: desc3
```

as you can see, todos.go doesn't print descriptions _yet_

## timer.c
timer scheduler in c

each argument is a timer, that **must** define a timer length
and optionally defines the timer label.
the labels can be separated from the timer length with ':', '-', ')' or ' '

usage: ./timer timers[:-) label]<br>
```shell
$ timer "10 hi"
1: hi: 10.000

$ timer 10:hi 5-hello
1: hi: 10.000
2: hello: 5.000

$ timer "10 a b" "5 c" 3
1: a b: 10.000
2: c: 5.000
3: timer: 3.000
```

<!-- TODO: make ctc again -->

## hottie.c
hot reloader in C

usage: hottie \<command\> file[s]

this will execute _command_ everytime any of the files are modified.
if the process created by _command_ doesn't halt before another modification is made, SIGTERM is sent to it
