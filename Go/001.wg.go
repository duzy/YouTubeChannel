// This demonstration shows a misuse of WaitGroup in Golang.
//
// In golang, the WaitGroup is conceptally a counter of tasks to be finished before going forward.
//
// The basic usage is:
//   1. Create a WaitGroup var, e.g. wg in this example.
//   2. Add the counter to indicate how many tasks to wait for.
//   3. Call routines to perform the tasks and tell wg each time when a task is Done.
//   4. The calling rountine simply waits at a point.
//
// In this demonstration, it shows a misuse of WaitGroup that:
//      * confued it with Mutex (possiblly)
//      * forget to protect shared resources (the `a` var in this example)
//      
package main

import (
	"log"
	"sync"
)

// This is the 'task' I mentioned in the above:
func update(m *sync.Mutex, a *int, wg *sync.WaitGroup, i int) {
        m.Lock() // request for the resource `a`
        defer func() {
                m.Unlock() // return the control at the end

                // Calls Done only after the resource is returned (Unlocked)
                wg.Done()
        } ()

	*a += 1 // updates `a` var, which is a 'shared resource'!
        //
        // It's important to keep the concept of `shared resource`, it's critical for creating
        // safe code in Golang when using Goroutines.
}

func main() {
        var m sync.Mutex
	var a int // This is the storage of the shared resource `a`.

        // Our WaitGroup for `a`.
	var wg sync.WaitGroup

        // The major part of the program, initiate 10000 Goroutines to perform parallel updates
        // to `a`, and then wait for the `all done` signal.
        wg.Add(10000)
	for i := 0; i < 10000; i++ {
                // A Goroutine is high performance and lightweight `thread` (conceptually). So
                // shared resources used in a Goroutine must be *protected* using a Mutex.
		go update(&m, &a, &wg, i) // the update task as goroutine
	}

        // wait for `all done`
	wg.Wait()

	log.Println("a=", a) // Let's see the result without protection for `a` now.
}
