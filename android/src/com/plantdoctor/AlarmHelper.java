package com.plantdoctor;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.util.Log;

import java.util.Calendar;

public class AlarmHelper {

    private static final String TAG = "AlarmHelper";
    private static final int REQUEST_CODE = 1000;

    public static final String ACTION_ALARM = "com.plantdoctor.ALARM_TRIGGERED";

    // =========================
    // PUBLIC API
    // =========================
    public static void scheduleExactAlarm(Context context) {
        AlarmManager alarmManager = getAlarmManager(context);
        if (alarmManager == null) return;

        if (!canScheduleExactAlarm(context, alarmManager)) return;

        PendingIntent pi = buildPendingIntent(context, PendingIntent.FLAG_UPDATE_CURRENT);
        if (pi == null) return;

        long triggerAt = computeNextSunday9PM();
        Log.d(TAG, "Scheduling alarm at: " + triggerAt);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            alarmManager.setExactAndAllowWhileIdle(
                    AlarmManager.RTC_WAKEUP,
                    triggerAt,
                    pi
            );
        } else {
            alarmManager.setExact(
                    AlarmManager.RTC_WAKEUP,
                    triggerAt,
                    pi
            );
        }

        Log.d(TAG, "Alarm scheduled successfully");
    }

    public static void cancelAlarm(Context context) {
        AlarmManager alarmManager = getAlarmManager(context);
        if (alarmManager == null) return;

        PendingIntent pi = buildPendingIntent(context, PendingIntent.FLAG_NO_CREATE);
        if (pi != null) {
            alarmManager.cancel(pi);
            pi.cancel();
            Log.d(TAG, "Alarm cancelled");
        } else {
            Log.d(TAG, "No existing alarm to cancel");
        }
    }

    // =========================
    // INTERNAL HELPERS
    // =========================
    private static AlarmManager getAlarmManager(Context context) {
        AlarmManager am = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
        if (am == null) {
            Log.e(TAG, "AlarmManager is null");
        }
        return am;
    }

    private static boolean canScheduleExactAlarm(Context context, AlarmManager am) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (!am.canScheduleExactAlarms()) {
                Log.w(TAG, "Exact alarm permission missing");

                Intent intent = new Intent(android.provider.Settings.ACTION_REQUEST_SCHEDULE_EXACT_ALARM);
                intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

                try {
                    context.startActivity(intent);
                } catch (Exception e) {
                    Log.e(TAG, "Failed to open exact alarm settings", e);
                }
                return false;
            }
        }
        return true;
    }

    private static PendingIntent buildPendingIntent(Context context, int baseFlag) {
        Intent intent = new Intent(context, AlarmReceiver.class);
        intent.setAction(ACTION_ALARM);
        intent.putExtra("source", "qt_android");

        int flags = baseFlag;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            flags |= PendingIntent.FLAG_IMMUTABLE;
        }

        return PendingIntent.getBroadcast(
                context,
                REQUEST_CODE,
                intent,
                flags
        );
    }

    private static long computeNextSunday9PM() {
        Calendar now = Calendar.getInstance();
        Calendar target = Calendar.getInstance();

        target.set(Calendar.HOUR_OF_DAY, 21);
        target.set(Calendar.MINUTE, 0);
        target.set(Calendar.SECOND, 0);
        target.set(Calendar.MILLISECOND, 0);

        int currentDay = now.get(Calendar.DAY_OF_WEEK);
        int daysUntilSunday = (Calendar.SUNDAY - currentDay + 7) % 7;

        if (daysUntilSunday == 0 && target.before(now)) {
            daysUntilSunday = 7;
        }

        target.add(Calendar.DAY_OF_YEAR, daysUntilSunday);

        return target.getTimeInMillis();
    }
}

