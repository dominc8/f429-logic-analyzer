package logic.analyzer;

import javafx.collections.FXCollections;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.layout.GridPane;
import javafx.scene.shape.Rectangle;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.stream.Collectors;

class Chart {
    private static javafx.scene.chart.LineChart<Number, Number> createChart(XYChart.Series<Number, Number> dataIn)
    {
        // Making point of chart invisible
        Rectangle rect = new Rectangle(0,0);
        rect.setVisible(false);
        dataIn.setNode(rect);

        // x axis
        NumberAxis xAxis = new NumberAxis();
        xAxis.setLabel("Test X label");

        // y axis
        NumberAxis yAxis = new NumberAxis();
        yAxis.setLabel("Test Y label");

        javafx.scene.chart.LineChart<Number, Number> lineChart =
                new javafx.scene.chart.LineChart<Number, Number>(xAxis, yAxis);

        // Adding data to the chart
        try {
            lineChart.getData().add(dataIn);
        }
        catch (Exception e) {
            System.out.println(e.getMessage());
        }

        // Cosmetics of the chart
        lineChart.setLegendVisible(false);

        return lineChart;
    }

    static void addChartsToLayout(GridPane layoutIn, DataCollector dataCollectorIn)
    {
        XYChart.Series<Number, Number> dataOut = new XYChart.Series<>();
        int iterator = 0;

        File file = new File("output.txt");
        try{
            Scanner sc = new Scanner(file);

            while (sc.hasNextLine()) {
                dataOut.getData().add(new XYChart.Data<Number, Number>(iterator, Integer.parseInt(sc.nextLine())));
                iterator++;
            }

            sc.close();

        } catch (Exception e) {
            AlertWindow.display("ERROR", e.getMessage());
        }

        XYChart.Series<Number, Number> data_tmp = copySeries(dataOut);
        int numbOfCharts = dataCollectorIn.getChannelStates().size();

        if(numbOfCharts > 8)
        {
            AlertWindow.display("Error!", "Too many charts!. How is this possible");
            return;
        }

        List<Integer> channelStates = new ArrayList<Integer>();
        channelStates = dataCollectorIn.getChannelStates();

        for(int i = 0; i < numbOfCharts; ++i)
        {

             for(XYChart.Data<Number, Number> data : data_tmp.getData()) {
                 data.setYValue(((int)data.getYValue() >>> channelStates.get(i)) & 0x01);
             }

             javafx.scene.chart.LineChart<Number, Number> chart = Chart.createChart(data_tmp);
             GridPane.setConstraints(chart, 0, i);
             chart.prefWidthProperty().bind(layoutIn.widthProperty());
             layoutIn.getChildren().add(chart);

             data_tmp = copySeries(dataOut);
        }
    }

    private static <S, T> XYChart.Series<S, T> copySeries(XYChart.Series<S, T> series) {
        return new XYChart.Series<>(series.getName(),
                series.getData().stream()
                        .map(data -> new XYChart.Data<S, T>(data.getXValue(), data.getYValue()))
                        .collect(Collectors.toCollection(FXCollections::observableArrayList)));
    }

}
