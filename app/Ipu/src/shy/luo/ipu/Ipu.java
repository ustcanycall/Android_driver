package shy.luo.ipu;

import android.app.Activity;

import android.os.ServiceManager;

import android.os.Bundle;
import android.os.IIpuService;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class Ipu extends Activity implements OnClickListener {
	private final static String LOG_TAG = "shy.luo.ipu.IpuActivity";
	
	private IIpuService ipuService = null;
    
    // MMAP BUFFER
    private EditText bufferoffsetText = null;
	private EditText buffervalueText = null;
	private Button bufferreadButton = null;
	private Button bufferwriteButton = null;
	private Button bufferclearButton = null;
	
    // CONFIG REG
    private EditText regoffsetText = null;
	private EditText regvalueText = null;
	private Button regreadButton = null;
	private Button regwriteButton = null;
	private Button regclearButton = null;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

	    ipuService = IIpuService.Stub.asInterface(
		        ServiceManager.getService("ipu"));
        
        bufferoffsetText = (EditText)findViewById(R.id.buffer_offset_value);
        buffervalueText = (EditText)findViewById(R.id.buffer_value);
        bufferreadButton = (Button)findViewById(R.id.buffer_button_read);
        bufferwriteButton = (Button)findViewById(R.id.buffer_button_write);
        bufferclearButton = (Button)findViewById(R.id.buffer_button_clear);
        
	    bufferreadButton.setOnClickListener(this);
	    bufferwriteButton.setOnClickListener(this);
	    bufferclearButton.setOnClickListener(this);
        
        regoffsetText = (EditText)findViewById(R.id.reg_offset_value);
        regvalueText = (EditText)findViewById(R.id.reg_value);
        regreadButton = (Button)findViewById(R.id.reg_button_read);
        regwriteButton = (Button)findViewById(R.id.reg_button_write);
        regclearButton = (Button)findViewById(R.id.reg_button_clear);
        
	    regreadButton.setOnClickListener(this);
	    regwriteButton.setOnClickListener(this);
	    regclearButton.setOnClickListener(this);

        Log.i(LOG_TAG, "Ipu Activity Created");
    }
    
    @Override
    public void onClick(View v) 
    {
    	if(v.equals(bufferreadButton)) 
        {
		    try {
                int[] val;
                int offset;
                val = new int[2];

                String offset_text = bufferoffsetText.getText().toString();
                offset = Integer.parseInt(offset_text);

    			ipuService.start();
    			ipuService.read_dma_buffer(val, offset, 0, 1);
    			String text = String.valueOf(val[0]);
    			buffervalueText.setText(text);
		    } 
            catch (RemoteException e) 
            {
			    Log.e(LOG_TAG, "Remote Exception while reading value from Ipu service.");
		    }		
    	}
    	else if(v.equals(bufferwriteButton)) 
        {
		    try 
            {
                int []val;
                val = new int[2];
    			String text = buffervalueText.getText().toString();
    			val[0] = Integer.parseInt(text);

                int offset;
                String offset_text = bufferoffsetText.getText().toString();
                offset = Integer.parseInt(offset_text);
			    ipuService.reset();

			    ipuService.write_dma_buffer(val, 0, offset, 1);
		    } 
            catch (RemoteException e) 
            {
			    Log.e(LOG_TAG, "Remote Exception while writing value to Ipu service.");
		    }
    	}
    	else if(v.equals(bufferclearButton)) {
    		String text = "";
    		buffervalueText.setText(text);
    		bufferoffsetText.setText(text);
    	}
    	else if(v.equals(regreadButton)) 
        {
		    try {
                int offset;
                int reg_value;

                String offset_text = regoffsetText.getText().toString();
                offset = Integer.parseInt(offset_text);

    			ipuService.start();

    			reg_value = ipuService.read_config_dword(offset);
    			String text = String.valueOf(reg_value);
    			regvalueText.setText(text);
		    } 
            catch (RemoteException e) 
            {
			    Log.e(LOG_TAG, "Remote Exception while reading value from Ipu service.");
		    }		
    	}
    	else if(v.equals(regwriteButton)) 
        {
		    try 
            {
                int reg_value;
    			String text = regvalueText.getText().toString();
    			reg_value = Integer.parseInt(text);

                int offset;
                String offset_text = regoffsetText.getText().toString();
                offset = Integer.parseInt(offset_text);
			    ipuService.reset();

			    ipuService.write_config_dword(offset, reg_value);
		    } 
            catch (RemoteException e) 
            {
			    Log.e(LOG_TAG, "Remote Exception while writing value to Ipu service.");
		    }
    	}
    	else if(v.equals(regclearButton)) {
    		String text = "";
    		regvalueText.setText(text);
    		regoffsetText.setText(text);
    	}
    }
}
