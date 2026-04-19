// ============================================
// REPLACE WITH YOUR FIREBASE CONFIG
// Get these from Firebase Console > Project Settings
// ============================================

import { initializeApp } from 'firebase/app'
import { getAuth } from 'firebase/auth'
import { getFirestore } from 'firebase/firestore'

const firebaseConfig = {
  apiKey: "YOUR_API_KEY",
  authDomain: "pauloheymann-integ-prog.firebaseapp.com",
  projectId: "pauloheymann-integ-prog",
  storageBucket: "pauloheymann-integ-prog.appspot.com",
  messagingSenderId: "YOUR_MESSAGING_SENDER_ID",
  appId: "YOUR_APP_ID"
}

// Initialize Firebase
const app = initializeApp(firebaseConfig)

// Initialize Firebase Authentication and get a reference to the service
export const auth = getAuth(app)

// Initialize Cloud Firestore and get a reference to the service
export const db = getFirestore(app)

export default app
