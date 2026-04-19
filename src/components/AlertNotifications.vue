<template>
  <div class="fixed bottom-6 right-6 z-50">
    <Transition
      v-for="alert in alerts"
      :key="alert.id"
      name="slide"
      class="mb-3"
    >
      <div
        :class="[
          'px-6 py-4 rounded-lg shadow-lg text-white max-w-sm',
          alert.type === 'warning' ? 'bg-yellow-500' :
          alert.type === 'danger' ? 'bg-red-500' :
          alert.type === 'info' ? 'bg-blue-500' :
          'bg-green-500'
        ]"
      >
        <div class="flex items-center justify-between">
          <p>{{ alert.message }}</p>
          <button
            @click="removeAlert(alert.id)"
            class="ml-4 text-2xl leading-none hover:opacity-75 transition"
          >
            ×
          </button>
        </div>
      </div>
    </Transition>
  </div>
</template>

<script setup lang="ts">
import { ref, watch } from 'vue'
import { useDashboardStore } from '@/stores/dashboardStore'
import alertService, { Alert } from '@/services/alertService'

const store = useDashboardStore()
const alerts = ref<Alert[]>([])

// Check for alerts whenever data changes
watch(
  () => store.totalCupsSold,
  (newCount) => {
    const lowWaterAlert = alertService.checkLowWater(newCount)
    if (lowWaterAlert) {
      alerts.value.push(lowWaterAlert)
      alertService.triggerBrowserNotification('Low Water Alert', {
        body: `Machine is running low on water: ${newCount}/50 cups sold`,
        icon: '⚠️'
      })
      setTimeout(() => removeAlert(lowWaterAlert.id), 5000)
    }
  }
)

watch(
  () => store.lastTransaction,
  (newTransaction) => {
    if (newTransaction) {
      const noActivityAlert = alertService.checkNoActivity(newTransaction.timestamp.getTime())
      // Note: we only show this if specifically triggered, not on every transaction
    }
  }
)

const removeAlert = (id: string) => {
  alerts.value = alerts.value.filter(a => a.id !== id)
}
</script>

<style scoped>
.slide-enter-active,
.slide-leave-active {
  transition: all 0.3s ease;
}

.slide-enter-from {
  transform: translateX(400px);
  opacity: 0;
}

.slide-leave-to {
  transform: translateX(400px);
  opacity: 0;
}
</style>
