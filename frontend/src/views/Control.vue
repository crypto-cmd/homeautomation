<template>
  <v-container class="container" justify="center" align="center">
    <v-row justify="center">
      <v-col justify="center">
        <v-card class="text" color="surface" title="Combination" subtitle="Enter your four digit passcode" flat>
          <v-card-item>
            <v-otp-input v-model="passcode" focused focus-all length="4"></v-otp-input>
            <v-btn class="my-5" color="blue" text="Submit" variant="tonal" @click="submit()"></v-btn>
          </v-card-item>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup>
/** JAVASCRIPT HERE */

// IMPORTS
import { ref, reactive, watch, onMounted, onBeforeUnmount, computed, } from "vue";
import { useRoute, useRouter } from "vue-router";
import { useAppStore } from "@/store/appStore";
import { useMqttStore } from "@/store/mqttStore"; // Import Mqtt Store
import { storeToRefs } from "pinia";


// VARIABLES
const Mqtt        = useMqttStore();
const AppStore    = useAppStore();
const router      = useRouter();
const route       = useRoute();
var passcode      = ref("0000");


// FUNCTIONS
onMounted(() => {
  // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
  Mqtt.connect(); // Connect to Broker located on the backend
  setTimeout(() => {
    // Subscribe to each topic
    Mqtt.subscribe("620164974");
    Mqtt.subscribe("620164974_sub");
  }, 3000);
});

onBeforeUnmount(() => {
  // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
  Mqtt.unsubcribeAll();
});

const submit = async () => {
  await AppStore.setPasscode(passcode.value);
};

</script>

<style scoped>
/** CSS STYLE HERE */
</style>
