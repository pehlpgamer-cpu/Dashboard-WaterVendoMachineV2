<template>
  <div class="min-h-screen bg-gradient-to-br from-blue-500 to-purple-600 flex items-center justify-center p-4">
    <div class="bg-white rounded-lg shadow-2xl p-8 w-full max-w-md">
      <h1 class="text-3xl font-bold mb-2 text-center text-gray-800">Water Vending Dashboard</h1>
      <p class="text-gray-600 text-center mb-8">Sign in to your account</p>

      <div v-if="error" class="mb-4 p-4 bg-red-100 border border-red-400 text-red-700 rounded">
        {{ error }}
      </div>

      <form @submit.prevent="handleLogin" class="space-y-6">
        <div>
          <label class="block text-sm font-medium text-gray-700 mb-2">Email</label>
          <input
            v-model="email"
            type="email"
            required
            class="w-full px-4 py-2 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-blue-500"
            placeholder="you@example.com"
          />
        </div>

        <div>
          <label class="block text-sm font-medium text-gray-700 mb-2">Password</label>
          <input
            v-model="password"
            type="password"
            required
            class="w-full px-4 py-2 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-blue-500"
            placeholder="••••••••"
          />
        </div>

        <button
          type="submit"
          :disabled="loading"
          class="w-full bg-blue-500 hover:bg-blue-600 disabled:bg-gray-400 text-white font-semibold py-2 rounded-lg transition"
        >
          {{ loading ? 'Signing in...' : 'Sign In' }}
        </button>
      </form>

      <div class="mt-6 text-center">
        <p class="text-gray-600 text-sm">Don't have an account?
          <button
            @click="isSignup = !isSignup"
            class="text-blue-500 hover:text-blue-600 font-medium"
          >
            {{ isSignup ? 'Sign In' : 'Sign Up' }}
          </button>
        </p>
      </div>

      <div v-if="isSignup" class="mt-6 p-4 bg-blue-50 rounded border border-blue-200">
        <p class="text-sm text-blue-800">
          <strong>Demo:</strong> You can sign up with any email/password combination for testing purposes.
        </p>
      </div>

      <!-- Dev Mode Toggle -->
      <div class="mt-6 pt-6 border-t border-gray-300">
        <label class="flex items-center cursor-pointer">
          <input
            v-model="devMode"
            type="checkbox"
            @change="toggleDevMode"
            class="w-4 h-4 text-blue-500"
          />
          <span class="ml-2 text-sm text-gray-700">Enable Dev Mode (Mock Data)</span>
        </label>
        <p v-if="devMode" class="mt-2 text-xs text-amber-600 bg-amber-50 p-2 rounded">
          ✓ Dev mode active. Click "Sign In" to access dashboard with mock data.
        </p>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { useRouter } from 'vue-router'
import authService from '@/services/authService'
import { isDevModeEnabled, setDevMode } from '@/utils/devMode'

const router = useRouter()
const email = ref('')
const password = ref('')
const loading = ref(false)
const error = ref('')
const isSignup = ref(false)
const devMode = ref(isDevModeEnabled())

onMounted(() => {
  // Auto-login in devMode if already enabled
  if (isDevModeEnabled()) {
    devMode.value = true
  }
})

const toggleDevMode = () => {
  if (devMode.value) {
    localStorage.setItem('devMode', 'true')
  } else {
    localStorage.removeItem('devMode')
  }
}

const handleLogin = async () => {
  try {
    loading.value = true
    error.value = ''

    if (devMode.value) {
      // In dev mode, skip auth and go directly to dashboard
      router.push('/dashboard')
      return
    }

    if (isSignup.value) {
      await authService.signup(email.value, password.value)
    } else {
      await authService.login(email.value, password.value)
    }

    router.push('/dashboard')
  } catch (err: any) {
    error.value = err.message || 'Authentication failed. Please try again.'
  } finally {
    loading.value = false
  }
}
</script>
