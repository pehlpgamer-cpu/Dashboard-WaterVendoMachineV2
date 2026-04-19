<template>
  <div class="bg-white p-6 rounded-lg shadow">
    <h3 class="text-lg font-semibold mb-4">Recent Transactions</h3>

    <div class="overflow-x-auto">
      <table class="w-full text-sm">
        <thead>
          <tr class="border-b">
            <th class="text-left py-3 px-4 font-medium">Timestamp</th>
            <th class="text-left py-3 px-4 font-medium">Type</th>
            <th class="text-left py-3 px-4 font-medium">Amount</th>
            <th class="text-left py-3 px-4 font-medium">Date</th>
          </tr>
        </thead>
        <tbody>
          <tr v-if="paginatedLogs.length === 0" class="border-b hover:bg-gray-50">
            <td colspan="4" class="py-4 px-4 text-center text-gray-500">No transactions found</td>
          </tr>
          <tr v-for="log in paginatedLogs" :key="log.id" class="border-b hover:bg-gray-50 transition">
            <td class="py-3 px-4">{{ formatTime(log.timestamp) }}</td>
            <td class="py-3 px-4">
              <span :class="[
                'px-2 py-1 rounded text-xs font-medium',
                log.isCold ? 'bg-blue-100 text-blue-800' : 'bg-orange-100 text-orange-800'
              ]">
                {{ log.isCold ? 'Cold' : 'Regular' }}
              </span>
            </td>
            <td class="py-3 px-4 font-semibold">${{ log.amount }}</td>
            <td class="py-3 px-4">{{ formatDate(log.timestamp) }}</td>
          </tr>
        </tbody>
      </table>
    </div>

    <!-- Pagination -->
    <div v-if="totalPages > 1" class="mt-6 flex items-center justify-center gap-2">
      <button
        @click="currentPage = Math.max(1, currentPage - 1)"
        :disabled="currentPage === 1"
        class="px-3 py-2 border rounded disabled:opacity-50 hover:bg-gray-100 transition"
      >
        Previous
      </button>
      <span class="text-sm text-gray-600">
        Page {{ currentPage }} of {{ totalPages }}
      </span>
      <button
        @click="currentPage = Math.min(totalPages, currentPage + 1)"
        :disabled="currentPage === totalPages"
        class="px-3 py-2 border rounded disabled:opacity-50 hover:bg-gray-100 transition"
      >
        Next
      </button>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue'
import { useDashboardStore } from '@/stores/dashboardStore'
import { format } from 'date-fns'

const store = useDashboardStore()
const currentPage = ref(1)
const itemsPerPage = 10

const totalPages = computed(() => Math.ceil(store.transactionHistory.length / itemsPerPage))

const paginatedLogs = computed(() => {
  const start = (currentPage.value - 1) * itemsPerPage
  return store.transactionHistory.slice(start, start + itemsPerPage)
})

const formatTime = (date: Date) => format(date, 'HH:mm:ss')
const formatDate = (date: Date) => format(date, 'yyyy-MM-dd')
</script>
