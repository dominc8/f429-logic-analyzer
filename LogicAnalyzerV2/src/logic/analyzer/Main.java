package logic.analyzer;

import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.stage.Stage;

import java.beans.EventHandler;


public class Main extends Application
{
    private static final boolean DEBUG_MODE = false;

    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) throws Exception {

        DataCollector dataCollector = new DataCollector();
        USBHandler usbHandler = new USBHandler(dataCollector);
        usbHandler.showMeWhatIsInside();

        if (!DEBUG_MODE) {
            primaryStage.setTitle("Logic Analyzer v2.0");

            HBox menuBar = Layout.menuBar(usbHandler);

            Scene scene = new Scene(menuBar);
            scene.getStylesheets().add("LineChart.css");
            primaryStage.setScene(scene);

            primaryStage.show();
        }
    }
}
