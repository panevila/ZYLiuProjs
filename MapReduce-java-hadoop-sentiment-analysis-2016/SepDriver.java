package stubs;


import org.apache.log4j.Logger;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;


/**
 * TopNDriver: assumes that all K's are unique for all given (K,V) values.
 * Uniqueness of keys can be achieved by using AggregateByKeyDriver job.
 *
 * @author Mahmoud Parsian
 *
 */
public class SepDriver  extends Configured implements Tool {

   private static Logger THE_LOGGER = Logger.getLogger(SepDriver.class);

   public int run(String[] args) throws Exception {
      Job job = new Job(getConf());
      
      job.setJobName("Prob1bJob2");

      job.setJarByClass(SepDriver.class);
      
      job.setInputFormatClass(SequenceFileInputFormat.class);

      job.setMapperClass(SepMapper.class);
      job.setReducerClass(SepReducer.class);
      job.setPartitionerClass(SepPartitioner.class);	//set the partition class
      job.setNumReduceTasks(3);

      // map()'s output (K,V)
      job.setMapOutputKeyClass(NullWritable.class);   
      job.setMapOutputValueClass(DoubleWritable.class);   
      
      // reduce()'s output (K,V)
      job.setOutputKeyClass(Text.class);
      job.setOutputValueClass(DoubleWritable.class);

      FileInputFormat.setInputPaths(job, new Path(args[0]));
      FileOutputFormat.setOutputPath(job, new Path(args[1]));

      boolean status = job.waitForCompletion(true);
      
      
      return status ? 0 : 1;
   }

   /**
   * The main driver for "Top N" program.
   * Invoke this method to submit the map/reduce job.
   * @throws Exception When there is communication problems with the job tracker.
   */
   public static void main(String[] args) throws Exception {
      // Make sure there are exactly 3 parameters
      if (args.length != 2) {
         THE_LOGGER.warn("usage P1BJ2Driver <input> <output>");
         System.exit(1);
      }
      int returnStatus = ToolRunner.run(new SepDriver(), args);
      System.exit(returnStatus);
   }

}