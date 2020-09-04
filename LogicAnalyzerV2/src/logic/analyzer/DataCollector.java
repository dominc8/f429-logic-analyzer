package logic.analyzer;

import javafx.scene.chart.XYChart;

import java.util.List;
import java.util.Random;

public class DataCollector {

    private XYChart.Series<Number, Number> collectedData;
    private List<Integer> channelStates;
    DataCollector() {
        collectedData = generateSampleData(100,2);
    }

    public static XYChart.Series<Number, Number> generateSampleData(int sampleCount, int maxYValue)
    {
        //Generate random data
        XYChart.Series<Number, Number> data = new XYChart.Series<>();

        //Generate radnom seed
        Random rand = new Random();

        for(int i = 0; i < sampleCount; ++i)
        {
            data.getData().add(new XYChart.Data<Number, Number>(i, rand.nextInt(maxYValue)));
        }

        return data;
    }
    void setCollectedData(XYChart.Series<Number, Number> dataIn, List<Integer> collectedChannelStatesIn)
    {
        this.collectedData = dataIn;
        this.channelStates = collectedChannelStatesIn;
    }
    XYChart.Series<Number, Number> getCollectedData()
    {
        return this.collectedData;
    }

    List<Integer> getChannelStates()
    {
        return this.channelStates;
    }
}
