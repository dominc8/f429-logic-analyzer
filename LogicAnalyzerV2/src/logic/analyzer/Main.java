package logic.analyzer;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.stage.Stage;


public class Main extends Application
{
    Button button;

    public static void main(String[] args)
    {
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) throws Exception
    {
        primaryStage.setTitle("Logic Analyzer v1.0");

        BorderPane mainLayout = new BorderPane();
        HBox menuBar = Layout.menuBar();
        GridPane mainBody = Layout.mainBody();

        // Create some charts to display
        Chart.addChartsToLayout(mainBody, 3);

        mainLayout.setTop(menuBar);
        mainLayout.setCenter(mainBody);

        Scene scene = new Scene(mainLayout);
        scene.getStylesheets().add("LineChart.css");
        primaryStage.setScene(scene);

        primaryStage.show();
    }
}
