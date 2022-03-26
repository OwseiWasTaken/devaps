package main

include "gutil"

type timer struct {
	time float64
	label string
	index int
	done bool
}

var (
	timers = []timer{}
)

func run (t timer, i float64) ( bool ) { // time passed -> running
	if i < t.time {
		printf("%s%s: %.1f seconds left \n", spos(t.index, 0), t.label, t.time-i)
		return true
	} else {
		print(spos(t.index, 0) + "\x1b[K")
		printf("%s%s%s: completed!%s\n", spos(t.index, 0), RGB(0, 255, 0), t.label, RGB(255,255,255))
		t.done = true
		return false
	}
}

func main(){
	InitGu()
	clear()
	test := get("--test").Exists
	if test {
		argv = []string{"4.3:timer", "de", "quatro.3", "segs", "5", ".5:iês"}
		argc = len(argv)
		printf("%stesting %s\n", spos(4, 0), argv)
	}

	var a string // argv[i]
	var ts []string // temporary string
	var err error

	var ( // make timer vars
		time float64
		label string
		// index = i
	)
	if argc == 0 {
		printf("%s[timer error]%s: timer.go needs at least one argument\n", RGB(255, 0, 0), RGB(255, 255, 255))
		exit(1)
	}
	// get args & make timers
	for i:=0;i<argc;i++{
		a = argv[i]

		if (a[0] == 46) {
			a = "0"+a
		}

		if ((47 < a[0] && a[0] < 58)) { // 0..9
			label = spf("timer %d", len(timers)+1) // default label

			// has a label
			if strings.Contains(a, ":") {
				ts = strings.Split(a, ":")
				if len(ts) == 2{
					a = ts[0]
					label = ts[1]
				} else {
					panic(errors.New(spf("$%d has more than one ':'", i+1)))
				}
			}

			// get time
			time, err = strconv.ParseFloat(a, 64)
			panic(err)

			// make timer
			timers = append(timers, timer{time, label, len(timers), false})

		} else if (i != 0) {
			timers[len(timers)-1].label += " " + a
		}
	}

	var t timer// timers[i]
	var r = true

	for j:=0.0;r;j+=0.1{
		r = false
		sleep(0.1)
		for i:=0;i<len(timers);i++{
			t = timers[i]
			r = run(t, j) || r
			if t.done {
				timers = append(timers[:i], timers[i+1:]...)
				//r = true
				i--
			}
		}
	}
	exit(0)
}
