package logic.analyzer;

import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.*;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;

class Layout {

    static HBox menuBar(USBHandler handlerIn) {

        HBox hBox = new HBox();
        hBox.setPadding(new Insets(15, 12, 15, 12));
        hBox.setSpacing(10);
        hBox.setStyle("-fx-background-color: #336699");

        Button runButton = new Button("Run >>");
        runButton.setPrefSize(100,20);
        runButton.addEventHandler(MouseEvent.MOUSE_CLICKED, new RunButtonHandler(handlerIn));

        Button optionsButton = new Button("Options");
        optionsButton.setPrefSize(100, 20);
        optionsButton.addEventHandler(MouseEvent.MOUSE_CLICKED, new OptionsButtonHandler(handlerIn));

        // Add buttons to the stage layout
        hBox.getChildren().addAll(runButton, optionsButton);

        return hBox;
    }

    static GridPane mainBody() {
        GridPane grid = new GridPane();
        grid.setPadding(new Insets( 10, 10, 10, 10));
        // Distance between grid cells
        grid.setHgap(10);
        grid.setStyle("-fx-background-color: rgba(0,0,0,0.77)");
        grid.setPrefHeight(100);
        grid.setPrefWidth(800);

        return grid;
    }

    static GridPane optionsWindow(OptionsButtonHandler handlerIn) {

        // Defining layout elements

        Label baudrate = new Label("Baudrate:");
        TextField baudrateTextField = new TextField("115200");

        Label time = new Label("Time:");
        TextField timeTextField = new TextField("100");

        Label command = new Label("Send command:");
        TextField commandTextField = new TextField("run");

        // Adding dropdown menu choice for channel number
        Label channelLabel = new Label("Enable channel:");
        CheckBox[] channelList = new CheckBox[8];
        for (int i = 0; i <  channelList.length; ++i) {
            channelList[i] = new CheckBox(Integer.toString(i + 1));
        }
        channelList[0].setSelected(true);

        // Save button configuration
        Button saveButton = new Button("Save and close");
        saveButton.addEventHandler(MouseEvent.MOUSE_CLICKED,
                new SaveButtonHandler(baudrateTextField, timeTextField, handlerIn, channelList));

        // Save button configuration
        Button sendCommandButton = new Button("Send command");
        sendCommandButton.addEventHandler(MouseEvent.MOUSE_CLICKED,
                new SendCommandHandler(commandTextField, handlerIn));

        HBox hbSaveButton = new HBox(10);
        hbSaveButton.setAlignment(Pos.BOTTOM_RIGHT);
        hbSaveButton.getChildren().add(saveButton);

        HBox hbCommandButton = new HBox(10);
        hbCommandButton.setAlignment(Pos.BOTTOM_RIGHT);
        hbCommandButton.getChildren().add(sendCommandButton);

        // Set options window layout
        GridPane grid = new GridPane();
        grid.setAlignment(Pos.CENTER);
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(25, 25, 25, 25));

        // Adding elements to the layout
        grid.add(baudrate, 0, 1);
        grid.add(baudrateTextField, 1, 1);
        grid.add(time, 0, 2);
        grid.add(timeTextField, 1,2);
        grid.add(command, 0, 3);
        grid.add(commandTextField, 1,3);
        grid.add(channelLabel, 0,4);
        for(int j = 0 ; j < channelList.length; ++j) {
            grid.add(channelList[j], 1, (j + 4) );
        }
        grid.add(saveButton, 1, 12, 2, 1);
        grid.add(sendCommandButton, 1, 13, 2, 1);
        return grid;
    }
}
