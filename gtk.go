package main

include "gutil"

func CompareBytes(ba1, ba2 []byte) (bool) {
	for i:=0;i<len(ba1);i++ {
		if ba1[i] != ba2[i] {return false}
	}
	return true
}

func main(){
	InitGu()
	InitGetCh()
	_ = CompareBytes([]byte{0}, []byte{1})
	var BA = make( []byte , 6)
	for {
		printf("%v\n", GetChBA(&BA))
		BA = make( []byte , 6)
	}
	exit(0)
}
