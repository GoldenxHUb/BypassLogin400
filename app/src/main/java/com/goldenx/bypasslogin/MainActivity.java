package com.goldenx.bypasslogin;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        TextView tv = new TextView(this);
        tv.setText("Bypass Login 400\n\n" +
                "✅ รองรับ Free Fire + Free Fire MAX\n" +
                "✅ ติดตั้ง Zygisk Module แล้ว\n" +
                "✅ เปิดเกม → Login ผ่านอัตโนมัติ\n\n" +
                "ไม่ต้องกดอะไรในแอปนี้");
        tv.setTextSize(18);
        tv.setPadding(50, 50, 50, 50);

        setContentView(tv);
    }
}
