<template>
  <div class="sensor-logs">
    <h2>Sensor Data Logs</h2>

    <div class="log-controls">
      <div class="filter-group">
        <label>Filter by Date:</label>
        <input type="date" v-model="selectedDate" @change="filterLogs" />
      </div>
      <div class="filter-group">
        <label>Show entries:</label>
        <select v-model="entriesToShow" @change="filterLogs">
          <option value="10">10</option>
          <option value="25">25</option>
          <option value="50">50</option>
          <option value="100">100</option>
        </select>
      </div>
      <button class="clear-button" @click="clearLogs">Clear Logs</button>
    </div>

    <div class="logs-container">
      <div v-if="loading" class="loading">Loading logs...</div>
      <div v-else-if="error" class="error">
        {{ error }}
      </div>
      <div v-else-if="filteredLogs.length === 0" class="no-data">
        No logs found for the selected criteria
      </div>
      <div v-else class="logs-table">
        <table>
          <thead>
            <tr>
              <th>Timestamp</th>
              <th>Temperature (°C)</th>
              <th>Humidity (%)</th>
              <th>Gas Level (PPM)</th>
              <th>Flame Level</th>
            </tr>
          </thead>
          <tbody>
            <tr v-for="log in filteredLogs" :key="log.timestamp">
              <td>{{ formatDate(log.timestamp) }}</td>
              <td>{{ log.temperature }}</td>
              <td>{{ log.humidity }}</td>
              <td>{{ log.gasLevel }}</td>
              <td>{{ log.flameLevel }}</td>
            </tr>
          </tbody>
        </table>
      </div>
    </div>
  </div>
</template>

<script>
import { ref, onMounted, computed, onUnmounted } from "vue";
import { database } from "@/config/firebase";
import { ref as dbRef, query, limitToLast, onValue } from "firebase/database";

const STORAGE_KEY = "sensor_logs";
const MAX_LOGS = 1000; // Maximum number of logs to store locally

export default {
  name: "SensorLogs",
  setup() {
    const logs = ref([]);
    const loading = ref(true);
    const error = ref(null);
    const selectedDate = ref("");
    const entriesToShow = ref("25");
    let unsubscribe = null;

    const filteredLogs = computed(() => {
      let filtered = [...logs.value];

      // Filter by date if selected
      if (selectedDate.value) {
        const selectedDateObj = new Date(selectedDate.value);
        filtered = filtered.filter((log) => {
          const logDate = new Date(log.timestamp);
          return logDate.toDateString() === selectedDateObj.toDateString();
        });
      }

      // Sort by timestamp (newest first)
      filtered.sort((a, b) => new Date(b.timestamp) - new Date(a.timestamp));

      // Limit number of entries
      return filtered.slice(0, parseInt(entriesToShow.value));
    });

    const formatDate = (timestamp) => {
      const date = new Date(timestamp);
      return date.toLocaleString();
    };

    // Save logs to localStorage
    const saveLogsToStorage = (newLogs) => {
      try {
        localStorage.setItem(STORAGE_KEY, JSON.stringify(newLogs));
      } catch (err) {
        console.error("Error saving logs to localStorage:", err);
      }
    };

    // Load logs from localStorage
    const loadLogsFromStorage = () => {
      try {
        const storedLogs = localStorage.getItem(STORAGE_KEY);
        if (storedLogs) {
          return JSON.parse(storedLogs);
        }
        return [];
      } catch (err) {
        console.error("Error loading logs from localStorage:", err);
        return [];
      }
    };

    // Add new log to storage
    const addLogToStorage = (newLog) => {
      try {
        const currentLogs = loadLogsFromStorage();
        currentLogs.unshift(newLog); // Add new log at the beginning

        // Keep only the most recent MAX_LOGS entries
        if (currentLogs.length > MAX_LOGS) {
          currentLogs.length = MAX_LOGS;
        }

        saveLogsToStorage(currentLogs);
      } catch (err) {
        console.error("Error adding log to storage:", err);
      }
    };

    const fetchLogs = () => {
      try {
        loading.value = true;
        error.value = null;

        // Load logs from localStorage
        logs.value = loadLogsFromStorage();
        loading.value = false;

        // Also listen to Firebase for new logs
        const logsRef = dbRef(database, "logs");
        const logsQuery = query(logsRef, limitToLast(100));

        unsubscribe = onValue(
          logsQuery,
          (snapshot) => {
            const data = snapshot.val();
            if (data) {
              const newLogs = Object.values(data);
              // Add new logs to localStorage
              newLogs.forEach((log) => {
                if (
                  !logs.value.some(
                    (existingLog) => existingLog.timestamp === log.timestamp
                  )
                ) {
                  addLogToStorage(log);
                }
              });
              logs.value = loadLogsFromStorage();
            }
            loading.value = false;
          },
          (err) => {
            console.error("Error fetching logs:", err);
            error.value = `Error loading logs: ${err.message}`;
            loading.value = false;
          }
        );
      } catch (err) {
        console.error("Error in fetchLogs:", err);
        error.value = `An unexpected error occurred: ${err.message}`;
        loading.value = false;
      }
    };

    const filterLogs = () => {
      // The filtering is handled by the computed property
      console.log("Filtering logs...");
    };

    // Clear logs from storage
    const clearLogs = () => {
      if (
        confirm(
          "Are you sure you want to clear all logs? This action cannot be undone."
        )
      ) {
        localStorage.removeItem(STORAGE_KEY);
        logs.value = [];
      }
    };

    onMounted(() => {
      fetchLogs();
    });

    onUnmounted(() => {
      if (unsubscribe) {
        unsubscribe();
      }
    });

    return {
      logs,
      loading,
      error,
      selectedDate,
      entriesToShow,
      filteredLogs,
      formatDate,
      filterLogs,
      clearLogs,
    };
  },
};
</script>

<style scoped>
.sensor-logs {
  padding: 20px;
  max-width: 1200px;
  margin: 0 auto;
}

.log-controls {
  display: flex;
  justify-content: center;
  gap: 20px;
  margin-bottom: 20px;
  padding: 15px;
  background: #f8f9fa;
  border-radius: 8px;
  flex-wrap: wrap;
}

.filter-group {
  display: flex;
  align-items: center;
  gap: 10px;
}

.filter-group label {
  font-weight: 500;
}

.filter-group input,
.filter-group select {
  padding: 8px;
  border: 1px solid #ddd;
  border-radius: 4px;
}

.logs-container {
  background: white;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
  overflow: hidden;
}

.loading,
.error,
.no-data {
  text-align: center;
  padding: 20px;
  font-size: 1.1em;
}

.error {
  color: #dc3545;
  background: #f8d7da;
}

.no-data {
  color: #6c757d;
}

.logs-table {
  overflow-x: auto;
}

table {
  width: 100%;
  border-collapse: collapse;
  text-align: left;
}

th,
td {
  padding: 12px 15px;
  border-bottom: 1px solid #dee2e6;
}

th {
  background-color: #f8f9fa;
  font-weight: 600;
  color: #495057;
}

tr:hover {
  background-color: #f8f9fa;
}

.clear-button {
  padding: 8px 16px;
  background-color: #dc3545;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 0.9em;
  transition: background-color 0.2s;
}

.clear-button:hover {
  background-color: #c82333;
}

@media (max-width: 768px) {
  .log-controls {
    flex-direction: column;
    align-items: stretch;
  }

  .filter-group {
    justify-content: space-between;
  }

  .clear-button {
    width: 100%;
    margin-top: 10px;
  }
}
</style>
