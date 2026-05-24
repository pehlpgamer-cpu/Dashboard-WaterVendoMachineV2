<template>
  <div class="bg-white p-6 rounded-lg shadow">
    <h3 class="text-lg font-semibold mb-4">Revenue by Date</h3>
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
  labels: store.revenueByDateData.labels,
  datasets: [
    {
      label: 'Cold Water Revenue',
      data: store.revenueByDateData.coldData,
      backgroundColor: '#3b82f6'
    },
    {
      label: 'Regular Water Revenue',
      data: store.revenueByDateData.regularData,
      backgroundColor: '#f97316'
    }
  ]
}))

const chartOptions: ChartOptions<'bar'> = {
  responsive: true,
  maintainAspectRatio: true,
  indexAxis: 'x' as const,
  plugins: {
    legend: { position: 'top' }
  },
  scales: {
    y: { beginAtZero: true, stacked: false }
  }
}
</script>
