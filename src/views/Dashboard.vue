<template>
  <div class="min-h-screen bg-gray-100">
    <!-- Header -->
    <header class="bg-white shadow">
      <div class="max-w-7xl mx-auto px-4 py-6 flex justify-between items-center">
        <div class="flex items-center gap-4">
          <h1 class="text-3xl font-bold text-gray-800">Water Vending Dashboard</h1>
          <span v-if="devModeEnabled" class="px-3 py-1 bg-amber-100 text-amber-800 text-sm font-semibold rounded-full">
            DEV MODE
          </span>
        </div>
        <button
          @click="handleLogout"
          class="px-4 py-2 bg-red-500 hover:bg-red-600 text-white rounded transition"
        >
          Logout
        </button>
      </div>
    </header>

    <!-- Main Content -->
    <main class="max-w-7xl mx-auto px-4 py-8">
      <!-- Statistics Cards -->
      <StatisticsCards />

      <!-- Machine Status & Export -->
      <div class="grid grid-cols-1 lg:grid-cols-3 gap-6 mb-6">
        <div class="lg:col-span-2">
          <MachineStatus />
        </div>
        <div>
          <ExportPanel />
        </div>
      </div>

      <!-- Filters -->
      <FilterPanel />

      <!-- Charts Grid -->
      <div class="grid grid-cols-1 lg:grid-cols-2 gap-6 mb-6">
        <SalesOverTimeChart />
        <ColdVsRegularChart />
        <RevenueByDateChart />
        <HourlySalesChart />
        <TopHoursChart />
        <ComparisonChart />
      </div>

      <!-- Transaction History -->
      <TransactionHistory />

      <!-- Alerts -->
      <AlertNotifications />
    </main>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue'
import { useRouter } from 'vue-router'
import { useDashboardStore } from '@/stores/dashboardStore'
import firestoreService from '@/services/firestoreService'
import authService from '@/services/authService'
import { isDevModeEnabled } from '@/utils/devMode'

import StatisticsCards from '@/components/StatisticsCards.vue'
import MachineStatus from '@/components/MachineStatus.vue'
import ExportPanel from '@/components/ExportPanel.vue'
import FilterPanel from '@/components/FilterPanel.vue'
import SalesOverTimeChart from '@/components/charts/SalesOverTimeChart.vue'
import ColdVsRegularChart from '@/components/charts/ColdVsRegularChart.vue'
import RevenueByDateChart from '@/components/charts/RevenueByDateChart.vue'
import HourlySalesChart from '@/components/charts/HourlySalesChart.vue'
import TopHoursChart from '@/components/charts/TopHoursChart.vue'
import ComparisonChart from '@/components/charts/ComparisonChart.vue'
import TransactionHistory from '@/components/TransactionHistory.vue'
import AlertNotifications from '@/components/AlertNotifications.vue'

const router = useRouter()
const store = useDashboardStore()
const devModeEnabled = ref(isDevModeEnabled())

let unsubscribe: (() => void) | null = null

onMounted(() => {
  // Only subscribe to Firestore updates in production mode
  if (!devModeEnabled.value) {
    unsubscribe = firestoreService.subscribeToWaterLogs((logs) => {
      store.setLogs(logs)
    })
  }

  // Request notification permission
  if ('Notification' in window && Notification.permission === 'default') {
    Notification.requestPermission()
  }
})

onUnmounted(() => {
  if (unsubscribe) {
    unsubscribe()
  }
  firestoreService.unsubscribeAll()
})

const handleLogout = async () => {
  try {
    if (!devModeEnabled.value) {
      await authService.logout()
    } else {
      localStorage.removeItem('devMode')
    }
    router.push('/login')
  } catch (err) {
    console.error('Logout failed:', err)
  }
}
</script>
