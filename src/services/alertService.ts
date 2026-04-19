export interface Alert {
  id: string
  type: 'warning' | 'danger' | 'info' | 'success'
  message: string
  timestamp: number
}

export class AlertService {
  private alerts: Map<string, Alert> = new Map()

  // Check if low water alert should trigger (cups > 48)
  checkLowWater(cupsSold: number): Alert | null {
    if (cupsSold > 48) {
      const alert: Alert = {
        id: 'low-water-' + Date.now(),
        type: 'warning',
        message: `⚠️ Low water alert: ${cupsSold}/50 cups sold. Refill soon!`,
        timestamp: Date.now()
      }
      this.alerts.set(alert.id, alert)
      return alert
    }
    return null
  }

  // Check if no activity alert should trigger (no transactions in 4+ hours)
  checkNoActivity(lastTransactionTime: number | null): Alert | null {
    if (!lastTransactionTime) return null

    const fourHoursMs = 4 * 60 * 60 * 1000
    const timeSinceLastTransaction = Date.now() - lastTransactionTime

    if (timeSinceLastTransaction > fourHoursMs) {
      const alert: Alert = {
        id: 'no-activity-' + Date.now(),
        type: 'info',
        message: `ℹ️ No activity for 4+ hours. Last transaction: ${new Date(lastTransactionTime).toLocaleTimeString()}`,
        timestamp: Date.now()
      }
      this.alerts.set(alert.id, alert)
      return alert
    }
    return null
  }

  // Get all active alerts
  getAlerts(): Alert[] {
    return Array.from(this.alerts.values())
  }

  // Remove alert
  removeAlert(id: string): void {
    this.alerts.delete(id)
  }

  // Clear all alerts
  clearAll(): void {
    this.alerts.clear()
  }

  // Trigger browser notification
  triggerBrowserNotification(title: string, options?: NotificationOptions): void {
    if ('Notification' in window && Notification.permission === 'granted') {
      new Notification(title, options)
    }
  }

  // Request notification permission
  requestNotificationPermission(): Promise<NotificationPermission> {
    if ('Notification' in window) {
      return Notification.requestPermission()
    }
    return Promise.resolve('denied')
  }
}

export default new AlertService()
