<template>
  <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4 mb-6">
    <!-- Total Cups -->
    <div class="bg-gradient-to-br from-blue-500 to-blue-600 text-white p-6 rounded-lg shadow">
      <div class="flex justify-between items-start">
        <div>
          <p class="text-blue-100 mb-2">Total Cups Sold</p>
          <h3 class="text-4xl font-bold">{{ store.totalCupsSold }}</h3>
        </div>
        <span class="text-4xl opacity-20">🥤</span>
      </div>
    </div>

    <!-- Total Revenue -->
    <div class="bg-gradient-to-br from-green-500 to-green-600 text-white p-6 rounded-lg shadow">
      <div class="flex justify-between items-start">
        <div>
          <p class="text-green-100 mb-2">Total Revenue</p>
          <h3 class="text-4xl font-bold">{{ formatPhilippinePeso(store.totalRevenue) }}</h3>
        </div>
        <span class="text-4xl opacity-20">💰</span>
      </div>
    </div>

    <!-- Average Price -->
    <div class="bg-gradient-to-br from-purple-500 to-purple-600 text-white p-6 rounded-lg shadow">
      <div class="flex justify-between items-start">
        <div>
          <p class="text-purple-100 mb-2">Avg Price/Transaction</p>
          <h3 class="text-4xl font-bold">{{ formatPhilippinePeso(store.averagePrice) }}</h3>
        </div>
        <span class="text-4xl opacity-20">💵</span>
      </div>
    </div>

    <!-- Cold vs Regular -->
    <div class="bg-gradient-to-br from-cyan-500 to-cyan-600 text-white p-6 rounded-lg shadow">
      <div class="flex justify-between items-start">
        <div>
          <p class="text-cyan-100 mb-2">Cold vs Regular</p>
          <p class="text-2xl font-bold">{{ store.coldVsRegularSplit.coldPercent }}% / {{ store.coldVsRegularSplit.regularPercent }}%</p>
        </div>
        <span class="text-4xl opacity-20">❄️</span>
      </div>
    </div>

    <!-- Sales per Hour -->
    <div class="bg-gradient-to-br from-orange-500 to-orange-600 text-white p-6 rounded-lg shadow">
      <div class="flex justify-between items-start">
        <div>
          <p class="text-orange-100 mb-2">Sales/Hour Avg</p>
          <h3 class="text-4xl font-bold">{{ store.salesPerHour }}</h3>
        </div>
        <span class="text-4xl opacity-20">⏱️</span>
      </div>
    </div>

    <!-- Last Transaction -->
    <div class="bg-gradient-to-br from-pink-500 to-pink-600 text-white p-6 rounded-lg shadow">
      <div class="flex justify-between items-start">
        <div>
          <p class="text-pink-100 mb-2">Last Transaction</p>
          <p v-if="store.lastTransaction" class="text-lg font-semibold">
            {{ store.lastTransaction.isCold ? 'Cold' : 'Regular' }} - {{ formatPhilippinePeso(store.lastTransaction.amount) }}
          </p>
          <p v-else class="text-lg font-semibold">No transactions</p>
          <p v-if="store.lastTransaction" class="text-sm text-pink-100 mt-1">
            {{ formatTime(store.lastTransaction.timestamp) }}
            <span v-if="store.lastTransaction.timeSynced === false" class="ml-1">Approx.</span>
          </p>
        </div>
        <span class="text-4xl opacity-20">⏰</span>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { useDashboardStore } from '@/stores/dashboardStore'
import { formatDistanceToNow } from 'date-fns'
import { formatPhilippinePeso } from '@/utils/currency'

const store = useDashboardStore()

const formatTime = (date: Date) => {
  return formatDistanceToNow(date, { addSuffix: true })
}
</script>
