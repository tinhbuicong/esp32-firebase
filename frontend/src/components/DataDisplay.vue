<template>
  <div class="data-display">
    <h2>Sensor Data</h2>
    <div v-if="loading" class="loading">Loading data...</div>
    <div v-else-if="error" class="error">
      {{ error }}
      <p class="debug-info">
        Please check the browser console for more details.
      </p>
    </div>
    <div v-else class="data-container">
      <div v-for="(value, key) in sensorData" :key="key" class="data-card">
        <h3>{{ key }}</h3>
        <div class="data-content">
          <p v-for="(val, index) in value" :key="index">
            {{ index }}: {{ val }}
          </p>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import { ref, onMounted, onUnmounted } from "vue";
import { database } from "@/config/firebase";
import { ref as dbRef, onValue } from "firebase/database";

export default {
  name: "DataDisplay",
  setup() {
    const sensorData = ref({});
    const loading = ref(true);
    const error = ref(null);
    let unsubscribe = null;

    const fetchData = () => {
      try {
        loading.value = true;
        error.value = null;

        // Reference to the root of your database
        const dataRef = dbRef(database, "/"); // Explicitly reference root path

        console.log("Attempting to connect to Firebase...");
        console.log("Database URL:", database.app.options.databaseURL);

        // Set up real-time listener
        unsubscribe = onValue(
          dataRef,
          (snapshot) => {
            console.log("Data received:", snapshot.val());
            const data = snapshot.val();
            sensorData.value = data || {};
            loading.value = false;
          },
          (err) => {
            console.error("Error fetching data:", err);
            console.error("Error code:", err.code);
            console.error("Error message:", err.message);
            error.value = `Error loading data: ${
              err.message || "Unknown error"
            }`;
            loading.value = false;
          }
        );
      } catch (err) {
        console.error("Error in fetchData:", err);
        error.value = `An unexpected error occurred: ${err.message}`;
        loading.value = false;
      }
    };

    onMounted(() => {
      console.log("Component mounted, initializing Firebase connection...");
      fetchData();
    });

    onUnmounted(() => {
      if (unsubscribe) {
        unsubscribe();
      }
    });

    return {
      sensorData,
      loading,
      error,
    };
  },
};
</script>

<style scoped>
.data-display {
  padding: 20px;
  max-width: 1200px;
  margin: 0 auto;
}

.loading,
.error {
  text-align: center;
  padding: 20px;
  font-size: 1.2em;
}

.error {
  color: #dc3545;
}

.debug-info {
  font-size: 0.8em;
  color: #666;
  margin-top: 10px;
}

.data-container {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(300px, 1fr));
  gap: 20px;
  padding: 20px;
}

.data-card {
  background: #ffffff;
  border-radius: 8px;
  padding: 20px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
  transition: transform 0.2s;
}

.data-card:hover {
  transform: translateY(-5px);
}

.data-card h3 {
  margin-top: 0;
  color: #2c3e50;
  border-bottom: 2px solid #eee;
  padding-bottom: 10px;
}

.data-content {
  text-align: left;
}

.data-content p {
  margin: 8px 0;
  color: #666;
}
</style>
