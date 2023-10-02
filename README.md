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

## ctc.c
character to code / code to character converter
it will convert codes/chars chars/codes as per the ASCII table.<br>
you can convert a string of chars by writing all of them together
and you can convert a string of codes by separating them with any of the ':', ',', ' ', '\n' delimiters

there's a -1 flag for piping, it transforms the output into easly readable text

```shell
$ctc "hello\n"
h:104
e:101
l:108
l:108
o:111
\n:10

$ctc -1 "hello\n"
104
101
108
108
111
10

$ ctc 104 101 108 108 111 10
104:h
101:e
108:l
108:l
111:o
10:\n


$ ctc -1 104 101 108 108 111 10
hello

```

## hottie.c
hot reloader in C

usage: hottie \<command\> file[s]

this will execute _command_ everytime any of the files are modified.
if the process created by _command_ doesn't halt before another modification is made, SIGTERM is sent to it
