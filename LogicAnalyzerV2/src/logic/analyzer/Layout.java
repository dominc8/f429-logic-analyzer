package logic.analyzer;

import javafx.geometry.Insets;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;

public class Layout {

    public static HBox menuBar()
    {
        HBox hBox = new HBox();
        hBox.setPadding(new Insets(15, 12, 15, 12));
        hBox.setSpacing(10);
        hBox.setStyle("-fx-background-color: #336699");

        Button runButton = new Button("Run >>");
        runButton.setPrefSize(100,20);

        Button optionsButton = new Button("Options");
        optionsButton.setPrefSize(100, 20);

        // Add buttons to the stage layout
        hBox.getChildren().addAll(runButton, optionsButton);

        return hBox;
    }

    public static GridPane mainBody()
    {
        GridPane grid = new GridPane();
        grid.setPadding(new Insets( 10, 10, 10, 10));
        // Distance between grid cells
        grid.setHgap(10);
        grid.setStyle("-fx-background-color: rgba(0,0,0,0.77)");
        grid.setPrefHeight(100);
        grid.setPrefWidth(800);

        return grid;
    }

}
