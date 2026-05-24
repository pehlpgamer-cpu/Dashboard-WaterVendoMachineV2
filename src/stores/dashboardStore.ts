import { defineStore } from 'pinia'
import { ref, computed } from 'vue'
import { WaterLog } from '@/services/firestoreService'
import { format, startOfDay, endOfDay, startOfWeek, endOfWeek, startOfMonth, endOfMonth, getHours } from 'date-fns'
import { isMockDataEnabled, generateMockWaterLogs } from '@/utils/mockData'

export const useDashboardStore = defineStore('dashboard', () => {
  // State
  const logs = ref<WaterLog[]>(isMockDataEnabled() ? generateMockWaterLogs(500) : [])
  const startDate = ref<Date>(new Date(Date.now() - 7 * 24 * 60 * 60 * 1000)) // Last 7 days
  const endDate = ref<Date>(new Date())
  const selectedWaterType = ref<'all' | 'cold' | 'regular'>('all')
  const selectedTimeRange = ref<'today' | 'week' | 'month' | 'all'>('week')
  const selectedHourRange = ref<{ start: number; end: number }>({ start: 0, end: 23 })

  // Computed: Filter logs by all criteria
  const filteredLogs = computed(() => {
    return logs.value.filter(log => {
      const logDate = new Date(log.timestamp)

      // Check date range
      if (logDate < startDate.value || logDate > endDate.value) {
        return false
      }

      // Check water type
      if (selectedWaterType.value === 'cold' && !log.isCold) {
        return false
      }
      if (selectedWaterType.value === 'regular' && log.isCold) {
        return false
      }

      // Check hour range
      const hour = getHours(logDate)
      if (hour < selectedHourRange.value.start || hour > selectedHourRange.value.end) {
        return false
      }

      return true
    })
  })

  // ========== STATISTICS ==========

  const totalCupsSold = computed(() => filteredLogs.value.length)

  const totalRevenue = computed(() => {
    return filteredLogs.value.reduce((sum, log) => sum + log.amount, 0)
  })

  const averagePrice = computed(() => {
    if (filteredLogs.value.length === 0) return 0
    return (totalRevenue.value / filteredLogs.value.length).toFixed(2)
  })

  const coldVsRegularSplit = computed(() => {
    if (filteredLogs.value.length === 0) {
      return { cold: 0, regular: 0, coldPercent: 0, regularPercent: 0 }
    }
    const cold = filteredLogs.value.filter(l => l.isCold).length
    const regular = filteredLogs.value.length - cold
    const coldPercent = ((cold / filteredLogs.value.length) * 100).toFixed(1)
    const regularPercent = ((regular / filteredLogs.value.length) * 100).toFixed(1)

    return { cold, regular, coldPercent: parseFloat(coldPercent), regularPercent: parseFloat(regularPercent) }
  })

  const salesPerHour = computed(() => {
    if (filteredLogs.value.length === 0) return 0

    const timeRangeMs = endDate.value.getTime() - startDate.value.getTime()
    if (timeRangeMs <= 0) return 0

    const hours = timeRangeMs / (1000 * 60 * 60)
    return (filteredLogs.value.length / hours).toFixed(2)
  })

  const latestLog = computed(() => {
    if (logs.value.length === 0) return null
    return logs.value.reduce((latest, log) => {
      return log.timestamp > latest.timestamp ? log : latest
    }, logs.value[0])
  })

  const lastTransaction = computed(() => {
    if (!latestLog.value) return null

    return {
      timestamp: new Date(latestLog.value.timestamp),
      isCold: latestLog.value.isCold,
      amount: latestLog.value.amount,
      timeSynced: latestLog.value.timeSynced
    }
  })

  // ========== CHART DATA ==========

  // Chart 1: Sales Over Time (hourly/daily)
  const salesOverTimeData = computed(() => {
    const grouped: { [key: string]: { label: string; count: number } } = {}

    // Group by day if range > 30 days, else by hour
    const daysDiff = Math.ceil((endDate.value.getTime() - startDate.value.getTime()) / (1000 * 60 * 60 * 24))
    const groupByDay = daysDiff > 30

    filteredLogs.value.forEach(log => {
      const date = new Date(log.timestamp)
      const key = groupByDay ? format(date, 'yyyy-MM-dd') : format(date, 'yyyy-MM-dd HH')
      const label = groupByDay ? format(date, 'yyyy-MM-dd') : format(date, 'MMM d HH:00')

      grouped[key] = grouped[key] || { label, count: 0 }
      grouped[key].count += 1
    })

    const labels = Object.keys(grouped).sort()
    const data = labels.map(label => grouped[label].count)

    return { labels: labels.map(label => grouped[label].label), data }
  })

  // Chart 2: Cold vs Regular (Pie)
  const coldVsRegularData = computed(() => {
    const split = coldVsRegularSplit.value
    return {
      labels: ['Cold Water', 'Regular Water'],
      data: [split.cold, split.regular],
      colors: ['#3b82f6', '#f97316']
    }
  })

  // Chart 3: Revenue by Date (Bar)
  const revenueByDateData = computed(() => {
    const grouped: { [key: string]: { cold: number; regular: number } } = {}

    filteredLogs.value.forEach(log => {
      const date = new Date(log.timestamp)
      const key = format(date, 'yyyy-MM-dd')

      if (!grouped[key]) {
        grouped[key] = { cold: 0, regular: 0 }
      }

      if (log.isCold) {
        grouped[key].cold += log.amount
      } else {
        grouped[key].regular += log.amount
      }
    })

    const labels = Object.keys(grouped).sort()
    const coldData = labels.map(label => grouped[label].cold)
    const regularData = labels.map(label => grouped[label].regular)

    return { labels, coldData, regularData }
  })

  // Chart 4: Hourly Distribution (24 hours)
  const hourlyDistributionData = computed(() => {
    const hourly = Array(24).fill(0)

    filteredLogs.value.forEach(log => {
      const hour = getHours(new Date(log.timestamp))
      hourly[hour]++
    })

    return {
      labels: Array.from({ length: 24 }, (_, i) => `${i.toString().padStart(2, '0')}:00`),
      data: hourly
    }
  })

  // Chart 5: Top Hours (Top 5)
  const topHoursData = computed(() => {
    const hourly = Array(24).fill(0)

    filteredLogs.value.forEach(log => {
      const hour = getHours(new Date(log.timestamp))
      hourly[hour]++
    })

    const topFive = hourly
      .map((count, hour) => ({ hour, count }))
      .sort((a, b) => b.count - a.count)
      .slice(0, 5)

    return {
      labels: topFive.map(item => `${item.hour.toString().padStart(2, '0')}:00`),
      data: topFive.map(item => item.count)
    }
  })

  // Chart 6: Daily/Weekly/Monthly Comparison
  const comparisonData = computed(() => {
    // Get today, this week, this month data
    const now = new Date()
    const todayStart = startOfDay(now)
    const todayEnd = endOfDay(now)
    const weekStart = startOfWeek(now)
    const weekEnd = endOfWeek(now)
    const monthStart = startOfMonth(now)
    const monthEnd = endOfMonth(now)

    const todayCount = logs.value.filter(l => {
      const d = new Date(l.timestamp)
      return d >= todayStart && d <= todayEnd
    }).length

    const weekCount = logs.value.filter(l => {
      const d = new Date(l.timestamp)
      return d >= weekStart && d <= weekEnd
    }).length

    const monthCount = logs.value.filter(l => {
      const d = new Date(l.timestamp)
      return d >= monthStart && d <= monthEnd
    }).length

    return {
      labels: ['Today', 'This Week', 'This Month'],
      data: [todayCount, weekCount, monthCount]
    }
  })

  // ========== PAGINATION FOR TABLE ==========
  const transactionHistory = computed(() => {
    return filteredLogs.value.map(log => ({
      ...log,
      timestamp: new Date(log.timestamp)
    }))
  })

  // ========== MACHINE STATUS ==========
  const machineStatus = computed(() => {
    if (!lastTransaction.value) {
      return { status: 'offline', message: 'No transactions yet' }
    }

    const timeDiff = Date.now() - lastTransaction.value.timestamp.getTime()
    const twoHoursMs = 2 * 60 * 60 * 1000

    if (timeDiff > twoHoursMs) {
      const minutesAgo = Math.floor(timeDiff / (1000 * 60))
      return { status: 'offline', message: `Last seen ${minutesAgo} minutes ago` }
    }

    return { status: 'online', message: 'Machine is active' }
  })

  // ========== ACTIONS ==========

  function setLogs(newLogs: WaterLog[]) {
    logs.value = newLogs
  }

  function setDateRange(start: Date, end: Date) {
    startDate.value = start
    endDate.value = end
  }

  function setTimeRange(range: 'today' | 'week' | 'month' | 'all') {
    selectedTimeRange.value = range
    const now = new Date()

    switch (range) {
      case 'today':
        startDate.value = startOfDay(now)
        endDate.value = endOfDay(now)
        break
      case 'week':
        startDate.value = new Date(now.getTime() - 7 * 24 * 60 * 60 * 1000)
        endDate.value = now
        break
      case 'month':
        startDate.value = startOfMonth(now)
        endDate.value = endOfMonth(now)
        break
      case 'all':
        startDate.value = new Date(0)
        endDate.value = now
        break
    }
  }

  function setWaterType(type: 'all' | 'cold' | 'regular') {
    selectedWaterType.value = type
  }

  function setHourRange(start: number, end: number) {
    selectedHourRange.value = { start, end }
  }

  function resetFilters() {
    selectedWaterType.value = 'all'
    selectedTimeRange.value = 'week'
    selectedHourRange.value = { start: 0, end: 23 }
    startDate.value = new Date(Date.now() - 7 * 24 * 60 * 60 * 1000)
    endDate.value = new Date()
  }

  return {
    // State
    logs,
    startDate,
    endDate,
    selectedWaterType,
    selectedTimeRange,
    selectedHourRange,
    filteredLogs,

    // Statistics
    totalCupsSold,
    totalRevenue,
    averagePrice,
    coldVsRegularSplit,
    salesPerHour,
    lastTransaction,
    latestLog,

    // Chart Data
    salesOverTimeData,
    coldVsRegularData,
    revenueByDateData,
    hourlyDistributionData,
    topHoursData,
    comparisonData,

    // Table
    transactionHistory,

    // Machine Status
    machineStatus,

    // Actions
    setLogs,
    setDateRange,
    setTimeRange,
    setWaterType,
    setHourRange,
    resetFilters
  }
})
