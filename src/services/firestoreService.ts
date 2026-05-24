import {
  collection,
  query,
  where,
  orderBy,
  onSnapshot,
  getDocs,
  limit as limitQuery,
  QueryConstraint
} from 'firebase/firestore'
import { db } from './firebaseConfig'

export interface WaterLog {
  id?: string
  amount: number
  isCold: boolean
  timestamp: number // milliseconds
  timeSynced?: boolean
  clientUptimeMs?: number
}

export class FirestoreService {
  private unsubscribers: (() => void)[] = []
  private readonly defaultResultLimit = 1000

  // Real-time listener for water logs
  subscribeToWaterLogs(
    callback: (logs: WaterLog[]) => void,
    startDate?: Date,
    endDate?: Date,
    maxResults = this.defaultResultLimit
  ) {
    const constraints: QueryConstraint[] = []

    if (startDate) {
      constraints.push(where('timestamp', '>=', startDate.getTime()))
    }
    if (endDate) {
      constraints.push(where('timestamp', '<=', endDate.getTime()))
    }

    constraints.push(orderBy('timestamp', 'desc'))
    constraints.push(limitQuery(maxResults))

    const q = query(
      collection(db, 'waterLogs'),
      ...constraints
    )

    const unsubscribe = onSnapshot(q, (querySnapshot) => {
      const logs: WaterLog[] = []
      querySnapshot.forEach((doc) => {
        const log = this.toWaterLog(doc.id, doc.data())
        if (log) {
          logs.push(log)
        }
      })
      callback(logs)
    })

    this.unsubscribers.push(unsubscribe)
    return unsubscribe
  }

  // Fetch water logs for a date range (one-time fetch)
  async fetchWaterLogs(startDate?: Date, endDate?: Date, maxResults = this.defaultResultLimit): Promise<WaterLog[]> {
    const constraints: QueryConstraint[] = []

    if (startDate) {
      constraints.push(where('timestamp', '>=', startDate.getTime()))
    }
    if (endDate) {
      constraints.push(where('timestamp', '<=', endDate.getTime()))
    }

    constraints.push(orderBy('timestamp', 'desc'))
    constraints.push(limitQuery(maxResults))

    const q = query(
      collection(db, 'waterLogs'),
      ...constraints
    )

    const querySnapshot = await getDocs(q)
    const logs: WaterLog[] = []

    querySnapshot.forEach((doc) => {
      const log = this.toWaterLog(doc.id, doc.data())
      if (log) {
        logs.push(log)
      }
    })

    return logs
  }

  private toWaterLog(id: string, data: Record<string, unknown>): WaterLog | null {
    const { amount, isCold, timestamp, timeSynced, clientUptimeMs } = data

    if (
      typeof amount !== 'number' ||
      typeof isCold !== 'boolean' ||
      typeof timestamp !== 'number' ||
      !Number.isFinite(timestamp)
    ) {
      return null
    }

    const log: WaterLog = { id, amount, isCold, timestamp }

    if (typeof timeSynced === 'boolean') {
      log.timeSynced = timeSynced
    }

    if (typeof clientUptimeMs === 'number' && Number.isFinite(clientUptimeMs)) {
      log.clientUptimeMs = clientUptimeMs
    }

    return log
  }

  // Unsubscribe from all listeners (cleanup)
  unsubscribeAll() {
    this.unsubscribers.forEach(unsub => unsub())
    this.unsubscribers = []
  }
}

export default new FirestoreService()
