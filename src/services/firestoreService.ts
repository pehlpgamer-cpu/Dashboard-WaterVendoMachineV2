import {
  collection,
  query,
  where,
  orderBy,
  onSnapshot,
  getDocs,
  Timestamp,
  DocumentData,
  Query,
  QueryConstraint
} from 'firebase/firestore'
import { db } from './firebaseConfig'

export interface WaterLog {
  id?: string
  amount: number
  isCold: boolean
  timestamp: number // milliseconds
}

export class FirestoreService {
  private unsubscribers: (() => void)[] = []

  // Real-time listener for water logs
  subscribeToWaterLogs(
    callback: (logs: WaterLog[]) => void,
    startDate?: Date,
    endDate?: Date
  ) {
    const constraints: QueryConstraint[] = []

    if (startDate) {
      constraints.push(where('timestamp', '>=', startDate.getTime()))
    }
    if (endDate) {
      constraints.push(where('timestamp', '<=', endDate.getTime()))
    }

    constraints.push(orderBy('timestamp', 'desc'))

    const q = query(
      collection(db, 'waterLogs'),
      ...constraints
    )

    const unsubscribe = onSnapshot(q, (querySnapshot) => {
      const logs: WaterLog[] = []
      querySnapshot.forEach((doc) => {
        logs.push({
          id: doc.id,
          ...doc.data()
        } as WaterLog)
      })
      callback(logs)
    })

    this.unsubscribers.push(unsubscribe)
    return unsubscribe
  }

  // Fetch water logs for a date range (one-time fetch)
  async fetchWaterLogs(startDate?: Date, endDate?: Date): Promise<WaterLog[]> {
    const constraints: QueryConstraint[] = []

    if (startDate) {
      constraints.push(where('timestamp', '>=', startDate.getTime()))
    }
    if (endDate) {
      constraints.push(where('timestamp', '<=', endDate.getTime()))
    }

    constraints.push(orderBy('timestamp', 'desc'))

    const q = query(
      collection(db, 'waterLogs'),
      ...constraints
    )

    const querySnapshot = await getDocs(q)
    const logs: WaterLog[] = []

    querySnapshot.forEach((doc) => {
      logs.push({
        id: doc.id,
        ...doc.data()
      } as WaterLog)
    })

    return logs
  }

  // Unsubscribe from all listeners (cleanup)
  unsubscribeAll() {
    this.unsubscribers.forEach(unsub => unsub())
    this.unsubscribers = []
  }
}

export default new FirestoreService()
