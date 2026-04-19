<template>
  <div class="bg-white p-6 rounded-lg shadow mb-6">
    <div class="flex items-center justify-between mb-4">
      <h3 class="text-lg font-semibold">Export Data</h3>
      <div class="flex gap-2">
        <button
          @click="exportCSV"
          class="px-4 py-2 text-sm bg-blue-500 hover:bg-blue-600 text-white rounded transition"
        >
          📥 CSV
        </button>
        <button
          @click="exportPDF"
          class="px-4 py-2 text-sm bg-red-500 hover:bg-red-600 text-white rounded transition"
        >
          📄 PDF
        </button>
        <button
          @click="exportExcel"
          class="px-4 py-2 text-sm bg-green-500 hover:bg-green-600 text-white rounded transition"
        >
          📊 Excel
        </button>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { useDashboardStore } from '@/stores/dashboardStore'
import exportService from '@/services/exportService'

const store = useDashboardStore()

const exportCSV = () => {
  exportService.exportToCSV(
    store.filteredLogs,
    `water-sales-${new Date().toISOString().split('T')[0]}.csv`
  )
}

const exportPDF = () => {
  exportService.exportToPDF(
    store.filteredLogs,
    `water-sales-${new Date().toISOString().split('T')[0]}.pdf`
  )
}

const exportExcel = async () => {
  await exportService.exportToExcel(
    store.filteredLogs,
    `water-sales-${new Date().toISOString().split('T')[0]}.xlsx`
  )
}
</script>
