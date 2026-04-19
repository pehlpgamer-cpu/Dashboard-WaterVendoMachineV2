# Win Setup Script for Windows PowerShell

Write-Host "🚀 Setting up Water Vending Machine Dashboard..." -ForegroundColor Green

# Check if Node.js is installed
if (!(Get-Command node -ErrorAction SilentlyContinue)) {
    Write-Host "❌ Node.js is not installed. Please install Node.js v18 or higher." -ForegroundColor Red
    exit 1
}

Write-Host "✅ Node.js version: $(node --version)" -ForegroundColor Green

# Install dependencies
Write-Host ""
Write-Host "📦 Installing dependencies..." -ForegroundColor Yellow
npm install

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ npm install failed" -ForegroundColor Red
    exit 1
}

Write-Host "✅ Dependencies installed" -ForegroundColor Green

# Create .env.local if it doesn't exist
if (!(Test-Path ".env.local")) {
    Write-Host ""
    Write-Host "📝 Creating .env.local (update with your Firebase credentials)" -ForegroundColor Yellow
    
    $envContent = @"
# Firebase Config - Get these from Firebase Console > Project Settings
VITE_FIREBASE_API_KEY=YOUR_API_KEY
VITE_FIREBASE_AUTH_DOMAIN=pauloheymann-integ-prog.firebaseapp.com
VITE_FIREBASE_PROJECT_ID=pauloheymann-integ-prog
VITE_FIREBASE_STORAGE_BUCKET=pauloheymann-integ-prog.appspot.com
VITE_FIREBASE_MESSAGING_SENDER_ID=YOUR_SENDER_ID
VITE_FIREBASE_APP_ID=YOUR_APP_ID
"@
    
    $envContent | Out-File -FilePath ".env.local" -Encoding UTF8
    Write-Host "✅ Created .env.local" -ForegroundColor Green
    Write-Host "⚠️  Please update .env.local with your Firebase credentials" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "✅ Setup complete!" -ForegroundColor Green
Write-Host ""
Write-Host "📚 Next steps:" -ForegroundColor Cyan
Write-Host "1. Update .env.local with your Firebase credentials"
Write-Host "2. Update src/services/firebaseConfig.ts to use environment variables"
Write-Host "3. Run: npm run dev"
Write-Host ""
Write-Host "📖 For more info, see README.md and DEPLOYMENT.md" -ForegroundColor Cyan
