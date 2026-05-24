# Hostinger Deployment Guide

This app is a static Vue/Vite dashboard. Hostinger does not need to run a Node.js server for production; upload the built files to `public_html`.

## 1. Firebase Production Environment

Create `.env.production.local` in the project root. Do not commit this file.

```env
VITE_FIREBASE_API_KEY=your_real_web_api_key
VITE_FIREBASE_AUTH_DOMAIN=pauloheymann-integ-prog.firebaseapp.com
VITE_FIREBASE_PROJECT_ID=pauloheymann-integ-prog
VITE_FIREBASE_STORAGE_BUCKET=pauloheymann-integ-prog.appspot.com
VITE_FIREBASE_MESSAGING_SENDER_ID=your_real_sender_id
VITE_FIREBASE_APP_ID=your_real_app_id
```

Get these values from Firebase Console -> Project Settings -> General -> Web app.

## 2. Firebase Checklist

1. Enable Authentication -> Email/Password.
2. Create production dashboard users manually in Firebase Authentication.
3. Add your Hostinger domain to Authentication -> Settings -> Authorized domains.
4. Publish the Firestore rules from `FIRESTORE_SETUP.md`.
5. Confirm `waterLogs` documents use:

```json
{
  "amount": 5,
  "isCold": true,
  "timestamp": 1713607200000,
  "timeSynced": true,
  "clientUptimeMs": 18400
}
```

## 3. Build

```bash
npm ci
npm run build
```

The build script runs:

1. `vue-tsc --noEmit`
2. Firebase env validation
3. `vite build`

If Firebase env values are blank or placeholder values, build stops before producing a deployable artifact.

## 4. Upload To Hostinger

1. Open Hostinger hPanel -> Websites -> your domain -> File Manager.
2. Open the domain `public_html` folder.
3. Upload the **contents of `dist`**, not the `dist` folder itself.
4. Ensure these exist directly inside `public_html`:
   - `index.html`
   - `assets/`
   - `.htaccess`
5. Visit your domain and hard-refresh.
6. Open `/dashboard` directly to confirm the `.htaccess` SPA fallback works.

## 5. Verification

Run before upload:

```bash
npm run check
npm run build
npm audit --omit=dev --audit-level=high
```

After upload:

1. Log in with a pre-created Firebase user.
2. Confirm an unknown user cannot register from the dashboard.
3. Refresh `/dashboard` and confirm it still loads.
4. Confirm charts, transaction table, CSV export, PDF export, alerts, and machine status work with Firestore data.

## Notes

- Public signup is intentionally disabled in the app. Add users in Firebase Console.
- Mock data mode is development-only and is not available in production builds.
- Spreadsheet export was removed because the previous browser spreadsheet dependency had unresolved high-severity advisories.
