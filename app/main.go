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

var (
    log_file *os.File
    serial_port *serial.Port
    config device_config
    app *tview.Application
    list *tview.List
    form *tview.Form
)

func mode_select_callback(option string, optionIndex int) {
    config.mode = optionIndex
}

func baud_select_callback(text string) {
    config.baudrate, _ = strconv.Atoi(text)
}

func sampling_freq_select_callback(text string) {
    config.sampling_freq, _ = strconv.Atoi(text)
}

func sampling_sources_select_callback(option string, optionIndex int) {
    config.sampling_sources, _ = strconv.Atoi(option)
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
    go read_data()
}

func update_config() {
    send_command(fmt.Sprintf("mode %v", config.mode))
    send_command(fmt.Sprintf("baud %v", config.baudrate))
    send_command(fmt.Sprintf("freq %v", config.sampling_freq))
    send_command(fmt.Sprintf("src %v", config.sampling_sources))
    app.SetFocus(list)
}

func validate_unsigned_int(testToCheck string, lastChar rune) bool {
    return unicode.IsDigit(lastChar)
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
    config.mode = 0
    config.baudrate = 115200
    config.sampling_freq = 1000
    config.sampling_sources = 8

    log_file, _ = os.OpenFile("log.txt", os.O_APPEND|os.O_RDWR|os.O_CREATE, 0644)
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
                AddDropDown("Mode", []string{"RT", "NRT"}, 0, mode_select_callback).
                AddInputField("Baud rate", "115200", 10, validate_unsigned_int, baud_select_callback).
                AddInputField("Sampling frequency", "1000", 10, validate_unsigned_int, sampling_freq_select_callback).
                AddDropDown("Sampling sources", []string{"1", "2", "4", "8"}, 3, sampling_sources_select_callback).
                AddButton("Save", update_config).
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
