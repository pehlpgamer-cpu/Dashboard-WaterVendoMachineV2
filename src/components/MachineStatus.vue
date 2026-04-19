<template>
  <div class="bg-white p-6 rounded-lg shadow mb-6">
    <div class="flex items-center justify-between mb-4">
      <h3 class="text-lg font-semibold">Machine Status</h3>
      <div class="flex items-center gap-3">
        <div
          :class="[
            'w-3 h-3 rounded-full',
            store.machineStatus.status === 'online' ? 'bg-green-500 animate-pulse' : 'bg-red-500'
          ]"
        ></div>
        <div>
          <p class="font-medium">{{ store.machineStatus.status === 'online' ? 'Online' : 'Offline' }}</p>
          <p class="text-sm text-gray-600">{{ store.machineStatus.message }}</p>
        </div>
      </div>
    </div>

    <div v-if="store.totalCupsSold >= 48" class="bg-yellow-50 border border-yellow-200 rounded p-4">
      <p class="text-yellow-800 font-medium">⚠️ Low Water Alert: {{ store.totalCupsSold }}/50 cups sold</p>
    </div>
  </div>
</template>

<script setup lang="ts">
import { useDashboardStore } from '@/stores/dashboardStore'

const store = useDashboardStore()
</script>
