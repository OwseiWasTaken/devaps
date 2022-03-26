package main

include "gutil"

type timer struct {
	time float64
	label string
	index int
	ln int
}

var (
	timers = []timer{}
)

func (t timer) run (i float64) ( bool ) { // time passed -> running
	if i < t.time {
		printf("%s%s: %.1f seconds left \n", spos(t.index, 0), t.label, t.time-i)
		return true
	} else {
		print(spos(t.index, 0) + "\x1b[K")
		printf("%s%s%s: completed!%s\n", spos(t.index, 0), RGB(0, 255, 0), t.label, RGB(255,255,255))
		return false
	}
}

func main(){
	InitGu()
	clear()

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
			timers = append(timers, timer{time, label, len(timers),
				len(spf("%s: %.1f seconds left \n", label, time)),
			})
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
			r = t.run(j) || r
		}
	}
}
