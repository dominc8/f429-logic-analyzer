package logic.analyzer;

import javafx.event.EventHandler;
import javafx.scene.Scene;
import javafx.scene.control.CheckBox;
import javafx.scene.control.TextField;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.GridPane;
import javafx.stage.Modality;
import javafx.stage.Stage;

import java.io.IOException;

class MyEventHandler {
    USBHandler usbHandler;
    Stage optionsWindow;
    MyEventHandler(){}
}

class RunButtonHandler extends MyEventHandler implements EventHandler<MouseEvent> {
    private Stage chartsWindow;
    private DataCollector dataCollector;

    RunButtonHandler(USBHandler handlerIn){
        usbHandler = handlerIn;
        dataCollector = handlerIn.getDataColector();
    }

    @Override
    public void handle(MouseEvent event) {

        usbHandler.showMeWhatIsInside();
        usbHandler.connectAndRead();

        System.out.println("RUN PRESSED");

        Stage window = new Stage();
        // Can't interact with other widows until you do with this
        window.initModality(Modality.APPLICATION_MODAL);
        window.setTitle("Channel charts");
        window.setMinWidth(800);
        window.setMinHeight(600);

        chartsWindow = window;
        GridPane mainBody = Layout.mainBody();
        Chart.addChartsToLayout(mainBody, dataCollector);

        Scene scene = new Scene(mainBody);
        chartsWindow.setScene(scene);
        chartsWindow.showAndWait();
    }
}

class OptionsButtonHandler extends MyEventHandler implements EventHandler<MouseEvent> {

    public OptionsButtonHandler (USBHandler handlerIn){
        usbHandler = handlerIn;
    }

    @Override
    public void handle(MouseEvent event) {
        System.out.println("OPTIONS PRESSED");

        //class of pop up option window
        Stage window = new Stage();
        optionsWindow = window;

        // Can't interact with other widows until you do with this
        window.initModality(Modality.APPLICATION_MODAL);
        window.setTitle("Options");
        window.setMinWidth(200);
        window.setMinHeight(400);

        Scene scene = new Scene(Layout.optionsWindow(this));
        window.setScene(scene);
        window.showAndWait();
    }
}

class SaveButtonHandler extends MyEventHandler implements EventHandler<MouseEvent> {
    private TextField baudrateTextField;
    private TextField timeTextField;
    private CheckBox[] channelList;

    SaveButtonHandler(TextField baudRateIn, TextField timeIn, OptionsButtonHandler handlerIn, CheckBox[] channelListIn) {
        usbHandler = handlerIn.usbHandler;
        optionsWindow = handlerIn.optionsWindow;
        baudrateTextField = baudRateIn;
        timeTextField = timeIn;
        channelList = channelListIn;

    }

    @Override
    public void handle(MouseEvent event) {
        System.out.println("SAVE AND CLOSE");
        usbHandler.setParams(Integer.parseInt(baudrateTextField.getText()),
                Integer.parseInt(timeTextField.getText()), channelList);
        optionsWindow.close();
    }
}

class SendCommandHandler extends MyEventHandler implements EventHandler<MouseEvent> {
    private TextField commandTextField;

    SendCommandHandler(TextField commandTextFieldIn, OptionsButtonHandler handlerIn) {
        usbHandler = handlerIn.usbHandler;
        optionsWindow = handlerIn.optionsWindow;
        commandTextField = commandTextFieldIn;
    }

    @Override
    public void handle(MouseEvent event) {

        System.out.println("SEND COMMAND");
        String commandText = commandTextField.getText();
        StringBuffer outputBuffer = new StringBuffer(20);

        for (int i = 0; i < commandText.length(); ++i){
            outputBuffer.append(commandText.charAt(i));
        }

        outputBuffer.append("\n".repeat(Math.max(0, 25 - commandText.length())));
        System.out.println(outputBuffer);

        try
        {
            usbHandler.connectAndWrite(outputBuffer.toString());
        } catch (IOException e)
        {
            AlertWindow.display("ERROR", e.getMessage());
        }
    }
}
