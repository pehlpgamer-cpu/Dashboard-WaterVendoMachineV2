# Project File Structure & Reference

## Complete File Manifest

### Root Configuration Files
```
├── package.json                      # Dependencies & scripts
├── vite.config.ts                    # Vite build configuration
├── tailwind.config.ts                # Tailwind CSS configuration
├── postcss.config.js                 # PostCSS configuration
├── tsconfig.json                     # TypeScript configuration
├── tsconfig.node.json                # TypeScript Node configuration
├── index.html                        # HTML entry point
├── .gitignore                        # Git ignore rules
├── .env.example                      # Environment template
```

### Documentation Files
```
├── README.md                         # Full project documentation
├── QUICKSTART.md                     # 5-minute quick start guide
├── DEPLOYMENT.md                     # Deployment & CI/CD setup
├── FIRESTORE_SETUP.md                # Firebase security rules & indexes
├── PROJECT_MANIFEST.md               # This file
```

### Setup Scripts
```
├── setup.sh                          # Linux/Mac setup script
├── setup.ps1                         # Windows PowerShell setup script
```

### Source Code Structure

#### Root Source Files
```
src/
├── main.ts                           # Application entry point
├── App.vue                           # Root Vue component
├── style.css                         # Global styles
```

#### Services (src/services/)
Firebase integration, authentication, data export, and alerts.

```
src/services/
├── firebaseConfig.ts                 # Firebase initialization
│   - Uses environment variables for credentials
│   - Exports auth and db instances
│   - Initializes Firebase app
│
├── authService.ts                    # Authentication service
│   - login(email, password): User login
│   - logout(): User logout
│   - onAuthStateChanged(callback): Auth state listener
│   - getCurrentUser(): Get current authenticated user
│   - getAuthState(): Promise-based auth state
│
├── firestoreService.ts               # Firestore data access
│   - subscribeToWaterLogs(callback, dateRange): Real-time listener
│   - fetchWaterLogs(dateRange): One-time fetch
│   - unsubscribeAll(): Cleanup all listeners
│   - Returns WaterLog interface with id, amount, isCold, timestamp
│
├── exportService.ts                  # Data export functionality
│   - exportToCSV(logs, filename): Download as CSV
│   - exportToPDF(logs, filename): Download as PDF
│
└── alertService.ts                   # Alert management
    - checkLowWater(cupsSold): Alert if > 48 cups
    - checkNoActivity(lastTime): Alert if > 4 hours inactive
    - triggerBrowserNotification(title, options): Browser alert
    - requestNotificationPermission(): Request user permission
```

#### State Management (src/stores/)
Pinia store handling state, filtering, and computed data.

```
src/stores/
└── dashboardStore.ts                 # Main dashboard store
    
    State:
    - logs: WaterLog[]
    - startDate, endDate: Date range filters
    - selectedWaterType: 'all' | 'cold' | 'regular'
    - selectedTimeRange: 'today' | 'week' | 'month' | 'all'
    - selectedHourRange: { start, end }
    - filteredLogs: Computed filtered logs
    
    Statistics (Computed):
    - totalCupsSold: Number
    - totalRevenue: Number
    - averagePrice: String
    - coldVsRegularSplit: Percentages
    - salesPerHour: String
    - lastTransaction: Object | null
    
    Chart Data (Computed):
    - salesOverTimeData: { labels, data }
    - coldVsRegularData: { labels, data, colors }
    - revenueByDateData: { labels, coldData, regularData }
    - hourlyDistributionData: { labels, data }
    - topHoursData: { labels, data }
    - comparisonData: { labels, data }
    
    Machine Status (Computed):
    - machineStatus: { status, message }
    
    Table Data (Computed):
    - transactionHistory: Paginated transactions
    
    Actions:
    - setLogs(logs): Load logs from Firestore
    - setDateRange(start, end): Update date filters
    - setTimeRange(range): Quick time filter
    - setWaterType(type): Water type filter
    - setHourRange(start, end): Hour filter
    - resetFilters(): Clear all filters
```

#### Router (src/router/)
Vue Router configuration with authentication guards.

```
src/router/
└── index.ts                          # Route definitions
    - /login: Login page (public)
    - /dashboard: Dashboard page (authenticated only)
    - Auth guard: Redirects based on login status
    - Routes use lazy loading for efficiency
```

#### Views (src/views/)
Full page components.

```
src/views/
├── Login.vue                         # Authentication page
│   - Email/password input
│   - Sign in / Sign up toggle
│   - Error handling
│
└── Dashboard.vue                     # Main dashboard page
    - Statistics cards row
    - Filters panel
    - 6 interactive charts in 2-column grid
    - Transaction history table
    - Machine status & export panels
    - Real-time Firestore listener
    - Alert notifications
```

#### Components (src/components/)
Reusable Vue components.

##### Statistics Components
```
src/components/
├── StatisticsCards.vue               # 6 KPI cards
│   - Total Cups Sold
│   - Total Revenue
│   - Average Price/Transaction
│   - Cold vs Regular (%)
│   - Sales/Hour Average
│   - Last Transaction Details
```

##### Filter Components
```
├── FilterPanel.vue                   # Advanced filtering UI
│   - Date range picker
│   - Water type selector
│   - Time period quick filters (Today/Week/Month/All)
│   - Hour range slider
│   - Transaction count display
```

##### Chart Components (6 charts)
```
src/components/charts/
├── SalesOverTimeChart.vue            # Area chart
│   - Sales trends (hourly or daily based on range)
│
├── ColdVsRegularChart.vue            # Doughnut chart
│   - Percentage breakdown of water types
│
├── RevenueByDateChart.vue            # Stacked bar chart
│   - Revenue by date, separated by water type
│
├── HourlySalesChart.vue              # Line chart
│   - 24-hour sales distribution
│
├── TopHoursChart.vue                 # Horizontal bar chart
│   - Top 5 sales hours
│
└── ComparisonChart.vue               # Multi-series bar chart
    - Today vs This Week vs This Month comparison
```

##### Data & Status Components
```
├── TransactionHistory.vue            # Paginated transaction table
│   - Timestamp, Type, Amount, Date columns
│   - 10 rows per page
│   - Previous/Next pagination
│   - Sortable columns
│
├── MachineStatus.vue                 # Status indicator
│   - Online/Offline status dot (green/red)
│   - Last seen time
│   - Low water alert (>48 cups)
│
├── ExportPanel.vue                   # Export buttons
│   - Download as CSV
│   - Download as PDF
│   - Respects current filters
│
└── AlertNotifications.vue            # Toast notifications
    - Low water alerts (>48 cups)
    - Inactivity alerts (>4 hours)
    - Browser notifications
    - Auto-dismiss after 5 seconds
```

---

## File Count Summary

| Category | Count |
|----------|-------|
| Configuration Files | 7 |
| Documentation Files | 5 |
| Setup Scripts | 2 |
| Service Files | 4 |
| Store Files | 1 |
| Router Files | 1 |
| View Files | 2 |
| UI Components | 10 |
| Chart Components | 6 |
| Total | **38 Files** |

---

## Component Tree

```
App.vue
└── router-view
    ├── Login.vue (public route)
    │   └── Form inputs & auth
    │
    └── Dashboard.vue (protected route)
        ├── StatisticsCards
        │   ├── 6 KPI card components
        │   └── Real-time stat updates
        │
        ├── MachineStatus
        │   ├── Status indicator
        │   └── Low water alert
        │
        ├── ExportPanel
        │   ├── CSV button
        │   ├── PDF button
        │   └── Excel button
        │
        ├── FilterPanel
        │   ├── Date range picker
        │   ├── Water type selector
        │   ├── Time period buttons
        │   ├── Hour range slider
        │   └── Reset button
        │
        ├── Charts (6)
        │   ├── SalesOverTimeChart
        │   ├── ColdVsRegularChart
        │   ├── RevenueByDateChart
        │   ├── HourlySalesChart
        │   ├── TopHoursChart
        │   └── ComparisonChart
        │
        ├── TransactionHistory
        │   ├── Table with pagination
        │   └── Next/Previous buttons
        │
        └── AlertNotifications
            └── Toast messages (bottom-right)
```

---

## Technology Stack

### Frontend Framework
- **Vue.js 3** - Progressive JavaScript framework
- **Vue Router 4** - Routing & navigation
- **Pinia** - State management store

### Build & Development
- **Vite 8** - Lightning-fast build tool & dev server
- **TypeScript** - Type-safe JavaScript
- **Tailwind CSS** - Utility-first styling framework

### Data & Charts
- **Firebase 12** - Backend as a Service
- **Firestore** - Real-time cloud database
- **Chart.js 4** - Interactive charts
- **vue-chartjs 5** - Vue wrapper for Chart.js

### Data Export
- **jsPDF 4** - PDF generation

### Utilities  
- **date-fns 2.30** - Date manipulation

---

## Key Features by File

### Real-Time Updates
**Files:** `firestoreService.ts`, `Dashboard.vue`, `dashboardStore.ts`
- Firestore listeners automatically sync data
- Sub-second updates to all charts
- No polling required

### Advanced Filtering
**Files:** `FilterPanel.vue`, `dashboardStore.ts`
- Date range with calendar picker
- Water type selector
- 4 quick time period buttons
- Hour-of-day range slider
- All filters combined in real-time

### Interactive Charts
**Files:** `ChartComponent.vue` (all 6)
- Each chart updates instantly on data/filter change
- Responsive grid layout
- Automatic scaling
- Legend and tooltips included

### Data Export
**Files:** `ExportPanel.vue`, `exportService.ts`
- Current filters applied to export
- Multiple formats (CSV, PDF)
- One-click download
- Timestamp in filename

### Authentication
**Files:** `Login.vue`, `authService.ts`, `router/index.ts`
- Firebase email/password auth
- Route-based protection
- Automatic redirect based on auth state
- Logout functionality

### Alerts & Notifications
**Files:** `AlertNotifications.vue`, `alertService.ts`, `MachineStatus.vue`
- Low water warning (>48 cups)
- Inactivity alert (>4 hours)
- Browser notification support
- Toast notifications

---

## Development Workflow

### Adding a New Chart

1. Create component in `src/components/charts/YourChart.vue`
2. Add computed data in `dashboardStore.ts` (e.g., `yourChartData`)
3. Import Chart.js plugins needed
4. Reference store data in template
5. Add to grid in `Dashboard.vue`

### Adding a New Filter

1. Add state to `dashboardStore.ts` (e.g., `selectedNewFilter`)
2. Add computed `filteredLogs` logic
3. Add UI input to `FilterPanel.vue`
4. Call store action on change
5. All charts update automatically

### Adding Statistics

1. Add computed property to `dashboardStore.ts`
2. Add card to `StatisticsCards.vue`
3. Reference computed property with real-time binding
4. Add emoji and styling

---

## Performance Optimizations

- **Lazy component loading** in router
- **Computed properties** for reactive updates
- **Small bundle size** (Vite optimizations)
- **Efficient Firestore queries** with date range constraints
- **Pagination** in transaction history (10 items per page)
- **CSS modules** to avoid style conflicts

---

## Production Checklist

- [ ] Update `.env.local` with real Firebase credentials
- [ ] Test Firebase security rules
- [ ] Verify Firestore indexes created
- [ ] Test all filters work correctly
- [ ] Verify export formats download
- [ ] Test authentication flow
- [ ] Check responsive design on mobile
- [ ] Run `npm run build` to check for errors
- [ ] Build and upload `dist` contents to Hostinger `public_html`
- [ ] Test in production environment

---

## Need Help?

See these files for detailed information:
- **Setup**: `QUICKSTART.md` (5-minute guide)
- **Deployment**: `DEPLOYMENT.md` 
- **Database**: `FIRESTORE_SETUP.md`
- **Full Docs**: `README.md`
