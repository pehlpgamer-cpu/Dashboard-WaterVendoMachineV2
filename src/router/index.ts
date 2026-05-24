import { createRouter, createWebHistory } from 'vue-router'
import type { RouteRecordRaw } from 'vue-router'
import Login from '@/views/Login.vue'
import Dashboard from '@/views/Dashboard.vue'
import authService from '@/services/authService'
import { isMockDataEnabled } from '@/utils/mockData'

const routes: RouteRecordRaw[] = [
  {
    path: '/',
    redirect: '/dashboard'
  },
  {
    path: '/login',
    name: 'Login',
    component: Login,
    meta: { requiresAuth: false }
  },
  {
    path: '/dashboard',
    name: 'Dashboard',
    component: Dashboard,
    meta: { requiresAuth: true }
  }
]

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes
})

router.beforeEach(async (to) => {
  const user = isMockDataEnabled() ? { uid: 'mock-user' } : await authService.getAuthState()

  if (to.meta.requiresAuth && !user) {
    return '/login'
  }

  if (to.path === '/login' && user) {
    return '/dashboard'
  }

  return true
})

export default router
