import { existsSync, readFileSync } from 'node:fs'
import { resolve } from 'node:path'

const requiredKeys = [
  'VITE_FIREBASE_API_KEY',
  'VITE_FIREBASE_AUTH_DOMAIN',
  'VITE_FIREBASE_PROJECT_ID',
  'VITE_FIREBASE_STORAGE_BUCKET',
  'VITE_FIREBASE_MESSAGING_SENDER_ID',
  'VITE_FIREBASE_APP_ID'
]

const env = {}
const envFiles = ['.env', '.env.local', '.env.production', '.env.production.local']

for (const file of envFiles) {
  const filePath = resolve(process.cwd(), file)
  if (!existsSync(filePath)) continue

  const lines = readFileSync(filePath, 'utf8').split(/\r?\n/)
  for (const line of lines) {
    const trimmed = line.trim()
    if (!trimmed || trimmed.startsWith('#')) continue

    const separatorIndex = trimmed.indexOf('=')
    if (separatorIndex === -1) continue

    const key = trimmed.slice(0, separatorIndex).trim()
    const rawValue = trimmed.slice(separatorIndex + 1).trim()
    env[key] = rawValue.replace(/^['"]|['"]$/g, '')
  }
}

Object.assign(env, process.env)

const invalidKeys = requiredKeys.filter((key) => {
  const value = env[key]
  return !value || value.startsWith('YOUR_') || value.endsWith('_HERE')
})

if (invalidKeys.length > 0) {
  console.error('Missing or placeholder Firebase environment variables:')
  for (const key of invalidKeys) {
    console.error(`- ${key}`)
  }
  console.error('Create .env.production.local with real Firebase web app values before building for Hostinger.')
  process.exit(1)
}
