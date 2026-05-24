# Water Vending Machine Dashboard

A modern, real-time Vue.js 3 dashboard for monitoring water vending machine sales, built with Vite, Firebase, Tailwind CSS, and Chart.js.

## Features

✅ **Real-time Data** - Firestore listeners sync data instantly  
✅ **6 Interactive Charts** - Sales trends, revenue, hourly distribution, comparisons  
✅ **6 KPI Cards** - Total cups, revenue, average price, cold/regular split, etc.  
✅ **Advanced Filtering** - Date range, water type, time period, hour of day  
✅ **Machine Status** - Online/offline indicator with last activity  
✅ **Transaction History** - Paginated table with sortable columns  
✅ **Data Export** - CSV and PDF formats  
✅ **Smart Alerts** - Low water warnings and inactivity notifications  
✅ **Firebase Auth** - Secure email/password authentication  
✅ **Mobile Responsive** - Touch-friendly on all devices  

## Project Setup

### 1. Install Dependencies

```bash
npm install
```

### 2. Configure Firebase

Copy `.env.example` to `.env.local` for development or `.env.production.local` before building for Hostinger:

```env
VITE_FIREBASE_API_KEY=your_real_web_api_key
VITE_FIREBASE_AUTH_DOMAIN=pauloheymann-integ-prog.firebaseapp.com
VITE_FIREBASE_PROJECT_ID=pauloheymann-integ-prog
VITE_FIREBASE_STORAGE_BUCKET=pauloheymann-integ-prog.appspot.com
VITE_FIREBASE_MESSAGING_SENDER_ID=your_real_sender_id
VITE_FIREBASE_APP_ID=your_real_app_id
```

Get these from: Firebase Console → Project Settings → General

### 3. Set Up Firestore Collection

1. In Firebase Console, go to **Firestore Database**
2. Create a new collection named `waterLogs`
3. Add sample documents with this structure:

```json
{
  "amount": 5,
  "isCold": true,
  "timestamp": 1713607200000,
  "timeSynced": true,
  "clientUptimeMs": 18400
}
```

### 4. Configure Firestore Security Rules

In Firebase Console → Firestore → Rules, paste:

```
rules_version = '2';
service cloud.firestore {
  match /databases/{database}/documents {
    // Allow read access for authenticated users
    match /waterLogs/{document=**} {
      allow read: if request.auth != null;
      allow create: if isValidTransaction(request.resource.data);
      allow update, delete: if false;
    }

    function isValidTransaction(data) {
      return data.keys().hasOnly(['amount', 'isCold', 'timestamp', 'timeSynced', 'clientUptimeMs']) &&
             data.keys().hasAll(['amount', 'isCold', 'timestamp', 'timeSynced', 'clientUptimeMs']) &&
             data.amount in [3, 5] &&
             data.isCold is bool &&
             data.timestamp is int &&
             data.timestamp > 0 &&
             data.timeSynced is bool &&
             data.clientUptimeMs is int &&
             data.clientUptimeMs >= 0;
    }
  }
}
```

### 5. Enable Firebase Authentication

1. Firebase Console → Authentication → Sign-in methods
2. Enable **Email/Password**

## Running the Dashboard

### Development Mode

```bash
npm run dev
```

Dashboard will open at: `http://localhost:5173`

### Build for Production

```bash
npm run build
```

### Preview Production Build

```bash
npm run preview
```

## Deployment

See `DEPLOYMENT.md` for the Hostinger `public_html` deployment flow.

## Architecture

```
src/
├── components/               # Vue components
│   ├── charts/              # 6 chart components
│   ├── StatisticsCards.vue  # KPI cards
│   ├── FilterPanel.vue      # Filtering UI
│   ├── MachineStatus.vue    # Status indicator
│   ├── ExportPanel.vue      # Export buttons
│   ├── TransactionHistory.vue
│   └── AlertNotifications.vue
├── services/                # Firebase & data services
│   ├── firebaseConfig.ts    # Firebase init
│   ├── firestoreService.ts  # Firestore queries
│   ├── authService.ts       # Authentication
│   ├── exportService.ts     # CSV/PDF
│   └── alertService.ts      # Alert logic
├── stores/                  # Pinia store
│   └── dashboardStore.ts    # State management
├── router/                  # Vue Router
│   └── index.ts            # Route definitions
├── views/                   # Page components
│   ├── Login.vue
│   └── Dashboard.vue
└── main.ts                  # App entry point
```

## Key Components

### StatisticsCards
- Total cups sold
- Total revenue
- Average price per transaction
- Cold vs Regular split (%)
- Sales per hour
- Last transaction details

### Charts (6 types)
1. **Sales Over Time** - Area chart (hourly/daily)
2. **Cold vs Regular** - Doughnut pie chart
3. **Revenue by Date** - Stacked bar chart
4. **Hourly Distribution** - 24-hour line chart
5. **Top 5 Hours** - Horizontal bar chart
6. **Daily/Weekly/Monthly** - Comparison bar chart

### Filters
- **Date Range** - From/To date picker
- **Water Type** - Cold / Regular / All
- **Time Period** - Today / This Week / This Month / All Time
- **Hour Range** - 0-23 hour slider

### Export Formats
- **CSV** - Excel-compatible spreadsheet
- **PDF** - Formatted report

### Real-Time Features
- Live transaction updates from ESP32
- Firestore listeners (no polling)
- Browser notifications for alerts
- Machine online/offline status
- Auto-update on new data

## Firestore Data Schema

Collection: `waterLogs`

```typescript
interface WaterLog {
  id: string              // Auto-generated
  amount: number          // 3 regular or 5 cold PHP
  isCold: boolean         // Cold water or regular
  timestamp: number       // Milliseconds since epoch
  timeSynced?: boolean    // false = approximate flush-time timestamp
  clientUptimeMs?: number // ESP32 uptime when sale queued
}
```

## Customization

### Change Colors

Edit `tailwind.config.ts`:

```typescript
theme: {
  extend: {
    colors: {
      primary: '#0066ff',
      secondary: '#6c757d',
      // ... add your colors
    }
  }
}
```

### Adjust Statistics

Edit `src/stores/dashboardStore.ts` to modify calculations.

### Change Alert Thresholds

Edit `src/services/alertService.ts`:

```typescript
checkLowWater(cupsSold: number): Alert | null {
  if (cupsSold > 48) { // Change 48 to your threshold
    // ...
  }
}
```

## Troubleshooting

### No data showing?
1. Check Firebase credentials in `.env.local` or `.env.production.local`
2. Verify Firestore collection exists and has documents
3. Check browser console for errors (F12)

### Authentication failing?
1. Ensure Email/Password auth is enabled in Firebase
2. Check Firestore security rules

### Charts not displaying?
1. Verify data is being loaded (check store)
2. Check for console errors

### Real-time updates not working?
1. Check Firestore listener in `Dashboard.vue`
2. Verify database rules allow read access

## API Reference

### Dashboard Store (Pinia)

```typescript
// State
store.logs                    // All logs from Firestore
store.filteredLogs           // Logs after applying filters
store.selectedWaterType      // 'all' | 'cold' | 'regular'
store.selectedTimeRange      // 'today' | 'week' | 'month' | 'all'

// Computed (Auto-updated)
store.totalCupsSold          // Number
store.totalRevenue           // Number
store.averagePrice           // String (PHP X)
store.coldVsRegularSplit     // { cold, regular, coldPercent, regularPercent }
store.salesPerHour           // String
store.lastTransaction        // { timestamp, isCold, amount }
store.machineStatus          // { status: 'online'|'offline', message }

// Chart Data
store.salesOverTimeData      // { labels, data }
store.coldVsRegularData      // { labels, data, colors }
store.revenueByDateData      // { labels, coldData, regularData }
store.hourlyDistributionData // { labels, data }
store.topHoursData           // { labels, data }
store.comparisonData         // { labels, data }

// Actions
store.setLogs(logs)
store.setDateRange(start, end)
store.setTimeRange(range)
store.setWaterType(type)
store.setHourRange(start, end)
store.resetFilters()
```

### Firebase Services

```typescript
// Auth Service
authService.login(email, password)
authService.logout()
authService.getCurrentUser()

// Firestore Service
firestoreService.subscribeToWaterLogs(callback, startDate?, endDate?)
firestoreService.fetchWaterLogs(startDate?, endDate?)
firestoreService.unsubscribeAll()

// Export Service
exportService.exportToCSV(logs, filename)
exportService.exportToPDF(logs, filename)

// Alert Service
alertService.checkLowWater(cupsSold)
alertService.checkNoActivity(lastTransactionTime)
alertService.triggerBrowserNotification(title, options)
```

## Performance Tips

1. **Lazy load charts** - Only render visible charts
2. **Limit data window** - Default to last 7 days, not all-time
3. **Use pagination** - Transaction table limits to 10 rows
4. **Index Firestore** - Add composite indexes for date range queries

## License

MIT

## Support

For issues or questions:
1. Check the troubleshooting section above
2. Review Firebase Console logs
3. Check browser console (F12) for errors
4. Verify Firestore collection and security rules

---

**Built with ❤️ for water vending machine monitoring**
