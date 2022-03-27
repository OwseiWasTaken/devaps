package main

include "gutil"

const (
	// togo && from
	C = iota
	F
	K
)

type temp struct {
	value float64
	togo int
	from int
	FromV float64
}

func KtoC (i float64) (float64) {
	return i+273.15
}

func CtoC (i float64) (float64) {
	return i
}

func FtoC (i float64) (float64) {
	return ( i - 32) / 1.8
}

func CtoF (i float64) (float64) {
	return i * 1.8 + 32
}

func CtoK (i float64) (float64) {
	return i - 273.15
}

func dpt (i temp) () { // debug print temp
	printf("value: %f\n", i.value)
	printf("needs to be: %s\n", NtN(i.togo))
	printf("came from: %s\n", NtN(i.from))
	printf("came from V: %d\n\n", i.FromV)
}

func WriteTemp ( i temp ) (string) {
	return spf("%f%s:%f%s", i.FromV, sNtN(i.from), i.value, sNtN(i.togo))
}

func PrintTemp	( i temp ) () {
	printf("  %.3f%s:%.3f%s\n", i.FromV, NtN(i.from), i.value, NtN(i.togo))
}

func NtN (i int) (string) { // num to name
	return []string{"°C", "°F", "K"}[i]
}

func sNtN (i int) (string) { // simple num to name
	return []string{"C", "K", "F"}[i]
}

func main(){
	InitGu()

	var stuff = [3][]temp{
		[]temp{}, // C
		[]temp{}, // F
		[]temp{}, // K
	}

	var w = RGB(255,255,255)
	var err error

	var IgnoreNext bool // ignore next argument (-o ...)
	var current int // current togo temp
	var ov float64 // temp.FromV
	var v float64 // temp.value
	var a string // current arg
	var now int // current temp type (C, K, F)
	var t temp // temp

	for i := 0 ; i < argc ; i++ {
		if IgnoreNext {
			IgnoreNext = false
			continue
		}
		a = argv[i]
		if len(a) < 2 {break}
		switch (argv[i]) {
		case "-f", "-f°", "-°f":
			current = F
		case "-k":
			current = K
		case "-c", "-c°", "-°c":
			current = C
		case "-o":
			IgnoreNext = true
		default:
			v, err = strconv.ParseFloat(argv[i][:len(argv[i])-1], 64)
			ov = v
			panic(err)
			switch (argv[i][len(argv[i])-1]) {
			case 'c', 'C':
				v = CtoC(v)
				now = C
			case 'k', 'K':
				v = KtoC(v)
				now = K
			case 'f', 'F':
				v = FtoC(v)
				now = F
			}
			stuff[current] = append(stuff[current], temp{v, current, now, ov})
		}
	}
	var buff = ""
	if get("-o").Exists { // write output
		for n:=0;n<len(stuff);n++ {
		for i:=0;i<len(stuff[n]);i++ {
			buff += WriteTemp(stuff[n][i])+"\n"
		}
		}

		if len(get("-o").List)-1 != 0 {
			err = os.WriteFile(get("-o").List[1], []byte(buff), 0644) // 1X 2W 4R
			panic(err)
		} else {
			err = os.WriteFile("out.txt", []byte(buff), 0644) // 1X 2W 4R
			panic(err)
		}
	} else { // normal print
		for n:=0;n<len(stuff);n++ {
			if len(stuff[n]) != 0 {
				if (n) == C {
					printf("%sCelsius%s:\n", RGB(255, 255, 0), w)
				} else if (n) == K {
					printf("\n%sKelvin%s:\n", RGB(0, 255, 255), w)
				} else if (n) == F {
					printf("\n%sFahrenheit%s:\n", RGB(255, 0, 255), w)
				}
				for i:=0;i<len(stuff[n]);i++ {
					t = stuff[n][i]
					switch (t.togo) {
					case C:
						t.value = CtoC(t.value)
					case K:
						t.value = CtoK(t.value)
					case F:
						t.value = CtoF(t.value)
					}
					PrintTemp(t)
				}
			}
		}
	}

	exit(0)
}
