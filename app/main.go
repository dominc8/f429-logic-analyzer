package main

import (
	"time"
	"github.com/tarm/serial"
	"github.com/gdamore/tcell/v2"
	"github.com/rivo/tview"
    "log"
    "fmt"
)

var (
    baudrate int
    sampling_freq int
)

func cmd_run() {
    log.Printf("[STUB] Send run command")
}

func cmd_baud() {
    log.Printf("[STUB] Send baud command")
}

func draw_text_line(screen tcell.Screen, str string, x int, y *int, width int) {
    tview.Print(screen, str, x, *y, width, tview.AlignCenter, tcell.ColorLime)
    *y++
}

func draw_config(screen tcell.Screen, x int, y int, width int, height int)  (int, int, int, int) {
    y++

    draw_text_line(screen, "Mode: RT", x, &y, width)
    draw_text_line(screen, fmt.Sprintf("Baud: %v", baudrate), x, &y, width)
    draw_text_line(screen, fmt.Sprintf("Sampling frequency: %vHz", sampling_freq), x, &y, width)

    return 0, 0, 0, 0
}

func main() {
    baudrate = 115200
    sampling_freq = 1000
    app := tview.NewApplication()
    list := tview.NewList().
                AddItem("Start sampling", "", 'a', cmd_run).
                AddItem("Set baud rate", "", 'b', cmd_baud).
                AddItem("Quit", "", 'q', func() {
                    app.Stop()
                })
    flex := tview.NewFlex().
                AddItem(list, 0, 1, true).
                AddItem(tview.NewBox().SetBorder(true).SetDrawFunc(draw_config), 0, 4, false)
    if err := app.SetRoot(flex, true).SetFocus(flex).Run(); err != nil {
        panic(err)
    }
    return
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
