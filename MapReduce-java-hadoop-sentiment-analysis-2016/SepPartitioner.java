package stubs;


import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.conf.Configurable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.Partitioner;

public class SepPartitioner extends Partitioner<NullWritable, DoubleWritable> implements
    Configurable {
	
  private Configuration configuration;
  /**
   * Set up the positive and negative hash set in the setConf method.
   */
  @Override
  public void setConf(Configuration configuration) {
     /*
     * Add the positive and negative words to the respective sets using the files 
     * positive-words.txt and negative-words.txt.
     */
    /*
     * TODO implement 
     */
	  	//Logger LOGGER = Logger.getLogger(SentimentPartitioner.class.getName());
	  	
  }

  /**
   * Implement the getConf method for the Configurable interface.
   */
  @Override
  public Configuration getConf() {
    return configuration;
  }

  /**
   * You must implement the getPartition method for a partitioner class.
   * This method receives the words as keys (i.e., the output key from the mapper.)
   * It should return an integer representation of the sentiment category
   * (positive, negative, neutral).
   * 
   * For this partitioner to work, the job configuration must have been
   * set so that there are exactly 3 reducers.
   */
  public int getPartition(NullWritable key, DoubleWritable value, int numReduceTasks) {
    /*
     * TODO implement
     * Change the return 0 statement below to return the number of the sentiment 
     * category; use 0 for positive words, 1 for negative words, and 2 for neutral words. 
     * Use the sets of positive and negative words to find out the sentiment.
     *
     * Hint: use positive.contains(key.toString()) and negative.contains(key.toString())
     * If a word appears in both lists assume it is positive. That is, once you found 
     * that a word is in the positive list you do not need to check if it is in the 
     * negative list. 
     */
	  if(value.get()>4.0) return 0;
	  else if(value.get()<2.0) return 1;
	  else return 2;
  }
}
