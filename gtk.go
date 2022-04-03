package main

include "gutil"

func CompareBytes(ba1, ba2 []byte) (bool) {
	for i:=0;i<len(ba1);i++ {
		if ba1[i] != ba2[i] {return false}
	}
	return true
}

func Compress( x []byte ) ( string ) {
	buff := ""
	for i:=0;i!=6;i++{
		if (x[i] == 0) { break }
		buff+=spf("%.3d,", x[i])
	}
	return buff
}


func main(){
	InitGu()
	InitGetCh()
	clear()
	var limit = -1 // < 0 = infinity
	var err error

	if get("--once").Exists {
		limit = 1
	}
	if get("--limit").Exists {
		if len(get("--limit").List) > 1 {
			PS(get("--limit").List)
			limit, err = strconv.Atoi(get("--limit").List[1])
			panic(err)
		} else {
			fprintf(stderr, "--limit needs a value! ($gtk --limit 32)\n")
		}
	}

	for i:=0;i!=limit;i++{
		x:=GetChByte()
		printf("%v\n", x)
		printf("%v\n", Compress(x))
	}
	exit(0)
}
