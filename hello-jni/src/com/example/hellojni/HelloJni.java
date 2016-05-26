/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.example.hellojni;

import android.app.Activity;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.view.View;
import android.widget.TextView;
import android.os.Bundle;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.EditText;

public class HelloJni extends Activity
{
	int rowa,cola,colb;
	TextView timetv;
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        /* Create a TextView and set its content.
         * the text is retrieved by calling a native
         * function.
         */
        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        setContentView(layout);
        
        TextView  tv = new TextView(this);
        layout.addView(tv);
        tv.setText( getDeviceInfo() );
        
        LinearLayout matrix_size_layout = new LinearLayout(this);
        matrix_size_layout.setOrientation(LinearLayout.HORIZONTAL);
        layout.addView(matrix_size_layout);
        
        TextView ratv = new TextView(this);
        ratv.setText("RowA: ");
        matrix_size_layout.addView(ratv);
        
        EditText raet = new EditText(this);
        raet.setRawInputType(InputType.TYPE_CLASS_NUMBER);
        matrix_size_layout.addView(raet);
        raet.addTextChangedListener(new TextWatcher(){

			@Override
			public void afterTextChanged(Editable s) {
				// TODO 自动生成的方法存根
				String str = s.toString();
				rowa = Integer.parseInt(str);
			}

			@Override
			public void beforeTextChanged(CharSequence s, int start, int count,
					int after) {
				// TODO 自动生成的方法存根
				
			}

			@Override
			public void onTextChanged(CharSequence s, int start, int before,
					int count) {
				// TODO 自动生成的方法存根
				
			}
        	
        });
        
        TextView catv = new TextView(this);
        catv.setText("ColA: ");
        matrix_size_layout.addView(catv);
        
        EditText caet = new EditText(this);
        caet.setRawInputType(InputType.TYPE_CLASS_NUMBER);
        matrix_size_layout.addView(caet);
        caet.addTextChangedListener(new TextWatcher(){

			@Override
			public void afterTextChanged(Editable s) {
				// TODO 自动生成的方法存根
				String str = s.toString();
				cola = Integer.parseInt(str);
			}

			@Override
			public void beforeTextChanged(CharSequence s, int start, int count,
					int after) {
				// TODO 自动生成的方法存根
				
			}

			@Override
			public void onTextChanged(CharSequence s, int start, int before,
					int count) {
				// TODO 自动生成的方法存根
				
			}
        	
        });

        TextView cbtv = new TextView(this);
        cbtv.setText("ColB: ");
        matrix_size_layout.addView(cbtv);
        
        EditText cbet = new EditText(this);
        cbet.setRawInputType(InputType.TYPE_CLASS_NUMBER);
        matrix_size_layout.addView(cbet);
        cbet.addTextChangedListener(new TextWatcher(){

			@Override
			public void afterTextChanged(Editable s) {
				// TODO 自动生成的方法存根
				String str = s.toString();
				colb = Integer.parseInt(str);
			}

			@Override
			public void beforeTextChanged(CharSequence s, int start, int count,
					int after) {
				// TODO 自动生成的方法存根
				
			}

			@Override
			public void onTextChanged(CharSequence s, int start, int before,
					int count) {
				// TODO 自动生成的方法存根
				
			}
        	
        });
        
        Button bn = new Button(this);
        layout.addView(bn);
        bn.setText("RUN TEST");
        bn.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO 自动生成的方法存根
				String timestr = calculateByOpenCL(rowa,cola,colb);
				String timetext = "Using " + timestr + " ms";
				timetv.setText(timetext);
				timetv.invalidate();
			}
		});
        
        timetv = new TextView(this);
        layout.addView(timetv);
        timetv.setText("Using 0 ms");
    }

    /* A native method that is implemented by the
     * 'hello-jni' native library, which is packaged
     * with this application.
     */
    public native String  getDeviceInfo();
    public native String calculateByOpenCL(int rowa,int cola,int colb);

    /* this is used to load the 'hello-jni' library on application
     * startup. The library has already been unpacked into
     * /data/data/com.example.hellojni/lib/libhello-jni.so at
     * installation time by the package manager.
     */
    static {
        System.loadLibrary("hello-jni");
    }
}
