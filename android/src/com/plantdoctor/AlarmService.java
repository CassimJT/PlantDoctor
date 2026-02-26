package com.plantdoctor;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;
import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;
import android.util.Log;

public class AlarmService extends Service {
    private static boolean isRunning = false;
    private static final int NOTIFICATION_ID = 123;
    private static final String CHANNEL_ID = "alarm_service_channel";
    private static final String TAG = "AlarmService";

    @Override
    public void onCreate() {
        super.onCreate();
        isRunning = true;
        Log.d(TAG, "AlarmService created");
        startForegroundNotification();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "AlarmService started");
        // Stop the service immediately after starting
        stopSelf();
        return START_NOT_STICKY;
    }

    private void startForegroundNotification() {
        createNotificationChannel();
        
        // Use Android system icon instead of custom R.drawable.icon
        int iconId = android.R.drawable.ic_dialog_info; // or ic_notification, ic_dialog_alert
        
        Notification notification = new NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle("PlantDoctor Alarm")
            .setContentText("Processing alarms in background")
            .setSmallIcon(iconId)
            .setPriority(NotificationCompat.PRIORITY_LOW)
            .build();
            
        startForeground(NOTIFICATION_ID, notification);
        Log.d(TAG, "Foreground notification started");
    }

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(
                CHANNEL_ID,
                "Alarm Service",
                NotificationManager.IMPORTANCE_LOW
            );
            
            NotificationManager notificationManager = getSystemService(NotificationManager.class);
            if (notificationManager != null) {
                notificationManager.createNotificationChannel(channel);
                Log.d(TAG, "Notification channel created");
            }
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        isRunning = false;
        Log.d(TAG, "AlarmService destroyed");
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    public static boolean isServiceRunning() {
        return isRunning;
    }
}
