package main

import (
	"time"
	"github.com/tarm/serial"
    "log"
)


func main() {
    c := &serial.Config{Name: "/dev/ttyACM0", Baud: 115200, ReadTimeout: time.Millisecond * 5000}
    s, err := serial.OpenPort(c)
    if err != nil {
        log.Fatal(err)
        return
    }
    log.Printf("Serial port opened %v", s)
    n, err := s.Write([]byte("run                 "))
    if err != nil {
        log.Fatal(err)
        return
    }
    log.Printf("Sent 'run' command, n=%v", n)

    buf := make([]byte, 256)

    for {
        n, err = s.Read(buf)
        if err != nil {
            log.Printf("err != nil from s.Read: %v", err)
            break
        }
        log.Printf("read got n=%v", n)
        nsmall := 10
        if n < 10 {
            nsmall = n
        }
        log.Print(buf[0:nsmall])
    }
    log.Printf("Exiting program")
}
