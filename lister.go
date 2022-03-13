include "gutil"

func ListDir ( dirname string ) () {
	s:=ls(dirname)
	i:=0
	for ;i+1<len(s);i++ {
		printf("%s ", s[i])
	}
	printf("%s\n", s[i])
}

