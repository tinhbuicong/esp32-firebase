<template>
  <div id="app">
    <nav class="navigation">
      <button
        :class="{ active: currentView === 'display' }"
        @click="currentView = 'display'"
      >
        View Data
      </button>
      <button
        :class="{ active: currentView === 'simulator' }"
        @click="currentView = 'simulator'"
      >
        Sensor Simulator
      </button>
      <button
        :class="{ active: currentView === 'logs' }"
        @click="currentView = 'logs'"
      >
        View Logs
      </button>
    </nav>

    <h1>ESP32 Sensor Monitoring</h1>

    <DataDisplay v-if="currentView === 'display'" />
    <SensorSimulator v-if="currentView === 'simulator'" />
    <SensorLogs v-if="currentView === 'logs'" />
  </div>
</template>

<script>
import { ref } from "vue";
import DataDisplay from "./components/DataDisplay.vue";
import SensorSimulator from "./components/SensorSimulator.vue";
import SensorLogs from "./components/SensorLogs.vue";

export default {
  name: "App",
  components: {
    DataDisplay,
    SensorSimulator,
    SensorLogs,
  },
  setup() {
    const currentView = ref("display");
    return { currentView };
  },
};
</script>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 20px;
}

h1 {
  color: #2c3e50;
  margin-bottom: 30px;
}

.navigation {
  margin-bottom: 20px;
  padding: 10px;
  background: #f8f9fa;
  border-radius: 8px;
  display: flex;
  justify-content: center;
  gap: 10px;
}

.navigation button {
  padding: 10px 20px;
  border: none;
  border-radius: 4px;
  background: #e9ecef;
  color: #495057;
  cursor: pointer;
  font-size: 1em;
  transition: all 0.2s;
}

.navigation button:hover {
  background: #dee2e6;
}

.navigation button.active {
  background: #4caf50;
  color: white;
}
</style>
