<template>
  <div class="bg-white p-6 rounded-lg shadow">
    <h3 class="text-lg font-semibold mb-4">Sales Over Time</h3>
    <Line :data="chartData" :options="chartOptions" />
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { Line } from 'vue-chartjs'
import { Chart as ChartJS, CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend } from 'chart.js'
import { useDashboardStore } from '@/stores/dashboardStore'

ChartJS.register(CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend)

const store = useDashboardStore()

const chartData = computed(() => ({
  labels: store.salesOverTimeData.labels,
  datasets: [
    {
      label: 'Cups Sold',
      data: store.salesOverTimeData.data,
      borderColor: '#0066ff',
      backgroundColor: 'rgba(0, 102, 255, 0.1)',
      tension: 0.3,
      fill: true
    }
  ]
}))

const chartOptions = {
  responsive: true,
  maintainAspectRatio: true,
  plugins: {
    legend: { display: true, position: 'top' }
  },
  scales: {
    y: { beginAtZero: true }
  }
}
</script>
