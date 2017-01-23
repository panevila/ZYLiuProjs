package stubs;

import java.io.IOException;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

/**
 *  Reducer's input are local top N from all mappers.
 *  We have a single reducer, which creates the final top N.
 * 
 * @author Mahmoud Parsian
 *
 */
public class SepReducer  extends Reducer<NullWritable, DoubleWritable, Text, IntWritable> {
	private Text K2 = new Text();
	private IntWritable V2 = new IntWritable();
	@Override
	public void reduce(NullWritable key, Iterable<DoubleWritable> values, Context context) 
	throws IOException, InterruptedException {
		int count = 0;
		for (DoubleWritable value : values) {
			count ++;
			if(count == 1)
			{
				String tmpstr = new String();
				if(value.get()>4.0)
					tmpstr = "> 4.0";
				else if(value.get()<2.0)
					tmpstr = "< 2.0";
				else
					tmpstr = "normal";
				K2.set(tmpstr);
			}
		}
		V2.set(count);
	    context.write(K2,V2);
	}
}