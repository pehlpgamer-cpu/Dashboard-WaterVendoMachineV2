import { FirebaseOptions, initializeApp } from 'firebase/app'
import { getAuth } from 'firebase/auth'
import { getFirestore } from 'firebase/firestore'

const requiredFirebaseEnv = [
  'VITE_FIREBASE_API_KEY',
  'VITE_FIREBASE_AUTH_DOMAIN',
  'VITE_FIREBASE_PROJECT_ID',
  'VITE_FIREBASE_STORAGE_BUCKET',
  'VITE_FIREBASE_MESSAGING_SENDER_ID',
  'VITE_FIREBASE_APP_ID'
] as const

function getRequiredEnv(name: typeof requiredFirebaseEnv[number]): string {
  const value = import.meta.env[name]

  if (!value || value.startsWith('YOUR_')) {
    throw new Error(`Missing required Firebase environment variable: ${name}`)
  }

  return value
}

const firebaseConfig: FirebaseOptions = {
  apiKey: getRequiredEnv('VITE_FIREBASE_API_KEY'),
  authDomain: getRequiredEnv('VITE_FIREBASE_AUTH_DOMAIN'),
  projectId: getRequiredEnv('VITE_FIREBASE_PROJECT_ID'),
  storageBucket: getRequiredEnv('VITE_FIREBASE_STORAGE_BUCKET'),
  messagingSenderId: getRequiredEnv('VITE_FIREBASE_MESSAGING_SENDER_ID'),
  appId: getRequiredEnv('VITE_FIREBASE_APP_ID')
}

const app = initializeApp(firebaseConfig)

export const auth = getAuth(app)

export const db = getFirestore(app)

export default app
