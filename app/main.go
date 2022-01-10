package main

import (
	"time"
	"github.com/tarm/serial"
	"github.com/gdamore/tcell/v2"
	"github.com/rivo/tview"
    "log"
    "fmt"
    "strconv"
)

var (
    newbaudrate string
    baudrate int
    sampling_freq int
    app *tview.Application
    list *tview.List
    input_field *tview.InputField
)

func cmd_run() {
    log.Printf("[STUB] Send run command")
}

func cmd_baud() {
    app.SetFocus(input_field)
    //log.Printf("[STUB] Send baud command")
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
    app = tview.NewApplication()
    list = tview.NewList().
                AddItem("Start sampling", "", 'a', cmd_run).
                AddItem("Set baud rate", "", 'b', cmd_baud).
                AddItem("Quit", "", 'q', func() {
                    app.Stop()
                })

    input_field = tview.NewInputField().
		SetLabel("Baud rate: ").
		SetFieldWidth(10).
		SetAcceptanceFunc(tview.InputFieldInteger).
        SetChangedFunc(func(text string) {
            newbaudrate = text
        }).
		SetDoneFunc(func(key tcell.Key) {
            if key == tcell.KeyEnter {
                newbaud, err := strconv.Atoi(newbaudrate)
                if err == nil {
			        baudrate = newbaud
                }
            }
            input_field.SetText("")
            app.SetFocus(list)
		})

    flex := tview.NewFlex().
                AddItem(list, 0, 1, true).
                AddItem(tview.NewBox().SetBorder(true).SetDrawFunc(draw_config), 0, 4, false).
                AddItem(input_field, 0, 1, false)

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
