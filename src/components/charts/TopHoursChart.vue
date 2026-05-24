<template>
  <div class="bg-white p-6 rounded-lg shadow">
    <h3 class="text-lg font-semibold mb-4">Top 5 Sales Hours</h3>
    <Bar :data="chartData" :options="chartOptions" />
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { Bar } from 'vue-chartjs'
import { Chart as ChartJS, CategoryScale, LinearScale, BarElement, Title, Tooltip, Legend } from 'chart.js'
import type { ChartOptions } from 'chart.js'
import { useDashboardStore } from '@/stores/dashboardStore'

ChartJS.register(CategoryScale, LinearScale, BarElement, Title, Tooltip, Legend)

const store = useDashboardStore()

const chartData = computed(() => ({
  labels: store.topHoursData.labels,
  datasets: [
    {
      label: 'Cups Sold',
      data: store.topHoursData.data,
      backgroundColor: '#f59e0b'
    }
  ]
}))

const chartOptions: ChartOptions<'bar'> = {
  indexAxis: 'y' as const,
  responsive: true,
  maintainAspectRatio: true,
  plugins: {
    legend: { display: false }
  },
  scales: {
    x: { beginAtZero: true }
  }
}
</script>
