package stubs;

import java.util.ArrayList;

import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;

import org.junit.Before;
import org.junit.Test;

public class TestProcessLogs {
	MapDriver<LongWritable, Text, Text, IntWritable> mapDriver;
	ReduceDriver<Text, IntWritable, Text, IntWritable> reduceDriver;
	MapReduceDriver<LongWritable, Text, Text, IntWritable, Text, IntWritable> mrDriver;
	
	@Before
	public void setUp() {
		LogFileMapper mapper = new LogFileMapper();
		mapDriver = new MapDriver<LongWritable, Text, Text, IntWritable>();
		mapDriver.setMapper(mapper);
		
		SumReducer reducer = new SumReducer();
		reduceDriver = new ReduceDriver<Text, IntWritable, Text, IntWritable>();
		reduceDriver.setReducer(reducer);
		
		mrDriver = new MapReduceDriver<LongWritable, Text, Text, IntWritable, Text, IntWritable>();
		mrDriver.setMapper(mapper);
		mrDriver.setReducer(reducer);
	}
	
	@Test
	public void testMapper(){
		mapDriver.withInput(new LongWritable(0), new Text("96.7.4.14 - - [24/Apr/2011:04:20:11 -0400] \"GET /cat.jpg HTTP/1.1\" 200 12433"));
		mapDriver.withOutput(new Text("96.7.4.14"), new IntWritable(1));
		mapDriver.runTest();
	}
	
	@Test
	public void testReducer(){
		ArrayList<IntWritable> lensarr = new ArrayList<IntWritable>();
		lensarr.add(new IntWritable(1));
		lensarr.add(new IntWritable(1));
		lensarr.add(new IntWritable(2));
		
		reduceDriver.withInput(new Text("96.7.4.14"), lensarr);
		reduceDriver.withOutput(new Text("96.7.4.14"), new IntWritable(4));
		reduceDriver.runTest();
	}
	
	@Test
	public void testMapReduce(){
		mrDriver.withInput(new LongWritable(0), new Text("96.7.4.14 - - [24/Apr/2011:04:20:11 -0400] \"GET /cat.jpg HTTP/1.1\" 200 12433"));
		mrDriver.withOutput(new Text("96.7.4.14"), new IntWritable(1));
		mrDriver.runTest();
	}
	
	
}
