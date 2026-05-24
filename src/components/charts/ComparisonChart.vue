<template>
  <div class="bg-white p-6 rounded-lg shadow">
    <h3 class="text-lg font-semibold mb-4">Daily/Weekly/Monthly Comparison</h3>
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
  labels: store.comparisonData.labels,
  datasets: [
    {
      label: 'Cups Sold',
      data: store.comparisonData.data,
      backgroundColor: ['#6366f1', '#8b5cf6', '#ec4899']
    }
  ]
}))

const chartOptions: ChartOptions<'bar'> = {
  responsive: true,
  maintainAspectRatio: true,
  plugins: {
    legend: { display: false }
  },
  scales: {
    y: { beginAtZero: true }
  }
}
</script>
