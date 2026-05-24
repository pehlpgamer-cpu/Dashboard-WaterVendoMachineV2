# Complete Setup Instructions

## 🎯 Full Setup Guide - Water Vending Machine Dashboard

This guide will walk you through every step to get the dashboard running.

---

## Prerequisites

Before starting, ensure you have:
- **Node.js v20.19+ or v22.12+** ([Download](https://nodejs.org))
- **npm or yarn** (comes with Node.js)
- **Firebase Account** (free) - ([Sign up](https://firebase.google.com))
- **GitHub Account** (optional, for deployment)
- **Text Editor** (VS Code recommended)

---

## Part 1: Firebase Setup (10 minutes)

### 1.1 Create Firebase Project

1. Go to [Firebase Console](https://console.firebase.google.com)
2. Click "Add Project"
3. Enter project name: `pauloheymann-integ-prog` (matching ESP32 code)
4. Accept terms and create

### 1.2 Get Firebase Credentials

1. In Firebase Console, click the gear icon → **Project Settings**
2. Scroll to "Your apps" section
3. Click "Add app" → **Web**
4. Register with nickname "Dashboard"
5. Copy the configuration values:
   ```javascript
   apiKey: "xxx"
   authDomain: "xxx.firebaseapp.com"
   projectId: "pauloheymann-integ-prog"
   storageBucket: "xxx.appspot.com"
   messagingSenderId: "xxx"
   appId: "xxx"
   ```
6. Keep these safe - you'll need them soon

### 1.3 Enable Authentication

1. In Firebase Console, go to **Authentication** (left sidebar)
2. Click **Sign-in method**
3. Click **Email/Password** → Enable it
4. Save

### 1.4 Create Firestore Database

1. In Firebase Console, go to **Firestore Database**
2. Click **Create database**
3. Choose **Start in test mode** (for development)
4. Select region (closest to you)
5. Click **Create**

### 1.5 Create `waterLogs` Collection

1. In Firestore, click **Start collection**
2. Collection ID: `waterLogs`
3. Click **Next**
4. Skip adding document (we'll use it from ESP32)
5. Click **Save**

### 1.6 Set Security Rules

1. In Firestore, go to **Rules** tab
2. Copy and paste the rules from `FIRESTORE_SETUP.md`
3. Click **Publish**

---

## Part 2: Project Setup (5 minutes)

### 2.1 Clone/Download Project

```bash
# Option 1: Clone from Git
git clone https://github.com/YOUR_REPO/dashboard.git
cd dashboard

# Option 2: Or use the existing folder
cd Dashboard-WaterVendoMachineV2
```

### 2.2 Install Dependencies

```bash
npm install
```

This installs all required packages (~100 packages, ~300MB).

### 2.3 Configure Environment

1. Create `.env.local` file in project root:
   ```bash
   cp .env.example .env.local
   ```

2. Edit `.env.local` and paste your Firebase credentials:
   ```env
   VITE_FIREBASE_API_KEY=YOUR_API_KEY_HERE
   VITE_FIREBASE_AUTH_DOMAIN=pauloheymann-integ-prog.firebaseapp.com
   VITE_FIREBASE_PROJECT_ID=pauloheymann-integ-prog
   VITE_FIREBASE_STORAGE_BUCKET=YOUR_BUCKET.appspot.com
   VITE_FIREBASE_MESSAGING_SENDER_ID=YOUR_SENDER_ID
   VITE_FIREBASE_APP_ID=YOUR_APP_ID
   ```

3. Do not edit `src/services/firebaseConfig.ts`; it reads these variables automatically.

---

## Part 3: Run Dashboard (2 minutes)

### 3.1 Start Development Server

```bash
npm run dev
```

You should see:
```
VITE v5.0.0  ready in XXX ms

➜  Local:   http://localhost:5173/
➜  press h + enter to show help
```

### 3.2 Open in Browser

Click the link or open: **http://localhost:5173**

The dashboard should open and redirect to `/login`

---

## Part 4: Test Authentication (3 minutes)

### 4.1 Create User

1. In Firebase Console, open Authentication -> Users.
2. Add a dashboard user manually.
3. Use that email/password on the login page.
4. You should be redirected to dashboard.

### 4.2 Add Sample Data (to see charts)

1. Open browser console (F12)
2. Paste and run:
   ```javascript
   // Generate sample transactions
   const firestore = firebase.firestore();
   const now = Date.now();
   
   for (let i = 1; i <= 20; i++) {
     const isCold = Math.random() > 0.5;
     firestore.collection('waterLogs').add({
       amount: isCold ? 5 : 3,
       isCold,
       timestamp: now - (i * 3600000), // Each transaction 1 hour apart
       timeSynced: true,
       clientUptimeMs: i * 1000
     });
   }
   console.log('Sample data added!');
   ```

3. Close console (F12)
4. Dashboard should now show data in charts

### 4.3 Test Features

- ✅ View statistics cards updating
- ✅ Interact with charts (click legend items)
- ✅ Change filters (date range, water type, etc.)
- ✅ Export as CSV/PDF
- ✅ View transaction history table
- ✅ Check machine status (should show green "Online")

---

## Part 5: Connect ESP32 (5 minutes)

### 5.1 Update ESP32 Code

Copy `esp32-water-vendo-code/secrets.example.h` to ignored local `esp32-water-vendo-code/secrets.h`, then set:

```cpp
#define WIFI_ENABLED true
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_PROJECT_ID "pauloheymann-integ-prog"
#define FIREBASE_API_KEY "YOUR_FIREBASE_WEB_API_KEY"
```

Use the same Firebase web API key from step 1.2.

### 5.2 Upload to ESP32

1. Upload your fixed Arduino code to ESP32
2. ESP32 should start logging transactions to Firestore
3. Watch dashboard in real-time as transactions appear!

---

## Part 6: Deploy To Hostinger

1. Create `.env.production.local` with real Firebase web app values.
2. Build:
   ```bash
   npm run build
   ```
3. Upload the contents of `dist` to Hostinger `public_html`.
4. Confirm `index.html`, `assets/`, and `.htaccess` are directly inside `public_html`.

See `DEPLOYMENT.md` for the full Hostinger checklist.

---

## Troubleshooting

### "npm: command not found"
- Node.js not installed - [Download](https://nodejs.org)
- Restart terminal/computer after install

### "Firebase config missing"
- Copy `.env.example` to `.env.local`
- Fill in real Firebase credentials
- Restart dev server (`npm run dev`)

### Dashboard blank / no data
1. Check browser console (F12) for errors
2. Verify Firebase credentials are correct
3. Check Firestore has `waterLogs` collection
4. Try adding sample data (see Part 4.2)

### Charts showing but empty
- Add sample data first (Part 4.2)
- Verify date range filter includes sample data
- Check browser console for errors

### Export not working
- Check pop-up blocker isn't blocking download
- Try different browser
- Check console for errors

### Real-time updates not working
1. Check Firestore security rules are correct
2. Verify user is authenticated
3. Check browser console for listener errors
4. Try refreshing page

### Authentication failing
1. Ensure Email/Password is enabled in Firebase
2. Try a different email address
3. Check Firebase console for errors

---

## Project Structure Overview

```
Dashboard-WaterVendoMachineV2/
├── src/
│   ├── components/          # Vue components
│   ├── services/            # Firebase services
│   ├── stores/              # Pinia store
│   ├── router/              # Vue Router
│   ├── views/               # Login & Dashboard pages
│   ├── App.vue              # Root component
│   └── main.ts              # Entry point
├── public/                  # Static files
├── index.html               # HTML template
├── package.json             # Dependencies
├── vite.config.ts           # Vite config
├── tailwind.config.ts       # Tailwind config
├── .env.example             # Environment template
├── README.md                # Full documentation
├── QUICKSTART.md            # 5-minute guide
├── DEPLOYMENT.md            # Deployment guide
├── FIRESTORE_SETUP.md       # Firebase setup
└── PROJECT_MANIFEST.md      # File reference
```

---

## Common Tasks

### Change Chart Colors
Edit `tailwind.config.ts`:
```typescript
theme: {
  extend: {
    colors: {
      primary: '#YOUR_COLOR',
      // ...
    }
  }
}
```

### Add New Filter
1. Add state to`dashboardStore.ts`
2. Add computed filter logic to `filteredLogs`
3. Add UI input to `FilterPanel.vue`
4. Charts auto-update!

### Change Alert Thresholds
Edit `src/services/alertService.ts`:
```typescript
if (cupsSold > 48) { // Change to your threshold
```

### Customize Dashboard Layout
Edit `Dashboard.vue`:
- Rearrange component order
- Change grid layouts
- Add/remove sections

---

## Performance Tips

1. **Limit data window** - Default 7 days (edit store for all-time)
2. **Use date filters** - Faster queries
3. **Pagination** - Transaction table limited to 10/page
4. **Lazy loading** - Components load on demand

---

## Security Best Practices

1. ✅ **Never commit `.env.local`** - It's in `.gitignore`
2. ✅ **Use security rules** - Restrict to authenticated users
3. ✅ **Limit read/write** - Write only from ESP32/service account
4. ✅ **Update API keys** - Rotate if exposed
5. ✅ **Monitor usage** - Check Firestore costs

---

## Next Steps

1. **Customize styling** - Change colors in `tailwind.config.ts`
2. **Add team members** - Invite to Firebase project
3. **Set up monitoring** - Enable Firebase analytics
4. **Configure backups** - Export data regularly
5. **Document API calls** - Update ESP32 integration notes

---

## Resources

- [Vue.js 3 Docs](https://vuejs.org/) - Framework guide
- [Firebase Docs](https://firebase.google.com/docs) - Backend setup
- [Vite Guide](https://vitejs.dev/) - Build tool
- [Tailwind CSS](https://tailwindcss.com/) - Styling
- [Chart.js](https://www.chartjs.org/) - Charts

---

## Support

### Getting Help

1. **Check error message** - Usually explains the problem
2. **Check browser console** - F12 → Console tab
3. **Read relevant guide**:
   - Setup issues → FIRESTORE_SETUP.md
   - Deployment issues → DEPLOYMENT.md
   - Quick questions → QUICKSTART.md

### Debugging

Enable verbose logging:
```typescript
// In Dashboard.vue
console.log('Store logs:', store.logs)
console.log('Filtered logs:', store.filteredLogs)
console.log('Chart data:', store.salesOverTimeData)
```

---

## Celebrate! 🎉

Your dashboard is now running! 

**Next**: Connect your ESP32 and watch data flow in real-time.

Questions? See `README.md` for comprehensive documentation.

---

## Quick Reference: Command Cheatsheet

```bash
# Development
npm run dev                 # Start dev server
npm run build              # Build for production
npm run preview            # Preview production build

# Installation
npm install                # Install dependencies
npm install package-name   # Add new package

# Deployment
npm run build              # Build static files for Hostinger

# Debugging
npm run build              # Check for build errors
```

---

**Happy monitoring! 🚀**
