<template>
  <v-container  fluid>
    <v-row class="row1">
      <v-col cols="4">
        <v-sheet class="sheet">
          <br />

          <v-text-field
            v-model="start"
            label="Start date"
            type="Date"
            dense
            solo-inverted
            class="mr-5"
            :style="{ padding: '17px', maxWidth: '300px' }"
            flat
          ></v-text-field>
          <v-text-field
            v-model="end"
            label="End date"
            type="Date"
            dense
            solo-inverted
            class="mr-5"
            :style="{ padding: '17px', maxWidth: '300px' }"
            flat
          ></v-text-field>
          <br />
          <v-btn
            class="text-none mb-4"
            @click="updateCards(); updateScatter(); updateLineCharts(); "
            id="analyze"
            color="surface"
            variant="elevated"
          >Analyze</v-btn>
        </v-sheet>
      </v-col>
      <v-col cols="3" align="center">
        <v-card
          title="Average"
          subtitle="For the selected period"
          width="350"
          height="200"
          variant="elevated"
          color="surface"
          density="compact"
          rounded="lg"
          align="center"
        >
          <v-card-item align="center">
            <span class="text-h1 text-black">
              {{ avg.value }}
            </span>
            <small>Gal</small>
          </v-card-item>
        </v-card>
      </v-col>
    </v-row>
    <v-row class="row">
      <v-col cols="12">
        <figure class="highcharts-figure">
          <div id="container"></div>
        </figure>
      </v-col>
      <v-col cols="12">
        <figure class="highcharts-figure">
          <div id="container0"></div>
        </figure>
      </v-col>
    </v-row>

  </v-container>
</template>

<script setup>
/** JAVASCRIPT HERE */

import Highcharts from "highcharts";
import more from "highcharts/highcharts-more";
import Exporting from "highcharts/modules/exporting";
import { withDirectives } from "vue";
Exporting(Highcharts);
more(Highcharts);

// IMPORTS
import { useMqttStore } from "@/store/mqttStore"; // Import Mqtt Store
import { storeToRefs } from "pinia";

import { useAppStore } from "@/store/appStore";
import {
  ref,
  reactive,
  watch,
  onMounted,
  onBeforeUnmount,
} from "vue";

// VARIABLES
const Mqtt = useMqttStore();
const AppStore = useAppStore();
const waterMngAnal = ref(null); // Chart object
const heightWaterLvl = ref(null); // Chart object
var start = ref(null);
var end = ref(null);
var avg= reactive({ value: 0 });
let waterheight =[];
// FUNCTIONS

const CreateCharts = async () => {
  // TEMPERATURE CHART
  waterMngAnal.value = Highcharts.chart("container", {
    chart: { zoomType: "x" },
    title: { text: "Water Management Analysis", align: "left" },

    yAxis: {
      title: {
        text: "Water Reserve",
        style: { color: "#000000" },
      },
      labels: { format: "{value} Gal" },
    },

    tooltip: {
      pointFormat: "Heatindex: {point.x} °C <br/> Temperature: {point.y} °C",
    },
    xAxis: {
      type: "datetime",
      title: { text: "Time", style: { color: "#000000" } },
    },
    tooltip: { shared: true },
    series: [
      {
        name: "Reserve",
        type: "line",
        data: [],
        turboThreshold: 0,
        color: Highcharts.getOptions().colors[0],
      },
    ],
  });

  heightWaterLvl.value = Highcharts.chart("container0", {
    chart: { zoomType: "x" },
    title: { text: "Height and Water Level Correlation Analysis", align: "left" },
    yAxis: {
      min: 0,
      title: {
        text: "Height",
        style: { color: "#000000" },
      },
      labels: { format: "{value} inches" },
    },

    tooltip: {
      pointFormat: "Humidity: {point.x} % ",
    },
    xAxis: {
      type: "datetime",
      title: { text: "Water Height", style: { color: "#000000" } },
    },
    tooltip: { shared: true },
    series: [
      {
        name: "Water Height",
        type: "scatter",
        data: [],
        turboThreshold: 0,
        color: Highcharts.getOptions().colors[0],
      },
    ],
  });



};

onMounted(() => {
  // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
  Mqtt.connect(); // Connect to Broker located on the backend
  setTimeout(() => {
    // Subscribe to each topic
    Mqtt.subscribe("620164974");
    Mqtt.subscribe("620164974_sub");
  }, 3000);
  CreateCharts();
});

onBeforeUnmount(() => {
  // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
  Mqtt.unsubcribeAll();
});

const updateLineCharts = async () => {
  if (!!start.value && !!end.value) {
    // Convert output from Textfield components to 10 digit timestamps
    let startDate = new Date(start.value).getTime() / 1000;
    let endDate = new Date(end.value).getTime() / 1000;
    // Fetch data from backend
    const data = await AppStore.getReserve(startDate, endDate);
    // Create arrays for each plot
    let reserve = [];


    // Iterate through data variable and transform object to format recognized by highcharts

    data.forEach((row) => {
      reserve.push({
        x: row.timestamp * 1000,
        y: parseFloat(row.reserve.toFixed(2)),
      });
      waterheight.push({
        x: row.timestamp * 1000,
        y: parseFloat(row.waterheight.toFixed(2)),
      });

    });
    console.log(reserve);
    console.log(waterheight);
    // Add data to Temperature and Heat Index chart
    waterMngAnal.value.series[0].setData(reserve);
    heightWaterLvl.value.series[0].setData(waterheight);

  }
};

const updateCards = async () => {
  // Retrieve Min, Max, Avg, Spread/Range
  if (!!start.value && !!end.value) {
    // 1. Convert start and end dates collected fron TextFields to 10 digit timestamps
    let startDate = new Date(start.value).getTime() / 1000;
    let endDate = new Date(end.value).getTime() / 1000;
    // 2. Fetch data from backend by calling the API functions
    const average = await AppStore.update_Card(startDate, endDate);
    console.log(average);
    avg.value = average[0].average.toFixed(1);


  }
};


const updateScatter = async () => {
  if (!!start.value && !!end.value) {
    console.log(waterheight);
    heightWaterLvl.value.series[0].setData(waterheight);
  }
};
</script>

<style scoped>
/** CSS STYLE HERE */

.container {
  background-color: #f5f5f5;
  width: 1200px;
}

.row {
  max-width: 1200px;
}

.row1 {
  max-width: 1200px;
  padding: 1;
}

.col1 {
  border: black;
}

.sheet {
  padding: 2;
  height: 250;
  border-radius: 10px;
  background-color:"primary";
}

#analyze {
  display: flex;
  justify-content: center;
  margin: 0 auto;
  align-items: center;
}

Figure {
  border: 2px solid black;
}
</style>
