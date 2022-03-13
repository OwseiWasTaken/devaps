package main

include "gutil"

import (
	"sort"
)

func _conv ( x string ) (int, error) {
	a, e := strconv.Atoi(x[:len(x)-1])
	switch (x[len(x)-1]) {
		case 's', 'S'://seconds
			return a, e
		case 'm', 'M'://seconds
			return a*60, e
		case 'h', 'H'://seconds
			return a*60*60, e
	}
	return 0, e
}

func conv (x string) (int) {
	r, e := _conv(x)// result, error
	panic(e)
	return r
}

func tmr ( slp int, index int) () {
	time.Sleep( time.Duration(1000000.0*slp) )
	x:=len(sprintf("%d: %d seconds left\n", index, slp))
	for i:=0;i<slp;i++{
		printf("%s%d: %d seconds left \n", spos(index, 0), index, slp-i)
		sleep(1)
	}
	printf("%s%s \n", spos(index, 0), strings.Repeat(" ", x))
	printf("%s%s%d: completed!%s\n", spos(index, 0), RGB(0, 255, 0), index, RGB(255,255,255))
}

func main(){
	if argc == 0 {
		exit(0)
	}
	InitGu()
	clear()

	HideCursor()
	var os = make([]int, argc)
	for i:=0;i<argc;i++ {
		os[i] = conv(argv[i])
	}
	sort.Ints(os)
	for i:=0;i<argc-1;i++ {
		go tmr(os[i], i)
	}
	tmr(os[argc-1], argc-1)
	ShowCursor()

	exit(0)
}
