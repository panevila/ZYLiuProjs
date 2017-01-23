package stubs;

import java.io.IOException;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

/**
 *  Mapper's input are read from SequenceFile and records are: (K, V)
 *    where 
 *          K is a Text
 *          V is an Integer
 * 
 * @author Mahmoud Parsian
 *
 */
public class SepMapper extends
   Mapper<Text, DoubleWritable, NullWritable, DoubleWritable> {

	@Override
   	public void map(Text key, DoubleWritable value, Context context)
         throws IOException, InterruptedException {
      context.write(NullWritable.get(), value);
   }
}