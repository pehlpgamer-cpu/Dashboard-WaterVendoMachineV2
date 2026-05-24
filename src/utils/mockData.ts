/**
 * Local-only mock data utilities.
 * Enable in development with mockData=true in URL query params or localStorage.
 */

export interface MockConfig {
  enabled: boolean
}

const MOCK_DATA_KEY = 'mockData'
const MOCK_DATA_ENABLED = 'true'

export function isMockDataEnabled(): boolean {
  if (!import.meta.env.DEV) {
    return false
  }

  const params = new URLSearchParams(window.location.search)
  if (params.get(MOCK_DATA_KEY) === MOCK_DATA_ENABLED) {
    localStorage.setItem(MOCK_DATA_KEY, MOCK_DATA_ENABLED)
    return true
  }

  return localStorage.getItem(MOCK_DATA_KEY) === MOCK_DATA_ENABLED
}

export function setMockDataMode(enabled: boolean): void {
  if (!import.meta.env.DEV) {
    return
  }

  if (enabled) {
    localStorage.setItem(MOCK_DATA_KEY, MOCK_DATA_ENABLED)
    window.location.href = '/?mockData=true'
  } else {
    localStorage.removeItem(MOCK_DATA_KEY)
    window.location.href = '/'
  }
}

export function clearMockDataMode(): void {
  localStorage.removeItem(MOCK_DATA_KEY)
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
    const amount = Math.random() < 0.3 ? 5 : 3 // 30% cold, 70% regular
    const isCold = amount === 5

    logs.push({
      id: `mock-${i}`,
      timestamp,
      amount,
      isCold,
      timeSynced: true,
      clientUptimeMs: i * 1000
    })
  }

  return logs.sort((a, b) => b.timestamp - a.timestamp)
}
