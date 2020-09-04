package logic.analyzer;

import com.fazecast.jSerialComm.SerialPort;
import javafx.scene.chart.XYChart;
import javafx.scene.control.CheckBox;
import org.usb4java.*;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

public class USBHandler
{
    private int baudrate = 115200;
    private int time = 20;
    private DataCollector dataCollector;
    private CheckBox[] channelList;

    USBHandler(DataCollector dataCollectorIn) {
        dataCollector = dataCollectorIn;
    }

    public DataCollector getDataColector() {
        return dataCollector;
    }

    void showMeWhatIsInside() {
        System.out.println(baudrate);
        System.out.println(time);
    }

    void setParams(int baudrateIn, int timeIn, CheckBox[] channelListIn) {
      baudrate = baudrateIn;
      time = timeIn;
      channelList = channelListIn;
    }

    public static DeviceDescriptor findDevice(int targetIdVendor, int targetidProduct)
    {
        // Create the libusb context
        Context context = new Context();

        // Initialize the libusb context
        LibUsb.init(context);

        // Read the USB device list
        DeviceList list = new DeviceList();
        LibUsb.getDeviceList(context, list);

        DeviceDescriptor descriptorOut = null;

        try
        {
            // Iterate over all devices and look for target passed as parameters
            for (Device device: list)
            {
                DeviceDescriptor descriptor = new DeviceDescriptor();
                LibUsb.getDeviceDescriptor(device, descriptor);

                if (descriptor.idVendor() == targetIdVendor && descriptor.idProduct() == targetidProduct)
                {
                    System.out.println("I found it!");
                    descriptorOut = descriptor;
                    break;
                }
            }
        }
        finally
        {
            // Ensure the allocated device list is freed
            LibUsb.freeDeviceList(list, true);
        }
        // Deinitialize the libusb contex
        LibUsb.exit(context);
        return descriptorOut;
    }

    void connectAndRead()
    {
        XYChart.Series<Number, Number> dataOut = new XYChart.Series<>();

        for (SerialPort port : SerialPort.getCommPorts())
        {
            System.out.println(port.getPortDescription() + " " + port.getDescriptivePortName());

            if (port.getPortDescription().toLowerCase().contains("st-link")) {
                System.out.println("Port found.");

                // open port connection
                port.openPort();

                port.setBaudRate(baudrate);
                port.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 0, 0);

                try {

                    System.out.println("Trying to read data...");
                    InputStream in = port.getInputStream();
                    FileWriter fw = new FileWriter("output.txt", false);

                    //FileOutputStream outputStream = new FileOutputStream("chartData");

                    long t = System.currentTimeMillis();
                    long end = t + time;

                    while (System.currentTimeMillis() < end)
                    {
                        //outputStream.write((byte)in.read());
                        fw.write(in.read() + "\n");
                    }

                    //outputStream.close();
                    fw.close();
                }
                catch (Exception e) {
                    AlertWindow.display("ERROR", e.getMessage());
                }

                System.out.println("Closing port...");
                port.closePort();


                List<Integer> channelStates = new ArrayList<Integer>();

                for(int j = 0; j<channelList.length; ++j) {
                    if (channelList[j].isSelected()) {
                        channelStates.add(j);
                    }
                }

                // Send data you read to the Data collector
                dataCollector.setCollectedData(dataOut, channelStates);
            }
        }
    }

    void connectAndWrite(String commandToSend) throws IOException {
        for (SerialPort port : SerialPort.getCommPorts()) {
            System.out.println(port.getPortDescription() + " " + port.getDescriptivePortName());

            if (port.getPortDescription().toLowerCase().contains("st-link")) {
                System.out.println("Port found.");

                // open port connection
                port.openPort();
                port.setBaudRate(baudrate);
                port.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 0, 0);

                System.out.println("Trying to write data...");
                OutputStream out = port.getOutputStream();

                out.write(commandToSend.getBytes());
                System.out.println(commandToSend);

                out.close();
                port.closePort();
            }
        }
    }
}