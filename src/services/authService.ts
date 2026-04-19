import {
  createUserWithEmailAndPassword,
  signInWithEmailAndPassword,
  signOut,
  onAuthStateChanged,
  User
} from 'firebase/auth'
import { auth } from './firebaseConfig'

export class AuthService {
  // Sign up with email and password
  async signup(email: string, password: string): Promise<User> {
    const result = await createUserWithEmailAndPassword(auth, email, password)
    return result.user
  }

  // Sign in with email and password
  async login(email: string, password: string): Promise<User> {
    const result = await signInWithEmailAndPassword(auth, email, password)
    return result.user
  }

  // Sign out
  async logout(): Promise<void> {
    await signOut(auth)
  }

  // Listen to auth state changes
  onAuthStateChanged(callback: (user: User | null) => void) {
    return onAuthStateChanged(auth, callback)
  }

  // Get current user
  getCurrentUser(): User | null {
    return auth.currentUser
  }

  // Get auth state promise
  getAuthState(): Promise<User | null> {
    return new Promise((resolve) => {
      const unsubscribe = onAuthStateChanged(auth, (user) => {
        unsubscribe()
        resolve(user)
      })
    })
  }
}

export default new AuthService()
