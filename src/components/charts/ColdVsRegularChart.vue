<template>
  <div class="bg-white p-6 rounded-lg shadow">
    <h3 class="text-lg font-semibold mb-4">Cold vs Regular Water</h3>
    <Doughnut :data="chartData" :options="chartOptions" />
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { Doughnut } from 'vue-chartjs'
import { Chart as ChartJS, ArcElement, Tooltip, Legend } from 'chart.js'
import { useDashboardStore } from '@/stores/dashboardStore'

ChartJS.register(ArcElement, Tooltip, Legend)

const store = useDashboardStore()

const chartData = computed(() => ({
  labels: store.coldVsRegularData.labels,
  datasets: [
    {
      data: store.coldVsRegularData.data,
      backgroundColor: ['#3b82f6', '#f97316'],
      borderColor: ['#1e40af', '#ea580c']
    }
  ]
}))

const chartOptions = {
  responsive: true,
  maintainAspectRatio: true,
  plugins: {
    legend: { position: 'bottom' }
  }
}
</script>
