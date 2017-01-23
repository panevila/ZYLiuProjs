package stubs;

import java.io.IOException;
import java.text.DecimalFormat;
import java.text.NumberFormat;

import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;


/**
 * Reducer's input is: (K, List<Integer>)
 *
 * Aggregate on the list of values and create a single (K,V), 
 * where V is the sum of all values passed in the list.
 *
 * This class, AggregateByKeyReducer, accepts (K, [2, 3]) and 
 * emits (K, 5).
 *
 *
 * @author Mahmoud Parsian
 *
 */
public class AverageReducer  extends
    Reducer<Text, DoubleWritable, Text, DoubleWritable> {
	DoubleWritable output = new DoubleWritable();
      @Override
      public void reduce(Text key, Iterable<DoubleWritable> values, Context context) 
         throws IOException, InterruptedException {
         
         double sum = 0;
         int num = 0;
         for (DoubleWritable value : values) {
               sum += value.get();
               num ++;
         }
         double avg = sum/num;
         output.set(avg);

         context.write(key, output);
      }
}