package main

include "gutil"

type El struct {
	Z int
	x int
	y int
	fam int
	mass float64
	srs []string
}

func GetByCoors(x, y int) (El) {
	for i:=0;i<len(elements);i++ {
		if elements[i].x == x && elements[i].y == y {
			return elements[i]
		}
	}
	return elements[0]
}

func FindClosetsMass (target float64) (El) {
	var (
		//diff
		d = 0.0
		//last diff
		index int
		md = target-elements[0].mass
	)

	for i:=0;i<len(elements);i++ {
		d = target-elements[i].mass
		if d < 0 { d*=-1 }
		if d < md {
			md = d
			index = i
		}
	}
	return elements[index+1]
}

const (
	missingdiff = 1
	wrongdiff = 4
)

func GetFam(ins string) ([]El) {
	// (Family & 0b1000) >> 3
	// 0b[b]111+1 = [b]8
	// &0b1000 == 0 b = false
	in, err := strconv.Atoi(ins[1:])
	if ins[0] == 'b' || ins[0] == 'B' {
		in+=8
	}
	panic(err)
	var ret = []El{}
	for i:=1;i<len(elements);i++ {
		if elements[i].fam == in {
			ret = append(ret, elements[i])
		}
	}
	return ret
}

func MeasureDiff (a, b string) (int) {
	a = strings.ToLower(a)
	b = strings.ToLower(b)
	ln := len(b)
	dif := len(a) - len(b)
	// lower score = better

	if dif < 0 {
		dif*=-1
		ln = len(a)
	}
	score := dif*missingdiff

	for i:=0;i<ln;i++ {
		if a[i] != b[i] {
			score+=wrongdiff
		}
	}
	return score
}

func FindClosetsName (target string) (El) {
	le := elements[0]
	// started as 'biggest'
	lowest := len(target)*missingdiff+wrongdiff
	t := 999
	t2:= 999

	for i:=1;i<len(elements);i++ {
		for j:=0;j<len(elements[i].srs);j++ {
			t = MeasureDiff(elements[i].srs[j], target)
			if t2<t {
				t = t2
			} else {
				t2 = t
			}
		}
		if lowest > t {
			lowest = t
			le = elements[i]
		}
	}

	return le
}

func (e El) str() (string) {
	// assume srs[0] is the symbol
	return spf("%d:%s", e.Z, e.srs[0])
}

func (e El) display() (string) {
	ns := ""
	for i:=1;i<len(e.srs);i++ {
		ns+=", "+e.srs[i]
	}
	return spf(
		"%s (%d,%d) Z=%d M=%f \"%s\"",
		e.srs[0], e.x, e.y, e.Z, e.mass, ns[2:],
	)
}

//TODO: review (and fix) e.fam
var elements = []El{}
func main(){
	InitGu()

	fl:=strings.Split(ReadFile("Els.txt"), "\n")
	var E El
	var tint int
	var index = 0
	var f float64
	var terror error
	var line []string

	for i:=0;i<len(fl);i++{
		line = strings.Split(fl[i], ",")
		if len(line) < 5 {continue}
		if fl[i][0] == '#' {continue}

		E = El{index, 0, 0, 0, 0.0, []string{}}

		tint, terror = strconv.Atoi(line[0])
		panic(terror)
		E.x = tint
		tint, terror = strconv.Atoi(line[1])
		panic(terror)
		E.y = tint
		tint, terror = strconv.Atoi(line[2])
		panic(terror)
		E.fam = tint
		f, terror = strconv.ParseFloat(line[3], 32)
		panic(terror)
		E.mass = f

		for i:=4;i<len(line);i++ {
			E.srs = append(E.srs, line[i])
		}

		elements = append(elements, E)
		index++
	}

	E = elements[0]
	var a string
	var ts []string
	var t int
	var t2 int
	var e error
	var scope []El

	for i:=0;i<argc;i++{
		a = argv[i]
		if (a[0] == 'a' || a[0] == 'A' || a[0] == 'b' || a[0] == 'B') {
			//TODO: define E
			scope = GetFam(a)
			PS(scope)
		} else if strings.Index(a, ",")!=-1 {
			ts = strings.Split(a, ",")
			t, e = strconv.Atoi(ts[0])
			panic(e)
			t2, e = strconv.Atoi(ts[1])
			panic(e)
			E = GetByCoors(t, t2)
		} else if a[len(a)-1] == 'm' {
			t, e = strconv.Atoi(a[:len(a)-1])
			panic(e)
			E = FindClosetsMass(float64(t))
			//mass
		} else {
			t, e = strconv.Atoi(a)
			if e == nil {
				E = elements[t]
			} else {
				E = FindClosetsName(a)
			}
		}
		PS(E.display())
	}


	exit(0)
}
