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

func GetByCoors(x, y int) (string) {
	for i:=0;i<len(elements);i++ {
		if elements[i].x == x && elements[i].y == y {
			return elements[i].display()
		}
	}
	return RGB(255, 0, 0)+"[no such material]"+RGB(255,255,255)
}

func FindClosetsMass (target float64) (string) {
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
	return elements[index].display()
}

const (
	missingdiff = 1
	wrongdiff = 4
)

func GetFam(in int) ([]string) {
	var ret = []string{}
	for i:=0;i<len(elements);i++ {
		if elements[i].fam == in {
			ret = append(ret, elements[i].display())
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

func FindClosetsName (target string) (string) {
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

	return le.display()
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
		"%s Z=%d M=%f \"%s\" (%d,%d)",
		e.srs[0], e.Z, e.mass, ns[2:], e.x, e.y,
	)
}

func main() () {
	InitGu()
	var fl []string

	if exists("Els.txt") {
		fl=strings.Split(ReadFile("Els.txt"), "\n")
	} else if exists("../Els.txt") {
		fl=strings.Split(ReadFile("../Els.txt"), "\n")
	}
	var tint int
	var index = 0
	var f float64
	var E El
	var terror error
	var line []string

	for i:=0;i<len(fl);i++{
		line = strings.Split(fl[i], ",")
		if len(line) < 5 {continue}
		if fl[i][0] == '#' {continue}

		E = El{index, 0, 0, 0, 0.0, []string{}}

		tint, terror = strconv.Atoi(line[0])
		panic(terror)
		E.fam = tint
		tint, terror = strconv.Atoi(line[1])
		panic(terror)
		E.x = tint
		tint, terror = strconv.Atoi(line[2])
		panic(terror)
		E.y = tint
		f, terror = strconv.ParseFloat(line[3], 32)
		panic(terror)
		E.mass = f

		for i:=4;i<len(line);i++ {
			E.srs = append(E.srs, line[i])
		}

		elements = append(elements, E)
		index++
	}
	intro()
	var s string
	for {
		print(">")
		s=oldinput()
		switch s{
			case "q", "quit","sair","exit","para","parar","sai":
				exit(0)
			case "/?", "help", "ajuda", "man", "manual":
				help()
			case "clear", "limpar", "clean", "cls", "limpa":
				clear()
			case "intro":
				intro()
			case "lista", "list":
				list()
			default:
				run(strings.Split(s, " "))
		}
	}
}

var prp = RGB(255,100,255)+">"+RGB(255,255,255)

func intro() {
	//TODO: make intro
	clear()
	PS("Olá, você está utilizando o Elements")
	PS("escreva \"ajuda\" (e aperte enter), e o manual do Elements vai aparecer")
	PS("escreva \"lista\" (e aperte enter), e uma lista de comandos vai aparecer")
}

func list() {
	clear()
	PS(prp+"limpar limpa a tela")
	PS(prp+"lista lista os comandos")
	PS(prp+"sair (ou \"parar\") para o Elements")
	PS(prp+"intro mostra a introdução do Elements")
	PS(prp+"ajuda (ou \"manual\") mostra o manual")
}

func help() {
	//TODO: make help page
	clear()
	PS("O Elements tenta compreender os seus comandos para reconhecer propriedades de elementos, e responder com os elementos com aquelas propriedades")
	PS("quando um elemento é encontrado, ele é apresentado assim:")
	PS("Símbolo Z=Número Atomico M=Massa \"Outros nomes, ...\" (posição na tabela periódica)")
	PS("\nUm exemplo:")
	PS(">A3 32 15m")
	PS("A3==>")
	PS("B Z=5 M=10.811000 \"Boron, Boro\" (2,13)")
	PS("Al Z=13 M=26.981501 \"Aluminum, Aluminium, Aluminio, Alumínio\" (3,13)")
	PS("Ga Z=31 M=69.723000 \"Gallium, Galio, Gálio\" (4,13)")
	PS("In Z=49 M=114.818001 \"Indium, Indio, Índio\" (5,13)")
	PS("Tl Z=81 M=204.383301 \"Thallium, Talio, Tálio\" (6,13)")
	PS("<==A3")
	PS("Ge Z=32 M=72.639999 \"Germanium, Germanio, Germânio\" (4,14)")
	PS("N Z=7 M=14.006700 \"Nitrogen, Nitrogenio, Nitrogênio\" (2,15)")
}

//TODO: review (and fix) e.fam
var elements = []El{}
func run(argv []string){

	var f float64
	var out string
	var a string
	var ts []string
	var t int
	var t2 int
	var e error
	var scope []string
	var argc = len(argv)

	for i:=0;i<argc;i++{
		a = argv[i]
		if t, e = strconv.Atoi(a[1:]); (a[0] == 'a' || a[0] == 'A' || a[0] == 'b' || a[0] == 'B')&& e == nil {
			if a[0]=='b' || a[0]=='B' {
				t*=-1
			}
			scope = GetFam(t)
			PS(a+RGB(0,255,255)+"==>"+RGB(255,255,255))
			for i:=0;i<len(scope);i++{
				PS(scope[i])
			}
			PS(RGB(0,255,255)+"<=="+RGB(255,255,255)+a)
			continue
		} else if strings.Index(a, ",")!=-1 {
			ts = strings.Split(a, ",")
			t, e = strconv.Atoi(ts[0])
			panic(e)
			t2, e = strconv.Atoi(ts[1])
			panic(e)
			out = GetByCoors(t, t2)
		} else if a[len(a)-1] == 'm' {
			f, e = strconv.ParseFloat(a[:len(a)-1], 32)
			//t, e = strconv.Atoi(a[:len(a)-1])
			panic(e)
			out = FindClosetsMass(f)
			//mass
		} else {
			t, e = strconv.Atoi(a)
			if e == nil {
				out = elements[t].display()
			} else {
				out = FindClosetsName(a)
			}
		}
		PS(out)
	}
}
