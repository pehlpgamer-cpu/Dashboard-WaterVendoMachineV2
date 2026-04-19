<template>
  <div class="bg-white p-6 rounded-lg shadow mb-6">
    <div class="flex items-center justify-between mb-6">
      <h3 class="text-lg font-semibold">Filters</h3>
      <button
        @click="store.resetFilters"
        class="px-4 py-2 text-sm bg-gray-200 hover:bg-gray-300 rounded transition"
      >
        Reset Filters
      </button>
    </div>

    <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4 mb-6">
      <!-- Time Period Quick Filters -->
      <div>
        <label class="block text-sm font-medium mb-2">Time Period</label>
        <div class="flex gap-2">
          <button
            v-for="period in ['today', 'week', 'month', 'all']"
            :key="period"
            @click="store.setTimeRange(period as any)"
            :class="[
              'px-3 py-1 text-sm rounded transition',
              store.selectedTimeRange === period
                ? 'bg-blue-500 text-white'
                : 'bg-gray-100 hover:bg-gray-200'
            ]"
          >
            {{ capitalize(period) }}
          </button>
        </div>
      </div>

      <!-- Water Type Filter -->
      <div>
        <label class="block text-sm font-medium mb-2">Water Type</label>
        <select
          :value="store.selectedWaterType"
          @change="store.setWaterType(($event.target as HTMLSelectElement).value as any)"
          class="w-full px-3 py-2 border border-gray-300 rounded focus:outline-none focus:ring-2 focus:ring-blue-500"
        >
          <option value="all">All Types</option>
          <option value="cold">Cold Water</option>
          <option value="regular">Regular Water</option>
        </select>
      </div>

      <!-- Hour Range Filter -->
      <div>
        <label class="block text-sm font-medium mb-2">Hour Range</label>
        <input
          type="range"
          min="0"
          max="23"
          :value="store.selectedHourRange.start"
          @change="store.setHourRange(parseInt(($event.target as HTMLInputElement).value), store.selectedHourRange.end)"
          class="w-full"
        />
        <p class="text-xs text-gray-600 mt-1">
          {{ store.selectedHourRange.start }}:00 - {{ store.selectedHourRange.end }}:00
        </p>
      </div>

      <!-- Date Range Picker -->
      <div>
        <label class="block text-sm font-medium mb-2">Date Range</label>
        <div class="flex gap-2">
          <input
            type="date"
            :value="formatDateInput(store.startDate)"
            @change="handleStartDateChange"
            class="flex-1 px-3 py-2 border border-gray-300 rounded text-sm focus:outline-none focus:ring-2 focus:ring-blue-500"
          />
          <input
            type="date"
            :value="formatDateInput(store.endDate)"
            @change="handleEndDateChange"
            class="flex-1 px-3 py-2 border border-gray-300 rounded text-sm focus:outline-none focus:ring-2 focus:ring-blue-500"
          />
        </div>
      </div>
    </div>

    <div class="text-sm text-gray-600">
      Showing {{ store.filteredLogs.length }} transactions
    </div>
  </div>
</template>

<script setup lang="ts">
import { useDashboardStore } from '@/stores/dashboardStore'
import { format } from 'date-fns'

const store = useDashboardStore()

const capitalize = (str: string) => str.charAt(0).toUpperCase() + str.slice(1)

const formatDateInput = (date: Date) => format(date, 'yyyy-MM-dd')

const handleStartDateChange = (event: Event) => {
  const value = (event.target as HTMLInputElement).value
  const date = new Date(value)
  store.setDateRange(date, store.endDate)
}

const handleEndDateChange = (event: Event) => {
  const value = (event.target as HTMLInputElement).value
  const date = new Date(value)
  store.setDateRange(store.startDate, date)
}
</script>
