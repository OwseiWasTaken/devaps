package main

import (
	"fmt"
)

type Iterator[Out any] interface {
	Collect() ([]Out)
	String() string
}

type Iter[T any] Iterator[T]
func Map[In, Out any](I Iter[In], fn func(V In, index int)Out) Iter[Out] {
	return Iter[Out](
		IterMap[In, Out]{ fn, I },
	)
}
func Filter[T any](I Iter[T], fn func(T, int)bool) Iter[T] {
	return Iter[T](IterFilter[T]{ fn, I })
}

type IterArray[Out any] []Out
func (IA IterArray[Out]) Collect() ([]Out) {
	return IA
}
func (IA IterArray[Out]) String() string {
	return fmt.Sprintf("%v", []Out(IA))
}

type IterMap[In any, Out any] struct {
	fn func(In, int) Out
	prevIter Iterator[In]
}
func (IM IterMap[In, Out]) Collect() (cout []Out) {
	cin := IM.prevIter.Collect()
	cout = make([]Out, len(cin))
	for index, item := range cin {
		cout[index] = IM.fn(item, index)
	}
	return
}
func (IM IterMap[In, Out]) String() string {
	var i In
	var b Out
	return fmt.Sprintf("Map[%T->%T]{ %s }", i, b, IM.prevIter)
}

type IterFilter[T any] struct {
	fn func(T, int) bool
	prevIter Iterator[T]
}
func (IF IterFilter[T]) Collect() (cout []T) {
	cin := IF.prevIter.Collect()
	cout = []T{}
	for index, item := range cin {
		if (IF.fn(item, index)) {
			cout = append(cout, item)
		}
	}
	return
}
func (IF IterFilter[T]) String() string {
	return fmt.Sprintf("Filter{ %s }", IF.prevIter)
}

func main() {
	var arr Iter[int] = IterArray[int]([]int{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10})
	arr = Map[int, int](arr, func(v, _ int) int {return v*3})
	arr = Filter[int](arr, func(v, _ int) bool {return v%2==0})
	arr_2 := Map[int, string](arr, func(v, _ int) string {return fmt.Sprintf("%d", v)})

	// if swaped (I, fn) for func inputs
	//arr_3 := Map[int, string](
	//	func(v, _ int) string {return fmt.Sprintf("%d", v)},
	//	Filter[int](
	//		func(v, _ int) bool {return v%2==0},
	//		Map[int, int](
	//			func(v, _ int) int {return v*3},
	//			IterArray[int]([]int{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}),
	//		),
	//	),
	//)

	fmt.Printf("%T:\n", arr_2)
	fmt.Printf("  %s\n", arr_2)
	fmt.Printf("  %s\n", arr_2.Collect())
	fmt.Printf("%T:\n", arr_3)
	fmt.Printf("  %s\n", arr_3)
	fmt.Printf("  %s\n", arr_3.Collect())
}

