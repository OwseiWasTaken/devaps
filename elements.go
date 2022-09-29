package main

include "gutil"

type El struct {
	symbol string
	name string
	nome string
	mass float64
	Family int
}

var (
	elements = []El{
		//0
		El{"ERROR", "ERROR", "ERROR", 0, 0},
		//1
		El{"H" , "Hydrogen", "Hidrogenio", 1.0079, 1},
		El{"He", "Helium", "Helio", 4.0026, 8},
		El{"Li", "Lithium", "Litio", 6.9410, 1},
		El{"Be", "Beryllium", "Berilio", 9.0122, 2},
		El{"B" , "Boron", "Boro", 10.8110, 3},
		El{"C" , "Carbon", "Carbono", 12.0107, 4},
		El{"N" , "Nitrogen", "Nitrogenio", 14.0067, 5},
		El{"O" , "Oxygen", "Oxigenio", 15.9994, 6},
		El{"F" , "Fluorine", "Fluor", 18.9984, 7},
		El{"Ne", "Neon", "Neon", 20.1797, 8},
		El{"Na", "Sodium", "Sodio", 22.9897, 1},
		El{"Mg", "Magnesium", "Magnesio", 24.3050, 2},
		El{"Al", "Aluminum", "Aluminio", 26.9815, 3},
		El{"Si", "Silicon", "Silicio", 28.0855, 4},
		El{"P" , "Phosphorus", "Fosforo", 30.9738, 5},
		El{"S" , "Sulfur", "Enxofre", 32.0650, 6},
		El{"Cl", "Chlorine", "Cloro", 35.4530, 7},
		El{"Ar", "Argon", "Argon", 39.9480, 2},
		El{"K" , "Potassium", "Potassio", 39.0983, 1},
		//20
		El{"Ca", "Calcium", "Calcio", 40.0780, 2},
		El{"Sc", "Scandium", "Escandio", 44.9559, 11},
		El{"Ti", "Titanium", "Titanio", 47.8670, 12},
		El{"V" , "Vanadium", "Vanadio", 50.9415, 13},
		El{"Cr", "Chromium", "Cromo", 51.9961, 14},
		El{"Mn", "Manganese", "Manganes", 54.9380, 15},
		El{"Fe", "Iron", "Ferro", 55.8450, 15},
		El{"Ni", "Nickel", "Cobalto", 58.6934, 16},
		El{"Co", "Cobalt", "Niquel", 58.9332, 16},
		El{"Cu", "Copper", "Cobre", 63.5460, 9},
		El{"Zn", "Zinc", "Zinco", 65.3900, 10},
		El{"Ga", "Gallium", "Galio", 69.7230, 3},
		El{"Ge", "Germanium", "Germanio", 72.6400, 4},
		El{"As", "Arsenic", "Arsenio", 74.9216, 5},
		El{"Se", "Selenium", "Selenio", 78.9600, 6},
		El{"Br", "Bromine", "Bromo", 79.9040, 7},
		El{"Kr", "Krypton", "Cripton", 83.8000, 8},
		El{"Rb", "Rubidium", "Rubidio", 85.4678, 1},
		El{"Sr", "Strontium", "Estroncio", 87.6200, 2},
		El{"Y" , "Yttrium", "Itrio", 88.9059, 11},
		//40
		El{"Zr", "Zirconium", "Zirconio", 91.2240, 12},
		El{"Nb", "Niobium", "Niobio", 92.9064, 13},
		El{"Mo", "Molybdenum", "Molibdenio", 95.9400, 14},
		El{"Tc", "Technetium", "Tecnecio", 98.0000, 15},
		El{"Ru", "Ruthenium", "Rutenio", 101.0700, 15},
		El{"Rh", "Rhodium", "Rodio", 102.9055, 16},
		El{"Pd", "Palladium", "Paladio", 106.4200, 16},
		El{"Ag", "Silver", "Prata", 107.8682, 9},
		El{"Cd", "Cadmium", "Cadmio", 112.4110, 10},
		El{"In", "Indium", "Índio", 114.8180, 3},
		El{"Sn", "Tin", "Estanho", 118.7100, 4},
		El{"Sb", "Antimony", "Antimonio", 121.7600, 5},
		El{"Te", "Tellurium", "Telurio", 127.6000, 6},
		El{"I" , "Iodine", "Iodo", 126.9045, 7},
		El{"Xe", "Xenon", "Xenonio", 131.2930, 8},
		El{"Cs", "Cesium", "Cesio", 132.9055, 1},
		El{"Ba", "Barium", "Bario", 137.3270, 2},
		El{"La", "Lanthanum", "Lantanio", 138.9055, 11},
		El{"Ce", "Cerium", "Cerio", 140.1160, 11},
		El{"Pr", "Praseodymium", "Praseodimio", 140.9077, 11},
		//60
		El{"Nd", "Neodymium", "Neodimio", 144.2400, 11},
		El{"Pm", "Promethium", "Promecio", 145.0000, 11},
		El{"Sm", "Samarium", "Samario", 150.3600, 11},
		El{"Eu", "Europium", "Europio", 151.9640, 11},
		El{"Gd", "Gadolinium", "Gadolinio", 157.2500, 11},
		El{"Tb", "Terbium", "Terbio", 158.9253, 11},
		El{"Dy", "Dysprosium", "Disprosio", 162.5000, 11},
		El{"Ho", "Holmium", "Holmio", 164.9303, 11},
		El{"Er", "Erbium", "Érbio", 167.2590, 11},
		El{"Tm", "Thulium", "Tulio", 168.9342, 11},
		El{"Yb", "Ytterbium", "Iterbio", 173.0400, 11},
		El{"Lu", "Lutetium", "Lutecio", 174.9670, 11},
		El{"Hf", "Hafnium", "Hafnio", 178.4900, 12},
		El{"Ta", "Tantalum", "Tantalo", 180.9479, 13},
		El{"W" , "Tungsten", "Tungstenio", 183.8400, 14},
		El{"Re", "Rhenium", "Renio", 186.2070, 15},
		El{"Os", "Osmium", "Ósmio", 190.2300, 15},
		El{"Ir", "Iridium", "Iridio", 192.2170, 16},
		El{"Pt", "Platinum", "Platina", 195.0780, 16},
		El{"Au", "Gold", "Ouro", 196.9665, 9},
		//80
		El{"Hg", "Mercury", "Mercurio", 200.5900, 10},
		El{"Tl", "Thallium", "Talio", 204.3833, 3},
		El{"Pb", "Lead", "Chumbo", 207.2000, 4},
		El{"Bi", "Bismuth", "Bismuto", 208.9804, 5},
		El{"Po", "Polonium", "Polonio", 209.0000, 6},
		El{"At", "Astatine", "Astato", 210.0000, 7},
		El{"Rn", "Radon", "Radonio", 222.0000, 8},
		El{"Fr", "Francium", "Francio", 223.0000, 1},
		El{"Ra", "Radium", "Radio", 226.0000, 2},
		El{"Ac", "Actinium", "Actinio", 227.0000, 11},
		El{"Th", "Thorium", "Torio", 232.0381, 11},
		El{"Pa", "Protactinium", "Protactinio", 231.0359, 11},
		El{"U" , "Uranium", "Uranio", 238.0289, 11},
		El{"Np", "Neptunium", "Neptunio", 237.0000, 11},
		El{"Pu", "Plutonium", "Plutonio", 244.0000, 11},
		El{"Am", "Americium", "Americio", 243.0000, 11},
		El{"Cm", "Curium", "Curio", 247.0000, 11},
		El{"Bk", "Berkelium", "Berquelio", 247.0000, 11},
		El{"Cf", "Californium", "Californio", 251.0000, 11},
		El{"Es", "Einsteinium", "Einstenio", 252.0000, 11},
		//100
		El{"Fm", "Fermium", "Fermio", 257.0000, 11},
		El{"Md", "Mendelevium", "Mendelevio", 258.0000, 11},
		El{"No", "Nobelium", "Nobelio", 259.0000, 11},
		El{"Lr", "Lawrencium", "Laurencio", 262.0000, 11},
	} //1..103
	NameToE = map[string]El{}
	NomeToE = map[string]El{}
	SymbolToE = map[string]El{}
	mlist = []float64{}
)

func FindClosetsMass (target float64) (El) {
	var (
		//diff
		d = 0.0
		//last diff
		index int
		md = target-mlist[0]
	)

	for i:=0;i<len(mlist);i++ {
		d = target-mlist[i]
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
		if elements[i].Family == in {
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
	lowest := len(target)*missingdiff+wrongdiff
	t := 0
	t2 := 0

	for i:=1;i<len(elements);i++ {
		t = MeasureDiff(elements[i].name, target)
		t2 = MeasureDiff(elements[i].nome, target)
		if t2<t {
			t = t2
		}
		if lowest > t {
			lowest = t
			le = elements[i]
		}
	}

	return le
}

func main(){
	InitGu()
	var E El
	for i:=1;i<len(elements);i++ {
		E = elements[i]
		NameToE[E.name] = E
		NomeToE[E.nome] = E
		SymbolToE[E.symbol] = E
		mlist = append(mlist, E.mass)
	}
	E = elements[0]
	var a string
	var t int
	var e error

	for i:=0;i<argc;i++{
		a = argv[i]
		if (a[0] == 'a' || a[0] == 'A' || a[0] == 'b' || a[0] == 'B') {
			//TODO: define E
			PS(GetFam(a))
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
		PS(E)
	}


	exit(0)
}
