#!/bin/bash

# Setup script for Water Vending Dashboard

echo "🚀 Setting up Water Vending Machine Dashboard..."

# Check if Node.js is installed
if ! command -v node &> /dev/null; then
    echo "❌ Node.js is not installed. Please install Node.js v18 or higher."
    exit 1
fi

echo "✅ Node.js version: $(node --version)"

# Install dependencies
echo ""
echo "📦 Installing dependencies..."
npm install

if [ $? -ne 0 ]; then
    echo "❌ npm install failed"
    exit 1
fi

echo "✅ Dependencies installed"

# Create .env.local if it doesn't exist
if [ ! -f ".env.local" ]; then
    echo ""
    echo "📝 Creating .env.local (update with your Firebase credentials)"
    cat > .env.local << EOF
# Firebase Config - Get these from Firebase Console > Project Settings
VITE_FIREBASE_API_KEY=YOUR_API_KEY
VITE_FIREBASE_AUTH_DOMAIN=pauloheymann-integ-prog.firebaseapp.com
VITE_FIREBASE_PROJECT_ID=pauloheymann-integ-prog
VITE_FIREBASE_STORAGE_BUCKET=pauloheymann-integ-prog.appspot.com
VITE_FIREBASE_MESSAGING_SENDER_ID=YOUR_SENDER_ID
VITE_FIREBASE_APP_ID=YOUR_APP_ID
EOF
    echo "✅ Created .env.local"
    echo "⚠️  Please update .env.local with your Firebase credentials"
fi

echo ""
echo "✅ Setup complete!"
echo ""
echo "📚 Next steps:"
echo "1. Update .env.local with your Firebase credentials"
echo "2. Update src/services/firebaseConfig.ts to use environment variables"
echo "3. Run: npm run dev"
echo ""
echo "📖 For more info, see README.md and DEPLOYMENT.md"
