# DevAps

## Apps for my everyday coding :)

## todos.go
TODOs finder in go

todos.go depends on $HOME/.config/todos

schema for the todo finder is
[*]string[*] (tab) (regex)
the prefix and suffix asteriscs say if the pattern
can have any string before of after the specified string

the pattern is then mathed on the file's name
**all** patterns that match will have the regex expression mathed on them

```
*.go		//\s*TODO.*
*.c			//\s*TODO.*
*.h			//\s*TODO.*
*.hpp		//\s*TODO.*
*.cpp		//\s*TODO.*
*.py		#\s*TODO.*
*.html	<!--.*-->
README.md	<!--.*-->
*.gohtml*	<!--.*-->
```

todos.go _will_ have an update where regex can be multi-line

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

## ctc.c
character to code / code to character converter
it will convert codes/chars chars/codes as per your string->byte table, generally ASCII.
you can convert a string of chars by writing all of them together
and you can convert a string of codes by separating them with any of the ':', ',', ' ', '\n' delimiters

there's a -1 flag for piping, it transforms the output into easly parsable text

```shell
$ctc "hello"
h:104
e:101
l:108
l:108
o:111

$ctc -1 "hello"
104
101
108
108
111

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

if no arguments (except flags) are provided to ctc, it will read from stdinj
```shell
$ echo "hello" | ctc
h:104
e:101
l:108
l:108
o:111
```

you can even pipe the output from ctc to ctc (using the pipe flag), to revet the changes
```shell
$ echo "hello" | ctc -1 | ctc -1
hello
```

## fpwd.go

fpwd.go is the new version of spwd
and spwd was the configurable version of pwd

fpwd uses, but doesn't depend on, ~/.config/fpwd

fpwd's schema for config file is:
(old pwd) (new pwd) [prefix]

fpwd replaces any "\e" in prefix to \x1b, that is normally used for escape sequences.
