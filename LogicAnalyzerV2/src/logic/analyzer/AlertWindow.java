package logic.analyzer;

import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.VBox;
import javafx.stage.Modality;
import javafx.stage.Stage;

public class AlertWindow {

    public static void display(String title, String message)
    {
        //class of pop up alert windows
        Stage window = new Stage();

        // Can't interact with other widows until you do with this
        window.initModality(Modality.APPLICATION_MODAL);
        window.setTitle(title);
        window.setMinWidth(100);
        window.setMinHeight(100);

        // Set alert text
        Label label = new Label();
        label.setText(message);

        // Close button
        Button closeButton = new Button("Close");
        closeButton.setOnAction(e -> window.close());

        // Set the layout
        VBox layout = new VBox(10);
        // Adding button and label to the window
        layout.getChildren().addAll(label, closeButton);
        layout.setAlignment(Pos.CENTER);

        Scene scene = new Scene(layout);
        window.setScene(scene);
        window.showAndWait();
    }

}
