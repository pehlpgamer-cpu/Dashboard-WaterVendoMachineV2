# Fire base Firestore Rules Configuration

Firestore Security Rules:

```
rules_version = '2';
service cloud.firestore {
  match /databases/{database}/documents {
    // Allow authenticated users to read all documents
    match /{document=**} {
      allow read: if request.auth != null;
    }
    
    // Water logs collection - specific rules
    match /waterLogs/{document=**} {
      // Read: Authenticated users only
      allow read: if request.auth != null;
      
      // Create: Valid transactions only
      allow create: if isValidTransaction(request.resource.data);
      
      // Update & Delete: Disabled to maintain data integrity
      allow update, delete: if false;
    }
    
    // Helper function to validate transaction data
    function isValidTransaction(data) {
      return data.keys().hasAll(['amount', 'isCold', 'timestamp']) &&
             data.amount in [3, 5] &&
             data.isCold is bool &&
             data.timestamp is number &&
             data.timestamp > 0;
    }
  }
}
```

## Setup Instructions

1. Go to [Firebase Console](https://console.firebase.google.com/)
2. Select your project: `pauloheymann-integ-prog`
3. Navigate to Firestore Database → Rules
4. Replace default rules with the code above
5. Click "Publish"

## Rule Explanation

### Read Rules
- ✅ All authenticated users can read water logs
- ❌ Unauthenticated users cannot read

### Create Rules  
- ✅ Valid transactions with all required fields can be created
- ❌ Invalid data is rejected
- Fields required:
  - `amount`: Must be 3 or 5 (water prices)
  - `isCold`: Must be boolean
  - `timestamp`: Must be positive number (milliseconds)

### Update/Delete Rules
- ❌ Completely disabled
- Reason: Maintain data integrity and audit trail
- All transactions are immutable

## Testing Rules

### Locally (Emulator)
```bash
firebase emulators:start
```

### In Firebase Console
1. Go to Rules → Testing
2. Create test cases to verify read/write permissions

## Firestore Indexes

For optimal performance with date-range queries, create composite indexes:

**Index 1:** Date Range Queries
- Collection: `waterLogs`
- Field 1: `timestamp` (Descending)
- Field 2: `isCold` (Ascending)

**Index 2:** Water Type + Date
- Collection: `waterLogs`  
- Field 1: `isCold` (Ascending)
- Field 2: `timestamp` (Descending)

Firebase will automatically suggest indexes when you first run queries.

## Collection Structure

```
waterLogs/
├── doc_0001
│   ├── amount: 5
│   ├── isCold: true
│   └── timestamp: 1713607200000
├── doc_0002
│   ├── amount: 3
│   ├── isCold: false
│   └── timestamp: 1713607260000
└── ...
```

## Firestore Pricing

**Free Tier Limits (per day):**
- 50,000 reads
- 20,000 writes
- 20,000 deletes

**Pricing:**
- $0.06 per 100k reads
- $0.18 per 100k writes
- $0.02 per 100k deletes
- Minimum: $1/month if usage exceeds free tier

For a typical vending machine:
- ~1,000 transactions/day = 1,000 writes
- ~2,000 dashboard views/day = 2,000 reads
- **Estimated monthly cost: $0 (within free tier)**

## Monitoring Usage

1. Firebase Console → Firestore → Usage
2. Check daily read/write counts
3. Monitor quota warnings

## Backup Strategy

1. Export data periodically:
   ```bash
   firebase firestore:delete --all --project pauloheymann-integ-prog
   ```

2. Implement scheduled exports:
   - Use Cloud Functions to backup data
   - Export to Cloud Storage monthly

3. Retention policy:
   - Keep 1 year of data in Firestore
   - Archive older data to Cloud Storage

## Troubleshooting

### "Permission denied" error?
- Check Firestore rules (see above)
- Verify user is authenticated
- Check browser console for auth errors

### Queries not working?
- Check if composite index exists
- Review Firestore rules for query restrictions
- Ensure date format is ISO or milliseconds

### High usage costs?
- Reduce real-time listeners
- Implement pagination
- Use simpler aggregation queries
