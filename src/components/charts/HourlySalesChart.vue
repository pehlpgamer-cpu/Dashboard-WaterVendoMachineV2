<template>
  <div class="bg-white p-6 rounded-lg shadow">
    <h3 class="text-lg font-semibold mb-4">Hourly Sales Distribution</h3>
    <Line :data="chartData" :options="chartOptions" />
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { Line } from 'vue-chartjs'
import { Chart as ChartJS, CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend } from 'chart.js'
import type { ChartOptions } from 'chart.js'
import { useDashboardStore } from '@/stores/dashboardStore'

ChartJS.register(CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend)

const store = useDashboardStore()

const chartData = computed(() => ({
  labels: store.hourlyDistributionData.labels,
  datasets: [
    {
      label: 'Sales per Hour',
      data: store.hourlyDistributionData.data,
      borderColor: '#10b981',
      backgroundColor: 'rgba(16, 185, 129, 0.1)',
      tension: 0.3,
      fill: true
    }
  ]
}))

const chartOptions: ChartOptions<'line'> = {
  responsive: true,
  maintainAspectRatio: true,
  plugins: {
    legend: { display: true }
  },
  scales: {
    y: { beginAtZero: true }
  }
}
</script>
