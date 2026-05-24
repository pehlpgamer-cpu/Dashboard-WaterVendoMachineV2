# Firestore Rules Configuration

Use these rules for the `waterLogs` collection.

```js
rules_version = '2';
service cloud.firestore {
  match /databases/{database}/documents {
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

## Setup

1. Go to Firebase Console.
2. Select project `pauloheymann-integ-prog`.
3. Open Firestore Database -> Rules.
4. Replace existing rules with the rules above.
5. Click Publish.

## Data Shape

```text
waterLogs/
  doc_0001
    amount: 5
    isCold: true
    timestamp: 1713607200000
    timeSynced: true
    clientUptimeMs: 18400
  doc_0002
    amount: 3
    isCold: false
    timestamp: 1713607260000
    timeSynced: false
    clientUptimeMs: 23800
```

`timestamp` is milliseconds since epoch. `timeSynced: false` means ESP32 queued an unsynced offline sale and uploaded it with an approximate flush-time timestamp. Dashboard validates documents client-side and ignores malformed rows.

## Indexes

Create this composite index if Firebase asks for it:

- Collection: `waterLogs`
- Field 1: `timestamp` descending
- Field 2: `isCold` ascending

## Operational Notes

- Reads require a signed-in Firebase user.
- Direct ESP32 create requests are schema-validated but unauthenticated; use device auth or a relay service if write identity becomes required.
- Historical transactions are immutable: update and delete are denied.
- The dashboard limits real-time reads to the latest 1000 rows to avoid accidental large reads.
