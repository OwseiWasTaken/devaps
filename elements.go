package main

include "gutil"

type El struct {
	symbol string
	name string
	nome string
	mass float64
}

var (
	elements = []El{
		El{"ERROR", "ERROR", "ERROR", 0},
		El{"H" , "Hydrogen", "Hidrogenio", 1.0079},
		El{"He", "Helium", "Helio", 4.0026},
		El{"Li", "Lithium", "Litio", 6.9410},
		El{"Be", "Beryllium", "Berilio", 9.0122},
		El{"B" , "Boron", "Boro", 10.8110},
		El{"C" , "Carbon", "Carbono", 12.0107},
		El{"N" , "Nitrogen", "Azoto", 14.0067},
		El{"O" , "Oxygen", "Oxigenio", 15.9994},
		El{"F" , "Fluorine", "Fluor", 18.9984},
		El{"Ne", "Neon", "Neon", 20.1797},
		El{"Na", "Sodium", "Sodio", 22.9897},
		El{"Mg", "Magnesium", "Magnesio", 24.3050},
		El{"Al", "Aluminum", "Aluminio", 26.9815},
		El{"Si", "Silicon", "Silicio", 28.0855},
		El{"P" , "Phosphorus", "Fosforo", 30.9738},
		El{"S" , "Sulfur", "Enxofre", 32.0650},
		El{"Cl", "Chlorine", "Cloro", 35.4530},
		El{"K" , "Potassium", "Argon", 39.0983},
		El{"Ar", "Argon", "Potassio", 39.9480},
		El{"Ca", "Calcium", "Calcio", 40.0780},
		El{"Sc", "Scandium", "Escandio", 44.9559},
		El{"Ti", "Titanium", "Titanio", 47.8670},
		El{"V" , "Vanadium", "Vanadio", 50.9415},
		El{"Cr", "Chromium", "Cromo", 51.9961},
		El{"Mn", "Manganese", "Manganes", 54.9380},
		El{"Fe", "Iron", "Ferro", 55.8450},
		El{"Ni", "Nickel", "Cobalto", 58.6934},
		El{"Co", "Cobalt", "Niquel", 58.9332},
		El{"Cu", "Copper", "Cobre", 63.5460},
		El{"Zn", "Zinc", "Zinco", 65.3900},
		El{"Ga", "Gallium", "Galio", 69.7230},
		El{"Ge", "Germanium", "Germanio", 72.6400},
		El{"As", "Arsenic", "Arsenio", 74.9216},
		El{"Se", "Selenium", "Selenio", 78.9600},
		El{"Br", "Bromine", "Bromo", 79.9040},
		El{"Kr", "Krypton", "Cripton", 83.8000},
		El{"Rb", "Rubidium", "Rubidio", 85.4678},
		El{"Sr", "Strontium", "Estroncio", 87.6200},
		El{"Y" , "Yttrium", "Itrio", 88.9059},
		El{"Zr", "Zirconium", "Zirconio", 91.2240},
		El{"Nb", "Niobium", "Niobio", 92.9064},
		El{"Mo", "Molybdenum", "Molibdenio", 95.9400},
		El{"Tc", "Technetium", "Tecnecio", 98.0000},
		El{"Ru", "Ruthenium", "Rutenio", 101.0700},
		El{"Rh", "Rhodium", "Rodio", 102.9055},
		El{"Pd", "Palladium", "Paladio", 106.4200},
		El{"Ag", "Silver", "Prata", 107.8682},
		El{"Cd", "Cadmium", "Cadmio", 112.4110},
		El{"In", "Indium", "Índio", 114.8180},
		El{"Sn", "Tin", "Estanho", 118.7100},
		El{"Sb", "Antimony", "Antimonio", 121.7600},
		El{"Te", "Tellurium", "Telurio", 127.6000},
		El{"I" , "Iodine", "Iodo", 126.9045},
		El{"Xe", "Xenon", "Xenonio", 131.2930},
		El{"Cs", "Cesium", "Cesio", 132.9055},
		El{"Ba", "Barium", "Bario", 137.3270},
		El{"La", "Lanthanum", "Lantanio", 138.9055},
		El{"Ce", "Cerium", "Cerio", 140.1160},
		El{"Pr", "Praseodymium", "Praseodimio", 140.9077},
		El{"Nd", "Neodymium", "Neodimio", 144.2400},
		El{"Pm", "Promethium", "Promecio", 145.0000},
		El{"Sm", "Samarium", "Samario", 150.3600},
		El{"Eu", "Europium", "Europio", 151.9640},
		El{"Gd", "Gadolinium", "Gadolinio", 157.2500},
		El{"Tb", "Terbium", "Terbio", 158.9253},
		El{"Dy", "Dysprosium", "Disprosio", 162.5000},
		El{"Ho", "Holmium", "Holmio", 164.9303},
		El{"Er", "Erbium", "Érbio", 167.2590},
		El{"Tm", "Thulium", "Tulio", 168.9342},
		El{"Yb", "Ytterbium", "Iterbio", 173.0400},
		El{"Lu", "Lutetium", "Lutecio", 174.9670},
		El{"Hf", "Hafnium", "Hafnio", 178.4900},
		El{"Ta", "Tantalum", "Tantalo", 180.9479},
		El{"W" , "Tungsten", "Tungstenio", 183.8400},
		El{"Re", "Rhenium", "Renio", 186.2070},
		El{"Os", "Osmium", "Ósmio", 190.2300},
		El{"Ir", "Iridium", "Iridio", 192.2170},
		El{"Pt", "Platinum", "Platina", 195.0780},
		El{"Au", "Gold", "Ouro", 196.9665},
		El{"Hg", "Mercury", "Mercurio", 200.5900},
		El{"Tl", "Thallium", "Talio", 204.3833},
		El{"Pb", "Lead", "Chumbo", 207.2000},
		El{"Bi", "Bismuth", "Bismuto", 208.9804},
		El{"Po", "Polonium", "Polonio", 209.0000},
		El{"At", "Astatine", "Ástato", 210.0000},
		El{"Rn", "Radon", "Radon", 222.0000},
		El{"Fr", "Francium", "Francio", 223.0000},
		El{"Ra", "Radium", "Radio", 226.0000},
		El{"Ac", "Actinium", "Actinio", 227.0000},
		El{"Th", "Thorium", "Torio", 232.0381},
		El{"Pa", "Protactinium", "Protactinio", 231.0359},
		El{"U" , "Uranium", "Uranio", 238.0289},
		El{"Np", "Neptunium", "Neptunio", 237.0000},
		El{"Pu", "Plutonium", "Plutonio", 244.0000},
		El{"Am", "Americium", "Americio", 243.0000},
		El{"Cm", "Curium", "Curio", 247.0000},
		El{"Bk", "Berkelium", "Berquelio", 247.0000},
		El{"Cf", "Californium", "Californio", 251.0000},
		El{"Es", "Einsteinium", "Einstenio", 252.0000},
		El{"Fm", "Fermium", "Fermio", 257.0000},
		El{"Md", "Mendelevium", "Mendelevio", 258.0000},
		El{"No", "Nobelium", "Nobelio", 259.0000},
		El{"Lr", "Lawrencium", "Laurencio", 262.0000},
	} //1..104
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
		md = 999.0
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
	var a string
	var t int
	var e error

	for i:=0;i<argc;i++{
		a = argv[i]
		if a[len(a)-1] == 'm' {
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
