import { initializeApp } from "firebase/app";
import { getDatabase } from "firebase/database";
import { getAuth } from "firebase/auth";

const firebaseConfig = {
  apiKey: "AIzaSyAxufg9SmzOYyGkDnWOjeYKzDJm2mKbdH0",
  authDomain: "monitor-ccab4.firebaseapp.com",
  databaseURL:
    "https://monitor-ccab4-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "monitor-ccab4",
  storageBucket: "monitor-ccab4.firebasestorage.app",
  messagingSenderId: "904650770210",
  appId: "1:904650770210:web:609dfe56996da43bbb077b",
  measurementId: "G-D4P1F1TRR5",
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);

// Initialize Firebase services
const database = getDatabase(app);
const auth = getAuth(app);

export { app, database, auth };
