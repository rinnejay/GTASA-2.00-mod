package com.example.ndkapp;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.EditText;

public class MainActivity extends Activity {

    static {
        System.loadLibrary("hooker");
        System.loadLibrary("target");
    }

    public native void startTargetLoop();
    public native void activateHookNow(int new_hp);
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        final TextView tv = new TextView(this);
      
        layout.addView(tv);
        Thread logcatThread = new Thread(() -> {
            try {
                startTargetLoop(); // Mulai loop target saat aplikasi dimulai
                Process process = Runtime.getRuntime().exec("logcat -s GTA_STYLE_HOOK TARGET_GAME");
                java.io.BufferedReader reader = new java.io.BufferedReader(
                        new java.io.InputStreamReader(process.getInputStream()));
                String line;
                while ((line = reader.readLine()) != null) {
                    if (line.contains("GTA_STYLE_HOOK") || line.contains("TARGET_GAME")) {
                        tv.setText(line);
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        });
        logcatThread.start();

        EditText input = new EditText(this);
        input.setInputType(android.text.InputType.TYPE_CLASS_NUMBER);
        input.setText("999"); // Default value
        input.setHint("Enter new value");
        layout.addView(input);

        Button hookButton = new Button(this);
        hookButton.setText("Activate Hook");
        hookButton.setOnClickListener(v -> {
            String newValue = input.getText().toString();
            //maximal panjang input 9 digits
            int maxInputLength = 9;
            if (newValue.isEmpty() || newValue.length() > maxInputLength) {
                input.setError("Input must be between 1 and " + maxInputLength + " digits");
                newValue = "100"; // Default ke 100 jika input kosong atau melebihi panjang maksimal
            }
            int hpValue = Integer.parseInt(newValue);
            activateHookNow(hpValue == 0 ? 100 : hpValue); // Default ke 100 jika input kosong atau 0
        });
        layout.addView(hookButton); 

        setContentView(layout);
    }
}