<template>
  <v-container class="container" fluid>
    <v-row>
      <v-col cols="2">
        <v-sheet>
          <v-card height="360px" width="175px">

            <v-slider class="slider" readonly thumb-label color="green" v-model="slider1" direction="vertical"
              label="Height (In)" track-size="50">

            </v-slider>
          </v-card>
        </v-sheet>
      </v-col>
      <v-col cols="10">
        <figure class="highcharts-figure">
          <div id="container"></div>
        </figure>
      </v-col>
    </v-row>
    <v-row>
      <v-col cols="8">
        <figure class="highcharts-figure">
          <div id="container0"></div>
        </figure>
      </v-col>
      <v-col cols="4">
        <v-sheet max-width="350px">
          <v-card class="mb-5" style="max-width: 350px" variant="tonal" color="primary" density="compact" rounded="lg"
            title="Water Level" subtitle="Capacity Remaining" align="center">
            <div id="container2"></div>
            <v-dialog width="500" v-model="isActive">
              <template v-slot:default="{ isActive }">
                <v-card title="OVERFLOW DETECTED" color="blue" background-color="primary darken-1" align="center">
                  <v-card-actions>
                    <v-spacer></v-spacer>
                  </v-card-actions>
                </v-card>
              </template>
            </v-dialog>
          </v-card>
        </v-sheet>
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
import { ref, reactive, watch, onMounted, onBeforeUnmount, computed, } from "vue";
import { useRoute, useRouter } from "vue-router";

// VARIABLES
const router = useRouter();
const route = useRoute();
const Mqtt = useMqttStore();
const { payload, payloadTopic } = storeToRefs(Mqtt);
const host = ref("www.yanacreations.com");
const port = ref(9002);
const point = ref(10);
const shift = ref(false);
let isActive = ref(false);
const waterHeight = computed(() => {
  if (!!payload.value) {
    return '${payload.value.waterheight.toFixed(2)} inches';
  }
}
);

const reServes = computed(() => {
  if (!!payload.value) {
    return '${payload.value.reserves.toFixed(2)} gallons';
  }
}
);

const percentage = computed(() => {
  if (!!payload.value) {
    return '${payload.value.percentage.toFixed(2)}';
  }
}
);
const reservesChart = ref(null);
const reservesGauge = ref(null);
const slider1 = ref(50);

var fm = new FluidMeter();
// FUNCTIONS




const CreateCharts = async () => {
  // TEMPERATURE CHART
  reservesChart.value = Highcharts.chart("container", {
    chart: {
      zoomType: "x",
      animation: false
    },
    title: { text: "Water Reserves(10 min)", align: "left" },

    yAxis: {
      title: {
        text: "Water level",
        style: { color: "#000000" },
      },
      labels: { format: "{value} Gal" },
    },

    tooltip: {
      pointFormat: "Temperature: {point.y} °C",
    },
    xAxis: {
      type: "datetime",
      title: { text: "Time", style: { color: "#000000" } },
    },
    tooltip: { shared: true },
    series: [
      {
        name: "Water",
        type: "area",
        data: [],
        turboThreshold: 0,
        color: Highcharts.getOptions().colors[0],
        // pointWidth: 1000,
      },
    ],
    plotOptions: {
      column: {
        pointPadding: 0,
        borderWidth: 0,
        groupPadding: 0,

        shadow: false
      },
      bar: {
        horizontal: false,
        columnWidth: '100%',
        endingShape: 'rounded',
      },
    },
  });

  reservesGauge.value = Highcharts.chart("container0", {
    title: { text: 'Water Reserves', align: 'left' },
    // the value axis
    yAxis: {
      min: 0,
      max: 1000,
      tickPixelInterval: 72,
      tickPosition: 'inside',
      tickColor: Highcharts.defaultOptions.chart.backgroundColor || '#FFFFFF',
      tickLength: 20,
      tickWidth: 2,
      minorTickInterval: null,
      labels: { distance: 20, style: { fontSize: '14px' } },
      lineWidth: 0,
      plotBands: [{ from: 0, to: 200, color: '#DF5353', thickness: 20 }, {
        from: 200, to: 600, color: '#DDDF0D', thickness: 20
      }, { from: 600, to: 1000, color: '#55BF3B', thickness: 20 }]
    },
    tooltip: { shared: true, },
    pane: { startAngle: -90, endAngle: 89.9, background: null, center: ['50%', '75%'], size: '110%' },
    series: [{
      type: 'gauge',
      name: 'Water Capacity',
      data: [0],
      tooltip: { valueSuffix: ' Gal' },
      dataLabels: {
        format: '{y} Gal', borderWidth: 0, color: (Highcharts.defaultOptions.title &&
          Highcharts.defaultOptions.title.style && Highcharts.defaultOptions.title.style.color) || '#333333', style: { fontSize: '16px' }
      },
      dial: { radius: '80%', backgroundColor: 'gray', baseWidth: 12, baseLength: '0%', rearLength: '0%' },
      pivot: { backgroundColor: 'gray', radius: 6 }
    }],
  });

  fm.init({
    targetContainer: document.getElementById("container2"),
    fillPercentage: percentage,
    options: {
      fontSize: "70px",
      fontFamily: "Arial",
      fontFillStyle: "white",
      drawShadow: true,
      drawText: true,
      drawPercentageSign: true,
      drawBubbles: true,
      size: 300,
      borderWidth: 25,
      backgroundColor: "#e2e2e2",
      foregroundColor: "#fafafa",
      foregroundFluidLayer: {
        fillStyle: "purple",
        angularSpeed: 100,
        maxAmplitude: 12,
        frequency: 30,
        horizontalSpeed: -150,
      },
      backgroundFluidLayer: {
        fillStyle: "pink",
        angularSpeed: 100,
        maxAmplitude: 9,
        frequency: 30,
        horizontalSpeed: 150,
      },
    },
  });

};


onMounted(() => {
  // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
  Mqtt.connect(); // Connect to Broker located on the backend
  setTimeout(() => {
    // Subscribe to each topic
    Mqtt.subscribe("620164974");
    Mqtt.subscribe("620164974_pub");
    Mqtt.subscribe("620164974_sub");
  }, 3000);
  CreateCharts();
});

onBeforeUnmount(() => {
  // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
  Mqtt.unsubcribeAll();
});


watch(payload, (data) => {
  // THIS FUNCTION IS CALLED WHEN THE VALUE OF THE VARIABLE "payload" CHANGES

  if (reservesChart.value.series[0].points.value > 500) { reservesChart.value.series[0].points.value--; }
  else { shift.value = true; }

  slider1.value = data.radar

  if (data.waterheight >= 77) {
    fm.setPercentage(100);
    isActive.value = true;

    reservesChart.value.series[0].addPoint({ y: parseFloat(data.reserve.toFixed(2)), x: data.timestamp * 1000 }, true, shift.values); // Add new data point
    reservesGauge.value.series[0].points[0].update(1000); // Add new data point
  }
  else if (data.waterheight <= 0) {
    fm.setPercentage(0);
    reservesChart.value.series[0].addPoint({ y: 0, x: data.timestamp * 1000 }, true, shift.values); // Add new data point
    reservesGauge.value.series[0].points[0].update(0); // Add new data point
    isActive.value = false;

  }
  else {
    fm.setPercentage(data.percentage.toFixed(2));
    reservesChart.value.series[0].addPoint({ y: parseFloat(data.reserve.toFixed(2)), x: data.timestamp * 1000 }, true, shift.values); // Add new data point
    reservesGauge.value.series[0].points[0].update(parseFloat(data.reserve.toFixed(2)));
    isActive.value = false;
  }
});


</script>

<style scoped>
/** CSS STYLE HERE */

.container {
  background-color: "surface";
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
}

.slider {
  height: 250;
  height: 4rem;
  border-radius: 2rem;
}

figure {
  border: 2px solid black;
}
</style>
