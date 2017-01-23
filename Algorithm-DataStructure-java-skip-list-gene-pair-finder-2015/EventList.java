//
// EVENTLIST.JAVA
// Skeleton code for your EventList collection type.
//
import java.util.*;

class EventList {
    
    Random randseq;
    
    ////////////////////////////////////////////////////////////////////
    // Here's a suitable geometric random number generator for choosing
    // pillar heights.  We use Java's ability to generate random booleans
    // to simulate coin flips.
    ////////////////////////////////////////////////////////////////////
    
    int randomHeight()
    {
	int v = 1;
	while (randseq.nextBoolean()) { v++; }
	return v;
    }

    
    //
    // Constructor
    //
    public EventList()
    {
	randseq = new Random(58243); // You may seed the PRNG however you like.
    }

    
    //
    // Add an Event to the list.
    //
    public void insert(Event e)
    {
	
    }

    
    //
    // Remove all Events in the list with the specified year.
    //
    public void remove(int year)
    {
	
    }
    
    //
    // Find all events with greatest year <= input year
    //
    public Event [] findMostRecent(int year)
    {
	return null;
    }
    
    
    //
    // Find all Events within the specific range of years (inclusive).
    //
    public Event [] findRange(int first, int last)
    {
	return null;
    }
}
