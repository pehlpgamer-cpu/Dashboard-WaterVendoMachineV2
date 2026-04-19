# Quick Start Guide - Water Vending Machine Dashboard

## 🎯 5-Minute Setup

### Step 1: Install (1 min)
```bash
npm install
```

### Step 2: Configure Firebase (2 min)
1. Edit `.env.local` with your Firebase credentials
2. Get credentials from: [Firebase Console](https://console.firebase.google.com)

### Step 3: Set Up Firestore (1 min)
1. Create collection: `waterLogs`
2. Add security rules (see `FIRESTORE_SETUP.md`)

### Step 4: Run (1 min)
```bash
npm run dev
```
Dashboard opens at: `http://localhost:5173`

---

## 📊 Dashboard Overview

### Real-Time Updates
- Data syncs instantly from Firestore
- 6 interactive charts update automatically
- Machine status shows online/offline

### Filtering
- Date range picker
- Water type selector
- Hour of day range
- Quick filters (Today/Week/Month)

### Exports
- Download as CSV, PDF, or Excel
- All filters respected in exports

### Alerts
- Low water warning (>48 cups)
- Inactivity notifications (>4 hours)
- Browser notifications supported

---

## 🔧 ESP32 Integration

Your Arduino code already logs to Firestore:

```cpp
// Set your Firebase credentials
#define FIREBASE_API_KEY "YOUR_API_KEY"
#define FIREBASE_PROJECT_ID "pauloheymann-integ-prog"
```

Data automatically appears in dashboard!

---

## 📁 Project Structure

```
dashboard/
├── src/
│   ├── components/      # Vue components (charts, stats, filters)
│   ├── services/        # Firebase & data services
│   ├── stores/          # Pinia state management
│   ├── router/          # Vue Router config
│   ├── views/           # Page components (Login, Dashboard)
│   ├── App.vue          # Root component
│   └── main.ts          # Entry point
├── public/              # Static assets
├── index.html           # HTML template
├── package.json         # Dependencies
├── vite.config.ts       # Build config
├── tailwind.config.ts   # Styling config
└── README.md            # Full documentation
```

---

## 🚀 Deployment

### Vercel (Recommended - 2 min)
```bash
npm install -g vercel
vercel
```

### Netlify (2 min)
```bash
npm run build
# Drag 'dist' folder to netlify.com
```

### Firebase Hosting (3 min)
```bash
npm install -g firebase-tools
firebase init
firebase deploy
```

---

## 🔑 Default Login

To test the dashboard:

1. Sign up with any email/password (first time)
2. Use same credentials to log in

Example:
- Email: `test@example.com`
- Password: `password123`

---

## 📊 Sample Data

To test with sample data, add these documents to Firestore `waterLogs` collection:

```json
{
  "amount": 5,
  "isCold": true,
  "timestamp": 1713607200000
}
```

```json
{
  "amount": 3,
  "isCold": false,
  "timestamp": 1713607260000
}
```

Timestamp is milliseconds since epoch. Use `Date.now()` in browser console.

---

## 🐛 Troubleshooting

### Dashboard shows no data?
1. Check `.env.local` has correct Firebase credentials
2. Verify Firestore collection `waterLogs` exists
3. Check browser console (F12) for errors

### Login not working?
1. Ensure Email/Password auth is enabled in Firebase
2. Check Firebase security rules

### Charts are empty?
1. Add sample data to Firestore
2. Check Pinia store updates (Vue DevTools)

### Real-time updates not working?
1. Verify database rules allow reads
2. Check browser console for listener errors

---

## 💡 Tips

- **Responsive**: Works on mobile, tablet, desktop
- **Lightweight**: No unnecessary scripts
- **Fast**: Real-time Firestore listeners
- **Secure**: Firebase Auth + security rules

---

## 📚 Learn More

- [Vue.js 3 Docs](https://vuejs.org/)
- [Vite Guide](https://vitejs.dev/)
- [Firebase Docs](https://firebase.google.com/docs)
- [Chart.js Examples](https://www.chartjs.org/docs/latest/)
- [Tailwind CSS](https://tailwindcss.com/)

---

## 🆘 Need Help?

1. Check `README.md` for detailed docs
2. See `DEPLOYMENT.md` for deploy issues
3. Review `FIRESTORE_SETUP.md` for database setup
4. Check browser console for errors (F12)

---

**Happy monitoring! 🎉**
