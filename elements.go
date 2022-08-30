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
		El{"H", "Hydrogen", "Hidrogénio", 1.0079},
		El{"He", "Helium", "Hélio", 4.0026},
		El{"Li", "Lithium", "Lítio", 6.9410},
		El{"Be", "Beryllium", "Berílio", 9.0122},
		El{"B", "Boron", "Boro", 10.8110},
		El{"C", "Carbon", "Carbono", 12.0107},
		El{"N", "Nitrogen", "Azoto", 14.0067},
		El{"O", "Oxygen", "Oxigénio", 15.9994},
		El{"F", "Fluorine", "Flúor", 18.9984},
		El{"Ne", "Neon", "Néon", 20.1797},
		El{"Na", "Sodium", "Sódio", 22.9897},
		El{"Mg", "Magnesium", "Magnésio", 24.3050},
		El{"Al", "Aluminum", "Alumínio", 26.9815},
		El{"Si", "Silicon", "Silício", 28.0855},
		El{"P", "Phosphorus", "Fósforo", 30.9738},
		El{"S", "Sulfur", "Enxofre", 32.0650},
		El{"Cl", "Chlorine", "Cloro", 35.4530},
		El{"K", "Potassium", "Árgon", 39.0983},
		El{"Ar", "Argon", "Potássio", 39.9480},
		El{"Ca", "Calcium", "Cálcio", 40.0780},
		El{"Sc", "Scandium", "Escândio", 44.9559},
		El{"Ti", "Titanium", "Titânio", 47.8670},
		El{"V", "Vanadium", "Vanádio", 50.9415},
		El{"Cr", "Chromium", "Cromo", 51.9961},
		El{"Mn", "Manganese", "Manganês", 54.9380},
		El{"Fe", "Iron", "Ferro", 55.8450},
		El{"Ni", "Nickel", "Cobalto", 58.6934},
		El{"Co", "Cobalt", "Níquel", 58.9332},
		El{"Cu", "Copper", "Cobre", 63.5460},
		El{"Zn", "Zinc", "Zinco", 65.3900},
		El{"Ga", "Gallium", "Gálio", 69.7230},
		El{"Ge", "Germanium", "Germânio", 72.6400},
		El{"As", "Arsenic", "Arsênio", 74.9216},
		El{"Se", "Selenium", "Selênio", 78.9600},
		El{"Br", "Bromine", "Bromo", 79.9040},
		El{"Kr", "Krypton", "Crípton", 83.8000},
		El{"Rb", "Rubidium", "Rubídio", 85.4678},
		El{"Sr", "Strontium", "Estrôncio", 87.6200},
		El{"Y", "Yttrium", "Ítrio", 88.9059},
		El{"Zr", "Zirconium", "Zircônio", 91.2240},
		El{"Nb", "Niobium", "Nióbio", 92.9064},
		El{"Mo", "Molybdenum", "Molibdênio", 95.9400},
		El{"Tc", "Technetium", "Tecnécio", 98.0000},
		El{"Ru", "Ruthenium", "Rutênio", 101.0700},
		El{"Rh", "Rhodium", "Ródio", 102.9055},
		El{"Pd", "Palladium", "Paládio", 106.4200},
		El{"Ag", "Silver", "Prata", 107.8682},
		El{"Cd", "Cadmium", "Cádmio", 112.4110},
		El{"In", "Indium", "Índio", 114.8180},
		El{"Sn", "Tin", "Estanho", 118.7100},
		El{"Sb", "Antimony", "Antimônio", 121.7600},
		El{"Te", "Tellurium", "Telúrio", 127.6000},
		El{"I", "Iodine", "Iodo", 126.9045},
		El{"Xe", "Xenon", "Xenônio", 131.2930},
		El{"Cs", "Cesium", "Césio", 132.9055},
		El{"Ba", "Barium", "Bário", 137.3270},
		El{"La", "Lanthanum", "Lantânio", 138.9055},
		El{"Ce", "Cerium", "Cério", 140.1160},
		El{"Pr", "Praseodymium", "Praseodímio", 140.9077},
		El{"Nd", "Neodymium", "Neodímio", 144.2400},
		El{"Pm", "Promethium", "Promécio", 145.0000},
		El{"Sm", "Samarium", "Samário", 150.3600},
		El{"Eu", "Europium", "Európio", 151.9640},
		El{"Gd", "Gadolinium", "Gadolínio", 157.2500},
		El{"Tb", "Terbium", "Térbio", 158.9253},
		El{"Dy", "Dysprosium", "Disprósio", 162.5000},
		El{"Ho", "Holmium", "Hólmio", 164.9303},
		El{"Er", "Erbium", "Érbio", 167.2590},
		El{"Tm", "Thulium", "Túlio", 168.9342},
		El{"Yb", "Ytterbium", "Itérbio", 173.0400},
		El{"Lu", "Lutetium", "Lutécio", 174.9670},
		El{"Hf", "Hafnium", "Háfnio", 178.4900},
		El{"Ta", "Tantalum", "Tântalo", 180.9479},
		El{"W", "Tungsten", "Tungstênio", 183.8400},
		El{"Re", "Rhenium", "Rênio", 186.2070},
		El{"Os", "Osmium", "Ósmio", 190.2300},
		El{"Ir", "Iridium", "Irídio", 192.2170},
		El{"Pt", "Platinum", "Platina", 195.0780},
		El{"Au", "Gold", "Ouro", 196.9665},
		El{"Hg", "Mercury", "Mercúrio", 200.5900},
		El{"Tl", "Thallium", "Tálio", 204.3833},
		El{"Pb", "Lead", "Chumbo", 207.2000},
		El{"Bi", "Bismuth", "Bismuto", 208.9804},
		El{"Po", "Polonium", "Polônio", 209.0000},
		El{"At", "Astatine", "Ástato", 210.0000},
		El{"Rn", "Radon", "Rádon", 222.0000},
		El{"Fr", "Francium", "Frâncio", 223.0000},
		El{"Ra", "Radium", "Rádio", 226.0000},
		El{"Ac", "Actinium", "Actínio", 227.0000},
		El{"Th", "Thorium", "Tório", 232.0381},
		El{"Pa", "Protactinium", "Protactínio", 231.0359},
		El{"U", "Uranium", "Urânio", 238.0289},
		El{"Np", "Neptunium", "Neptúnio", 237.0000},
		El{"Pu", "Plutonium", "Plutónio", 244.0000},
		El{"Am", "Americium", "Amerício", 243.0000},
		El{"Cm", "Curium", "Cúrio", 247.0000},
		El{"Bk", "Berkelium", "Berquélio", 247.0000},
		El{"Cf", "Californium", "Califórnio", 251.0000},
		El{"Es", "Einsteinium", "Einstênio", 252.0000},
		El{"Fm", "Fermium", "Férmio", 257.0000},
		El{"Md", "Mendelevium", "Mendelévio", 258.0000},
		El{"No", "Nobelium", "Nobélio", 259.0000},
		El{"Lr", "Lawrencium", "Laurêncio", 262.0000},
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


	exit(0)
}
