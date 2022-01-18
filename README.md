# f429-logic-analyzer
Logic analyzer implemented on STM32F429 Discovery with UART interface to host machine.

### How it works
Program starts with setting basic configuration including default UART and DMA settings needed for command handling (115200 baudrate). Then it reads commands sent through UART and applies proper configuration changes. After detecting run command it starts the timer configured according to given sampling frequency.
The timer triggers interrupt routine which reads proper pins and saves them to temporary buffer. When the buffer is full the program switches pointer to buffer to another one and triggers DMA transfer of full buffer through UART. To stop sampling in real-time mode the button needs to be pressed, which triggers interrupt routine that stops sampling, sends remaining data and returns to the beginning of the program.

#### Commands
* **run** – start sampling
* **baud x** – set baudrate to **x**
* **src x** – sample **x** pins simultaneously (1, 2, 4 or 8)
* **freq x** – set sampling frequency to **x** Hz
* **mode 0/1** \* – set mode to real–time (program samples channels and sends them through UART when buffer is full, does both things in parallel) or non–real–time (program stores data internally and sends data after sampling is over)      \**Currently only real–time mode is possible*

Commands have to be sent individually with fixed size of 20 chars, i.e. to send **run** command one has to send "run" and 17 chars being i.e. nulls or spaces. It is required because receiving chars through UART is done via DMA which triggers interrupt after receiving 20 bytes of data.

#### UI app
UI application was made in Java. It serves as a user-devices interface. It has the ability to show different data channels (up to 4 charts can be comfortably showed).
It also can send commands (listed above) to the device. Moreover user can change time of measurement which will be plotted on the charts.

#### TUI app
TUI application was made in Go. It can be used to send commands to device. Configuring parameters and saving them results in sending proper commands
to device through UART. Starting sampling tries to flush and discard any data that is present on UART, then it sends **run** command and saves received
samples to binary file. Stopping sampling stops reading from UART and closes the file.

##### Tested "limits"
* Sampling 8 pins simultaneously with frequency of 200 kHz and sending them through 2M baudrate UART

