package logic.analyzer;

import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.layout.GridPane;

public class Chart {
    public static javafx.scene.chart.LineChart<Number, Number> createChart(XYChart.Series<Number, Number> dataIn)
    {
        // x axis
        NumberAxis xAxis = new NumberAxis();
        xAxis.setLabel("Test X label");

        // y axis
        NumberAxis yAxis = new NumberAxis();
        yAxis.setLabel("Test Y label");

        javafx.scene.chart.LineChart<Number, Number> lineChart =
                new javafx.scene.chart.LineChart<Number, Number>(xAxis, yAxis);

        // Adding data to the chart
        lineChart.getData().add(dataIn);

        // Cosmetics of the chart
        lineChart.setLegendVisible(false);

        return lineChart;
    }

    public static void addChartsToLayout(GridPane layoutIn, int numbOfCharts)
    {
        // Ganerates numbOfCharts charts and adds them to the grid layout
        if(numbOfCharts > 4)
        {
            AlertWindow.display("Error!", "Too many charts! Please provide " +
                    "number between 1 to 4");
            return;
        }

        for(int i = 0; i < numbOfCharts; ++i)
        {
             javafx.scene.chart.LineChart<Number, Number> chart = Chart.createChart(DataCollector.generateSampleData
                     (100, 100));
             GridPane.setConstraints(chart, 0, i);
             chart.prefWidthProperty().bind(layoutIn.widthProperty());
             layoutIn.getChildren().add(chart);
        }
    }
}
