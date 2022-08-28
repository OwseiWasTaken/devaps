package main

include "gutil"

const DEFAULTRANK = 8

var (
	comment = map[string]string {
		"py" :"#",
		"go" :"//",
		"c"  :"//",
		"cpp":"//",
		"ino":"//",
	}
	rankcolor = map[int]string {
		1:RGB(255,0,0),
		2:RGB(250,100,100),
		3:RGB(250,80,10),
		4:RGB(100,200,100),
		5:RGB(0,255,0),
		6:RGB(40,150,180),
		7:RGB(100,100,255),
	}
	nodesc = RGB(140,140,140)+"No Description"+RGB(255,255,255)
)

type Todo struct {
	from string
	at int
	rank int
	name string
	desc string
}

var (
	TABSIZE = 2
	TAB = strings.Repeat(" ", TABSIZE)
	AT = RGB(100,100,255)+"@"+RGB(255,255,255)
	trim = []string{
		" ", "\t",
	}
)

type Pair struct {
	line string
	at int
}

func (T Todo) str () (string) {
	return spf(TAB+"%s%d"+AT+"%d: %s", rankcolor[T.rank], T.rank, T.at, T.name)
}

func dotrim ( line string ) (string) {
	for j:=0;j<len(trim);j++ {
		line = strings.Trim(line, trim[j])
	}
	return line
}

func DoFile ( filename string ) {
	var (
		tmp []string
		file = []Pair{}
		cmnt string
		TODOS = []Todo{}
	)

	if !exists(filename) {
		exit(2)
	}

	tmp = strings.Split(filename,".")
	cmnt = comment[tmp[len(tmp)-1]]
	trim = append(trim, cmnt)
	trim = append(trim, "TODO")


	tmp = strings.Split(ReadFile(filename), "\n")
	for i:=0;i<len(tmp);i++ {
		if strings.Contains(tmp[i], cmnt+"TODO") {
			//trim and append
			file = append(file, Pair{dotrim(tmp[i]), i+1})
		}
	}

	var (
		rank = 0
		name string
		desc string

		stmp string
		itmp int
		line string
		err error
		j int
	)

	for i:=0;i<len(file);i++ {
		//reset vars
		rank = 0
		name = ""
		desc = ""

		line = dotrim(file[i].line)

		if !strings.Contains(line, ":") {
			continue
		}

		if line[0] == '('{
			stmp = line[1:]
			for j=0;j<len(stmp);j++ {
				if stmp[j] == ')'{break}
				itmp, err = strconv.Atoi(string(stmp[j]))
				if err != nil {break}
				rank = (rank*10)+itmp
			}
			//+2 N)
			line = line[j+2:]
		} else {
			rank = DEFAULTRANK
		}

		if line[0] == ':' {
			desc = nodesc
			name = line[1:]
		} else {
			tmp = strings.Split(line, ":")
			name = tmp[0]
			if len(tmp) == 2 {
				desc = tmp[1]
			} else {
				desc = "missing description"
			}
		}
		if len(name) == 0 && len(desc) != 0 {
			name = desc
			desc = nodesc
		}
		if len(desc) == 0{
			desc = nodesc
		}
		name = strings.Trim(name, " ")
		desc = strings.Trim(desc, " ")

		//printf(
		//	"line:\"%s\"\ndesc:%s\nname:%s\nrank:%d\n\n",
		//	line, desc, name, rank,
		//)
		TODOS = append(TODOS, Todo{
			filename, file[i].at, rank, name, desc,
		})
	}

	if len(TODOS) != 0 {
		PS(filename+":")
		for i:=0;i<len(TODOS);i++ {
			PS(TODOS[i].str())
		}
	} else {
		printf("%s: %sno TODOs%s\n", filename, RGB(0,255,0),RGB(255,255,255))
	}
	//TODO(3): name
	//TODO(1) name: desc
	//TODO(3) name

}

func main(){
	if argc == 0 {
		exit(1)
	}
	for i:=0;i<argc;i++{
		DoFile(argv[i])
	}
}
