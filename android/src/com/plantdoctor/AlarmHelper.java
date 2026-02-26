package com.plantdoctor;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.provider.Settings;
import android.util.Log;
import java.util.Calendar;

public class AlarmHelper {
    private static final String TAG = "AlarmHelper";
    private static final int REQUEST_CODE = 1000;

    public static void scheduleExactAlarm(Context context) {
        try {
            AlarmManager alarmManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
            if (alarmManager == null) {
                Log.e(TAG, "AlarmManager is null");
                return;
            }

            // Check for SCHEDULE_EXACT_ALARM permission on Android 12+
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                if (!alarmManager.canScheduleExactAlarms()) {
                    Log.e(TAG, "Cannot schedule exact alarms - prompting user");
                    Intent intent = new Intent(android.provider.Settings.ACTION_REQUEST_SCHEDULE_EXACT_ALARM);
                    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    try {
                        context.startActivity(intent);
                        Log.d(TAG, "Prompted user for SCHEDULE_EXACT_ALARM permission");
                    } catch (Exception e) {
                        Log.e(TAG, "Failed to prompt for SCHEDULE_EXACT_ALARM", e);
                    }
                    return;
                }
            }

            Intent intent = new Intent(context, AlarmReceiver.class);
            intent.setAction("com.salesmate.ALARM_TRIGGERED");
            intent.putExtra("source", "qt_android");

            int flags = PendingIntent.FLAG_UPDATE_CURRENT;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                flags |= PendingIntent.FLAG_IMMUTABLE;
            }

            PendingIntent pendingIntent = PendingIntent.getBroadcast(
                context,
                REQUEST_CODE,
                intent,
                flags
            );

            // Calculate next Sunday at 9:00 PM
            Calendar calendar = Calendar.getInstance();
            calendar.setTimeInMillis(System.currentTimeMillis());
            calendar.set(Calendar.HOUR_OF_DAY, 21); // 9:00 PM
            calendar.set(Calendar.MINUTE, 0);
            calendar.set(Calendar.SECOND, 0);
            calendar.set(Calendar.MILLISECOND, 0);

            // Set to next Sunday
            int currentDay = calendar.get(Calendar.DAY_OF_WEEK);
            int daysUntilSunday = (Calendar.SUNDAY - currentDay + 7) % 7;
            if (daysUntilSunday == 0 && calendar.getTimeInMillis() <= System.currentTimeMillis()) {
                // If today is Sunday and time has passed 9:00 PM, schedule for next Sunday
                daysUntilSunday = 7;
            }
            calendar.add(Calendar.DAY_OF_YEAR, daysUntilSunday);

            long triggerAt = calendar.getTimeInMillis();
            Log.d(TAG, "Scheduling alarm for: " + calendar.getTime());

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                alarmManager.setExactAndAllowWhileIdle(
                    AlarmManager.RTC_WAKEUP,
                    triggerAt,
                    pendingIntent
                );
            } else {
                alarmManager.setExact(
                    AlarmManager.RTC_WAKEUP,
                    triggerAt,
                    pendingIntent
                );
            }
            Log.d(TAG, "Alarm scheduled successfully");
        } catch (Exception e) {
            Log.e(TAG, "Alarm scheduling failed", e);
        }
    }

    public static void cancelAlarm(Context context) {
        Intent intent = new Intent(context, AlarmReceiver.class);
        int flags = PendingIntent.FLAG_NO_CREATE;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            flags |= PendingIntent.FLAG_IMMUTABLE;
        }

        PendingIntent pendingIntent = PendingIntent.getBroadcast(
            context, REQUEST_CODE, intent, flags
        );

        if (pendingIntent != null) {
            AlarmManager alarmManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
            alarmManager.cancel(pendingIntent);
            pendingIntent.cancel();
            Log.d(TAG, "Alarm canceled");
        }
    }
}


