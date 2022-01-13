package main

import (
    "github.com/tarm/serial"
    "github.com/rivo/tview"
    "log"
    "strconv"
    "unicode"
    "fmt"
    "os"
    "encoding/binary"
)


type device_config struct {
    mode int
    baudrate int
    sampling_freq int
    sampling_sources int
}

func (cfg device_config) init() {
    cfg.mode = 0
    cfg.baudrate = 115200
    cfg.sampling_freq = 1000
    cfg.sampling_sources = 8
}

func (cfg device_config) update() {
    send_command(fmt.Sprintf("mode %v", cfg.mode))
    send_command(fmt.Sprintf("baud %v", cfg.baudrate))
    send_command(fmt.Sprintf("freq %v", cfg.sampling_freq))
    send_command(fmt.Sprintf("src %v", cfg.sampling_sources))
}

var (
    serial_port *serial.Port
    app *tview.Application
    list *tview.List
    form *tview.Form
)

func validate_unsigned_int(testToCheck string, lastChar rune) bool {
    return unicode.IsDigit(lastChar)
}

func send_command(text string) {
    cmd := []byte(fmt.Sprintf("%-20s", text))
    n, err := serial_port.Write(cmd)
    if err != nil {
        log.Printf("cmd '%s' [%v] failed: %v", text, cmd, err)
    }
    log.Printf("Sent cmd '%v' [%v]", string(cmd), n)
}

func cmd_run() {
    serial_port.Flush()
    send_command("run")
    go read_data() // TODO: this is probably neverending routine, so what happens if someone starts sampling, stops and then starts again?
}

func connect_to_device() {
    serial_config := &serial.Config{Name: "/dev/ttyACM0", Baud: 115200}
    s, err := serial.OpenPort(serial_config)
    if err != nil {
        log.Fatal(err)
    }
    log.Printf("Serial port opened %v", s)
    serial_port = s
}

func read_data() {
    outf, _ := os.OpenFile("tmp_data.bin", os.O_APPEND|os.O_RDWR|os.O_CREATE, 0644)
    defer outf.Close()
    buf := make([]byte, 256)

    for {
        n, err := serial_port.Read(buf)
        if err == nil && n > 0 {
            binary.Write(outf, binary.LittleEndian, buf[:n])
        } else {
            break
        }
    }
}

func main() {
    var config device_config
    config.init()

    log_file, _ := os.OpenFile("log.txt", os.O_APPEND|os.O_RDWR|os.O_CREATE, 0644)
    defer log_file.Close()
    log.SetOutput(log_file)
    log.SetFlags(log.Lshortfile|log.LstdFlags)

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
                AddDropDown("Mode", []string{"RT", "NRT"}, 0, func(_ string, index int) {
                    config.mode = index
                }).
                AddInputField("Baud rate", "115200", 10, validate_unsigned_int, func(s string) {
                    config.baudrate, _ = strconv.Atoi(s)
                }).
                AddInputField("Sampling frequency", "1000", 10, validate_unsigned_int, func(s string){
                    config.sampling_freq, _ = strconv.Atoi(s)
                }).
                AddDropDown("Sampling sources", []string{"1", "2", "4", "8"}, 3, func(s string, _ int) {
                    config.sampling_sources, _ = strconv.Atoi(s)
                }).
                AddButton("Save", func() {
                    config.update()
                    app.SetFocus(list)
                }).
                SetCancelFunc(func() {
                    app.SetFocus(list)
                })

    form.SetBorder(true).SetTitle("Configuration").SetTitleAlign(tview.AlignCenter)

    flex := tview.NewFlex().
                AddItem(list, 0, 1, true).
                AddItem(form, 0, 4, false)

    connect_to_device()

    if err := app.SetRoot(flex, true).SetFocus(flex).Run(); err != nil {
        panic(err)
    }

}
