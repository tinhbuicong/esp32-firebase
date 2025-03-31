<template>
  <div class="sensor-simulator">
    <h2>Sensor Simulator</h2>

    <div class="sensor-controls">
      <div class="sensor-group">
        <h3>Temperature Sensor</h3>
        <div class="control-group">
          <label>Temperature (°C):</label>
          <input
            type="number"
            v-model="temperature"
            step="0.1"
            min="-40"
            max="80"
          />
        </div>
      </div>

      <div class="sensor-group">
        <h3>Humidity Sensor</h3>
        <div class="control-group">
          <label>Humidity (%):</label>
          <input
            type="number"
            v-model="humidity"
            step="0.1"
            min="0"
            max="100"
          />
        </div>
      </div>

      <div class="sensor-group">
        <h3>Gas Sensor</h3>
        <div class="control-group">
          <label>Gas Level (PPM):</label>
          <input type="number" v-model="gasLevel" step="1" min="0" max="1000" />
        </div>
      </div>

      <div class="sensor-group">
        <h3>Flame Sensor</h3>
        <div class="control-group">
          <label>Flame Level:</label>
          <input
            type="number"
            v-model="flameLevel"
            step="1"
            min="0"
            max="1024"
          />
        </div>
      </div>
    </div>

    <div class="simulation-controls">
      <div class="auto-update">
        <label>
          <input
            type="checkbox"
            v-model="autoUpdate"
            @change="toggleAutoUpdate"
          />
          Auto Update (every 5 seconds)
        </label>
      </div>

      <button class="send-button" @click="sendData" :disabled="sending">
        {{ sending ? "Sending..." : "Send Data" }}
      </button>
    </div>

    <div v-if="lastUpdate" class="last-update">
      Last update: {{ lastUpdate }}
    </div>

    <div v-if="error" class="error">
      {{ error }}
    </div>
  </div>
</template>

<script>
import { ref, onUnmounted } from "vue";
import { database } from "@/config/firebase";
import { ref as dbRef, set } from "firebase/database";

export default {
  name: "SensorSimulator",
  setup() {
    const temperature = ref(25.0);
    const humidity = ref(50.0);
    const gasLevel = ref(0);
    const flameLevel = ref(0);
    const autoUpdate = ref(false);
    const sending = ref(false);
    const lastUpdate = ref(null);
    const error = ref(null);
    let autoUpdateInterval = null;

    const sendData = async () => {
      try {
        sending.value = true;
        error.value = null;

        const timestamp = new Date().toISOString();
        const sensorData = {
          temperature: temperature.value,
          humidity: humidity.value,
          gasLevel: gasLevel.value,
          flameLevel: flameLevel.value,
          timestamp: timestamp,
        };

        // Send current data to sensors node
        await set(dbRef(database, "sensors"), sensorData);

        // Store log entry
        const logRef = dbRef(
          database,
          `logs/${timestamp.replace(/[.:]/g, "-")}`
        );
        await set(logRef, sensorData);

        lastUpdate.value = new Date().toLocaleString();
      } catch (err) {
        console.error("Error sending data:", err);
        error.value = `Error sending data: ${err.message}`;
      } finally {
        sending.value = false;
      }
    };

    const toggleAutoUpdate = () => {
      if (autoUpdate.value) {
        // Start auto-update
        autoUpdateInterval = setInterval(sendData, 5000);
        sendData(); // Send immediately
      } else {
        // Stop auto-update
        if (autoUpdateInterval) {
          clearInterval(autoUpdateInterval);
          autoUpdateInterval = null;
        }
      }
    };

    onUnmounted(() => {
      if (autoUpdateInterval) {
        clearInterval(autoUpdateInterval);
      }
    });

    return {
      temperature,
      humidity,
      gasLevel,
      flameLevel,
      autoUpdate,
      sending,
      lastUpdate,
      error,
      sendData,
      toggleAutoUpdate,
    };
  },
};
</script>

<style scoped>
.sensor-simulator {
  padding: 20px;
  max-width: 800px;
  margin: 0 auto;
}

.sensor-controls {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 20px;
  margin-bottom: 30px;
}

.sensor-group {
  background: #f8f9fa;
  padding: 20px;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
}

.sensor-group h3 {
  margin-top: 0;
  color: #2c3e50;
  font-size: 1.2em;
}

.control-group {
  display: flex;
  align-items: center;
  gap: 10px;
}

.control-group label {
  min-width: 120px;
}

.control-group input {
  width: 100px;
  padding: 8px;
  border: 1px solid #ddd;
  border-radius: 4px;
}

.simulation-controls {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
  padding: 20px;
  background: #f8f9fa;
  border-radius: 8px;
}

.auto-update {
  display: flex;
  align-items: center;
  gap: 10px;
}

.send-button {
  padding: 10px 20px;
  background-color: #4caf50;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 1em;
  transition: background-color 0.2s;
}

.send-button:hover {
  background-color: #45a049;
}

.send-button:disabled {
  background-color: #cccccc;
  cursor: not-allowed;
}

.last-update {
  text-align: center;
  color: #666;
  font-size: 0.9em;
  margin-top: 10px;
}

.error {
  color: #dc3545;
  text-align: center;
  margin-top: 10px;
  padding: 10px;
  background: #f8d7da;
  border-radius: 4px;
}
</style>
