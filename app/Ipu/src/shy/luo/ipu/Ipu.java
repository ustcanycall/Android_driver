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

	private EditText valueText = null;
	private Button readButton = null;
	private Button writeButton = null;
	private Button clearButton = null;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

	    ipuService = IIpuService.Stub.asInterface(
		        ServiceManager.getService("ipu"));
        
        valueText = (EditText)findViewById(R.id.edit_value);
        readButton = (Button)findViewById(R.id.button_read);
        writeButton = (Button)findViewById(R.id.button_write);
        clearButton = (Button)findViewById(R.id.button_clear);

	    readButton.setOnClickListener(this);
	    writeButton.setOnClickListener(this);
	    clearButton.setOnClickListener(this);
        
        Log.i(LOG_TAG, "Ipu Activity Created");
    }
    
    @Override
    public void onClick(View v) 
    {
    	if(v.equals(readButton)) 
        {
		    try {
                int[] val;
                val = new int[2];
    			ipuService.start();
    			ipuService.read_dma_buffer(val, 0, 0, 1);
    			String text = String.valueOf(val[0]);
    			valueText.setText(text);
		    } 
            catch (RemoteException e) 
            {
			    Log.e(LOG_TAG, "Remote Exception while reading value from Ipu service.");
		    }		
    	}
    	else if(v.equals(writeButton)) 
        {
		    try 
            {
    			String text = valueText.getText().toString();
                int []val;
                val = new int[2];
    			val[0] = Integer.parseInt(text);
			    ipuService.reset();
			    ipuService.write_dma_buffer(val, 0, 0, 1);
		    } 
            catch (RemoteException e) 
            {
			    Log.e(LOG_TAG, "Remote Exception while writing value to Ipu service.");
		    }
    	}
    	else if(v.equals(clearButton)) {
    		String text = "";
    		valueText.setText(text);
    	}
    }
}
