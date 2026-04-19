# Firebase Deployment Configuration

## Prerequisites
1. Install Firebase CLI: `npm install -g firebase-tools`
2. Authenticate: `firebase login`

## Deployment Steps

1. Build for production:
   ```bash
   npm run build
   ```

2. Initialize Firebase:
   ```bash
   firebase init
   ```

3. Deploy:
   ```bash
   firebase deploy
   ```

## Environment Variables

Create a `.env.local` file in the root directory:

```env
# Firebase Config (from Firebase Console > Project Settings)
VITE_FIREBASE_API_KEY=YOUR_API_KEY
VITE_FIREBASE_AUTH_DOMAIN=pauloheymann-integ-prog.firebaseapp.com
VITE_FIREBASE_PROJECT_ID=pauloheymann-integ-prog
VITE_FIREBASE_STORAGE_BUCKET=pauloheymann-integ-prog.appspot.com
VITE_FIREBASE_MESSAGING_SENDER_ID=YOUR_SENDER_ID
VITE_FIREBASE_APP_ID=YOUR_APP_ID
```

## Update firebaseConfig.ts

Modify `src/services/firebaseConfig.ts` to use environment variables:

```typescript
const firebaseConfig = {
  apiKey: import.meta.env.VITE_FIREBASE_API_KEY,
  authDomain: import.meta.env.VITE_FIREBASE_AUTH_DOMAIN,
  projectId: import.meta.env.VITE_FIREBASE_PROJECT_ID,
  storageBucket: import.meta.env.VITE_FIREBASE_STORAGE_BUCKET,
  messagingSenderId: import.meta.env.VITE_FIREBASE_MESSAGING_SENDER_ID,
  appId: import.meta.env.VITE_FIREBASE_APP_ID
}
```

## Vercel Deployment

1. Connect your GitHub repo to Vercel
2. Add environment variables in Vercel settings
3. Deploy automatically on push to main branch

## Firebase Firestore Security Rules

```
rules_version = '2';
service cloud.firestore {
  match /databases/{database}/documents {
    // Allow authenticated users to read water logs
    match /waterLogs/{document=**} {
      allow read: if request.auth != null;
      allow create: if isValidTransaction(request.resource.data);
      allow update, delete: if false; // Prevent modification of historical data
    }
    
    // Helper function to validate transaction data
    function isValidTransaction(data) {
      return data.keys().hasAll(['amount', 'isCold', 'timestamp']) &&
             data.amount in [3, 5] &&
             data.isCold is bool &&
             data.timestamp is number;
    }
  }
}
```

## Firestore Indexes

If you query by date range frequently, create a composite index:

1. Go to Firebase Console → Firestore → Indexes
2. Create composite index:
   - Collection: `waterLogs`
   - Fields: `timestamp` (Descending), `isCold` (Ascending)

## ESP32 Integration

Update your ESP32 Arduino code to use your Firebase credentials:

```cpp
#define FIREBASE_API_KEY "YOUR_API_KEY"
#define FIREBASE_PROJECT_ID "pauloheymann-integ-prog"
```

The ESP32 will send data to the `waterLogs` collection automatically.

## Monitoring & Analytics

1. **Firebase Console** - Real-time database stats
2. **Google Analytics** - Dashboard traffic (if enabled)
3. **Firestore Usage** - Monitor read/write counts

## Cost Optimization

- **Firestore Free Tier**: 50k reads/day, 20k writes/day
- Limit real-time listeners where possible
- Use pagination to avoid large data transfers
- Archive old data periodically

## Troubleshooting

### Firebase not connecting?
- Verify API key is correct (not redacted in console)
- Check security rules (should allow reads for authenticated users)
- Ensure Firestore is enabled

### Build failing?
- Run `npm install` to ensure all dependencies
- Check Node.js version (v18+ recommended)
- Clear `node_modules` and reinstall if needed

### Slow performance?
- Check Firestore indexes
- Reduce number of simultaneous listeners
- Implement pagination for large datasets
