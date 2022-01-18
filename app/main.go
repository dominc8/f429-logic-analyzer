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
    "time"
)
var all_baudrates = [...]string {
		"4000000",
		"3500000",
		"3000000",
		"2500000",
		"2000000",
		"1500000",
		"1152000",
		"1000000",
		"921600",
		"576000",
		"500000",
		"460800",
		"230400",
		"115200",
		"57600",
		"38400",
		"19200",
		"9600",
		"4800",
		"2400",
		"1800",
		"1200",
		"600",
		"300",
		"200",
		"150",
		"134",
		"110",
		"75",
		"50",
	}

type sampling_state struct {
    channel chan int
    is_sampling bool
}

type device_config struct {
    mode int
    baudrate int
    sampling_freq int
    sampling_sources int
}

func (cfg *device_config) init() {
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
    config device_config
    serial_port *serial.Port
    app *tview.Application
    list *tview.List
    form *tview.Form
    sampl_state sampling_state
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

func flush_port(port *serial.Port) {
    port.Flush()
    n_all := 0
    for {
        buf := make([]byte, 256)
        n, err := port.Read(buf)
        n_all += n
        if err != nil || n == 0 {
            log.Printf("Breaking flushing, n_all=%v, err=%v", n_all, err)
            break
        }
    }
}

func cmd_run() {
    if sampl_state.is_sampling == false {
        flush_port(serial_port)
        send_command("run")
        serial_port.Close()
        connect_to_device()
        list.SetItemText(0, "Stop sampling", "")
        sampl_state.is_sampling = true
        sampl_state.channel = make(chan int)
        go read_data()
    } else {
        close (sampl_state.channel)
        flush_port(serial_port)
        sampl_state.is_sampling = false
        list.SetItemText(0, "Start sampling", "")
    }
}

func connect_to_device() {
    serial_config := &serial.Config{Name: "/dev/ttyACM0", Baud: config.baudrate, ReadTimeout: time.Second * 5}
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
    buf := make([]byte, 1024)

    for {
        select {
            case _, ok := <- sampl_state.channel:
                if ok == false {
                    return
                }
            default:
                n, err := serial_port.Read(buf)
                if err == nil && n > 0 {
                    binary.Write(outf, binary.LittleEndian, buf[:n])
                }
        }
    }
}

func main() {
    config.init()

    log_file, _ := os.OpenFile("log.txt", os.O_APPEND|os.O_RDWR|os.O_CREATE, 0644)
    defer log_file.Close()
    log.SetOutput(log_file)
    log.SetFlags(log.Lshortfile|log.LstdFlags)

    app = tview.NewApplication()
    list = tview.NewList().
                AddItem("Start sampling", "", 'a', cmd_run).
                AddItem("Configure parameters", "", 'b', func() {
                    if !sampl_state.is_sampling {
                        app.SetFocus(form)
                    }
                }).
                AddItem("Quit", "", 'q', func() {
                    app.Stop()
                })

    form = tview.NewForm().
                AddDropDown("Mode", []string{"RT", "NRT"}, 0, func(_ string, index int) {
                    config.mode = index
                }).
                AddDropDown("Baud rate", all_baudrates[:], 13, func(s string, _ int) {
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
