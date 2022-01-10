package main

import (
	"time"
	"github.com/tarm/serial"
	"github.com/rivo/tview"
    "log"
    "strconv"
    "unicode"
)


type device_config struct {
    mode string
    baudrate int
    sampling_freq int
}

var (
    config device_config
    app *tview.Application
    list *tview.List
    form *tview.Form
)

func cmd_run() {
    log.Printf("[STUB] Send run command")
}

func mode_select_callback(option string, optionIndex int) {
    config.mode = option
}

func baud_select_callback(text string) {
    config.baudrate, _ = strconv.Atoi(text)
}

func sampling_freq_select_callback(text string) {
    config.sampling_freq, _ = strconv.Atoi(text)
}

func update_config() {
    log.Printf("[STUB] %v %v %v", config.mode, config.baudrate, config.sampling_freq)
    app.SetFocus(list)
}

func validate_unsigned_int(testToCheck string, lastChar rune) bool {
    return unicode.IsDigit(lastChar)
}


func main() {
    config.mode = "RT"
    config.baudrate = 115200
    config.sampling_freq = 1000
    app = tview.NewApplication()
    list = tview.NewList().
                AddItem("Start sampling", "", 'a', cmd_run).
                AddItem("Configure parameters", "", 'b', func() {
                    app.SetFocus(form)
                }).
                AddItem("Quit", "", 'q', func() {
                    app.Stop()
                })

    form = tview.NewForm().
		AddDropDown("Mode", []string{"RT", "NRT"}, 0, mode_select_callback).
		AddInputField("Baud rate", "115200", 10, validate_unsigned_int, baud_select_callback).
		AddInputField("Sampling frequency", "1000", 10, validate_unsigned_int, sampling_freq_select_callback).
		AddButton("Save", update_config).
        SetCancelFunc(func() {
            app.SetFocus(list)
        })

	form.SetBorder(true).SetTitle("Configuration").SetTitleAlign(tview.AlignCenter)

    flex := tview.NewFlex().
                AddItem(list, 0, 1, true).
                AddItem(form, 0, 4, false)

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
