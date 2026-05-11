/**
 * Development mode utilities for testing with mock data
 * Enable: devMode=true in URL query params or localStorage
 * Usage: http://localhost:5173/?devMode=true
 */

export interface MockConfig {
  enabled: boolean
}

const DEV_MODE_KEY = 'devMode'
const DEV_MODE_ENABLED = 'true'

export function isDevModeEnabled(): boolean {
  // Check URL params
  const params = new URLSearchParams(window.location.search)
  if (params.get(DEV_MODE_KEY) === DEV_MODE_ENABLED) {
    localStorage.setItem(DEV_MODE_KEY, DEV_MODE_ENABLED)
    return true
  }

  // Check localStorage
  return localStorage.getItem(DEV_MODE_KEY) === DEV_MODE_ENABLED
}

export function setDevMode(enabled: boolean): void {
  if (enabled) {
    localStorage.setItem(DEV_MODE_KEY, DEV_MODE_ENABLED)
    window.location.href = '/?devMode=true'
  } else {
    localStorage.removeItem(DEV_MODE_KEY)
    window.location.href = '/'
  }
}

export function getDevModeStatus(): string {
  return isDevModeEnabled() ? 'DEV MODE (Mock Data)' : 'Production'
}

/**
 * Generate mock water logs for demo purposes
 */
export function generateMockWaterLogs(count: number = 500) {
  const logs = []
  const now = Date.now()
  const sevenDaysMs = 7 * 24 * 60 * 60 * 1000

  for (let i = 0; i < count; i++) {
    const timestamp = now - Math.random() * sevenDaysMs
    const amount = Math.random() < 0.3 ? 10 : 15 // 30% cold (10 units), 70% regular (15 units)
    const isCold = amount === 10

    logs.push({
      id: `mock-${i}`,
      timestamp,
      amount,
      isCold
    })
  }

  return logs.sort((a, b) => b.timestamp - a.timestamp)
}
