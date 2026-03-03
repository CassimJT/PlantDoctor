package com.plantdoctor;

import android.app.Activity;
import android.net.Uri;
import android.util.Base64;
import android.util.Log;
import java.io.InputStream;

public class GalleryHelper {

    public static String loadContentUri(Activity activity, String uriString) {
        Log.d("GalleryHelper", "loadContentUri() called with URI: " + uriString);
        try {
            Uri uri = Uri.parse(uriString);
            InputStream inputStream = activity.getContentResolver().openInputStream(uri);
            if (inputStream == null) {
                Log.w("GalleryHelper", "InputStream is null");
                return "";
            }

            byte[] buffer = new byte[8192];
            int bytesRead;
            java.io.ByteArrayOutputStream output = new java.io.ByteArrayOutputStream();

            while ((bytesRead = inputStream.read(buffer)) != -1) {
                output.write(buffer, 0, bytesRead);
            }

            inputStream.close();
            byte[] imageBytes = output.toByteArray();
            String base64 = Base64.encodeToString(imageBytes, Base64.NO_WRAP);
            Log.d("GalleryHelper", "Base64 length: " + base64.length());
            return base64;

        } catch (Exception e) {
            Log.e("GalleryHelper", "Error reading URI: " + e.getMessage(), e);
            return "";
        }
    }
}
