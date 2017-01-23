package stubs;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.Map;
import java.util.List;
import java.util.ArrayList;

import org.apache.hadoop.conf.Configuration;
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
public class TopNReducer  extends
   Reducer<NullWritable, Text, IntWritable, Text> {

   private int N = 10; // default
   private SortedMap<Integer, String> top = new TreeMap<Integer, String>();
   private HashMap<String,String> titles = new HashMap<String,String>();

   @Override
   public void reduce(NullWritable key, Iterable<Text> values, Context context) 
      throws IOException, InterruptedException {
	   //Get movie titles
	   BufferedReader reader;
	   String tmpStr;
	   File f;
	   String tmpnum = new String();
	   String tmpstr = new String(); 
		try {
			f = new File("movie_titles.txt");
			reader = new BufferedReader(new FileReader(f));
			while((tmpStr = reader.readLine())!=null)
			{
				String[] words = tmpStr.split(",",3); 	//split by white space characters
				if (words.length!=3) continue;
				tmpnum = words[0];
				tmpstr = words[2];
				titles.put(tmpnum, tmpstr);
			}
		}catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
      for (Text value : values) {
    	  String valueAsString = value.toString().trim();
         String[] tokens = valueAsString.split(",");
         String tmpstr3 = titles.get(tokens[0]);
         String url = new String();
         if(tmpstr3!=null)
        	 url = tmpstr3;
         int frequency =  Integer.parseInt(tokens[1]);
         top.put(frequency, url);
         // keep only top N
         if (top.size() > N) {
            top.remove(top.firstKey());
         }
      }
      
      // emit final top N
        List<Integer> keys = new ArrayList<Integer>(top.keySet());
        for(int i=keys.size()-1; i>=0; i--){
         context.write(new IntWritable(keys.get(i)), new Text(top.get(keys.get(i))));
      }
   }
   
   @Override
   protected void setup(Context context) 
      throws IOException, InterruptedException {
      this.N = context.getConfiguration().getInt("N", 10); // default is top 10
   }


}