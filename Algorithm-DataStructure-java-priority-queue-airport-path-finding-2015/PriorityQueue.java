import java.util.ArrayList;

//
// PRIORITYQUEUE.JAVA
// A priority queue class supporting sundry operations needed for
// Dijkstra's algorithm.
//

class PriorityQueue<T> {
    
	ArrayList<PQElement> PQElements = new ArrayList<PQElement>(); //Queue array
	//"PQElements" contains a T object, a ref. to handle and a key value
	//It is on the end of this file
	
	int m_nHeapSize;
    // constructor
    //
    public PriorityQueue()
    {
    	PQElements.add(null);
    	m_nHeapSize=0;
    }
    
    // Return true iff the queue is empty.
    //
    public boolean isEmpty()
    {
    	if(m_nHeapSize>0)
    		return false;
    	else
    		return true;
    }
    
    // Insert a pair (key, value) into the queue, and return
    // a Handle to this pair so that we can find it later in
    // constant time.
    //
    Handle insert(int key, T value)
    {
    	m_nHeapSize ++;
    	Handle hVertex = new Handle(m_nHeapSize);	//assign handle
    	PQElement tmpElement = new PQElement(key, value, hVertex);
    	PQElements.add(tmpElement);
    	int Size = PQElements.size();
    	int i = Size-1;
    	int iParent = i/2;
    	
    	//Maintain Min-Heap-Properties
    	while(i>1 && PQElements.get(iParent).GetKey()>PQElements.get(i).GetKey())
    	{
    		//SWAP A[i] and A[iParent]
    		tmpElement = PQElements.get(iParent);
    		PQElements.set(iParent, PQElements.get(i));
    		PQElements.set(i, tmpElement);
    		//Handle Operations
    		PQElements.get(i).GetHandle().SetIndex(i);
    		PQElements.get(iParent).GetHandle().SetIndex(iParent);
    		
    		i = iParent;
    		iParent = iParent/2;
    	}
    	return hVertex;
    }
    
    // Return the smallest key in the queue.
    //
    public int min()
    {
    	return PQElements.get(1).GetKey();
    }
    
    // Extract the (key, value) pair associated with the smallest
    // key in the queue and return its "value" object.
    //
    public T extractMin()
    {
    	if(m_nHeapSize<1)
    	{
    		return null;
    	}
    	T temp = PQElements.get(1).GetValue();
    	PQElements.get(1).GetHandle().SetIndex(-1); //Set to -1 means it has been extracted
    	//Remove
    	
    	PQElements.set(1, PQElements.get(m_nHeapSize)); //A[1]=A[Last]
    	PQElements.get(m_nHeapSize).GetHandle().SetIndex(1); //remove the last
    	
    	PQElements.remove(m_nHeapSize);
    	m_nHeapSize --;
    	
    	int i = 1;
    	//Heapify
    	while(2*i<=m_nHeapSize)	//while have child
    	{
    		int iLeft = 2*i;
    		int iRight = 2*i+1;
    		int iSmallest = i;
    		
    		//Let i swap with the smaller one of its child, if that one is smaller than i
    		if(iLeft<=m_nHeapSize && PQElements.get(iLeft).GetKey()<PQElements.get(i).GetKey())
    			iSmallest = iLeft;
    		else
    			iSmallest = i;
    		if(iRight<=m_nHeapSize && PQElements.get(iRight).GetKey()<PQElements.get(iSmallest).GetKey())
    			iSmallest = iRight;
    		
    		if(iSmallest != i)
    		{
    			//SWAP A[i] and A[iParent]
    			PQElement tmpElement = PQElements.get(iSmallest);
        		PQElements.set(iSmallest, PQElements.get(i));
        		PQElements.set(i, tmpElement);
        		//Handle Operations
        		PQElements.get(i).GetHandle().SetIndex(i);
        		PQElements.get(iSmallest).GetHandle().SetIndex(iSmallest);
        		i = iSmallest;		//Recursively Heapify(iSmallest)
    		}
    		else
    			break;
    	}

    	return temp;
    }
    
    
    // Look at the (key, value) pair referenced by Handle h.
    // If that pair is no longer in the queue, or its key
    // is <= newkey, do nothing and return false.  Otherwise,
    // replace "key" by "newkey", fixup the queue, and return
    // true.
    //
    public boolean decreaseKey(Handle h, int newkey)
    {
    	if(h.GetIndex() == -1)
    		return false;
    	
    	int iDecrease = h.GetIndex();
    	if(newkey < PQElements.get(iDecrease).GetKey() )
    	{
    		int i = iDecrease;
        	int iParent = i/2;
        	PQElement tmpElement = new PQElement();
        	PQElements.get(iDecrease).SetKey(newkey);
        	//Maintain Min-Heap-Properties
        	while(i>1 && PQElements.get(iParent).GetKey()>PQElements.get(i).GetKey())
        	{
        		//SWAP A[i] and A[iParent]
        		tmpElement = PQElements.get(iParent);
        		PQElements.set(iParent, PQElements.get(i));
        		PQElements.set(i, tmpElement);
        		//Hander Operations
        		PQElements.get(i).GetHandle().SetIndex(i);
        		PQElements.get(iParent).GetHandle().SetIndex(iParent);
        		
        		i = iParent;			//Recursively bubble up
        		iParent = iParent/2;
        	}
    		return true;
    	}
    	return false;
    }
    
    
    // Get the key of the (key, value) pair associated with a 
    // given Handle. (This result is undefined if the handle no longer
    // refers to a pair in the queue.)
    //
    public int handleGetKey(Handle h)
    {
    	int i = h.GetIndex();		//Get current position
    	if(i>m_nHeapSize)
    		return 0;
    	return PQElements.get(i).GetKey();
    }

    // Get the value object of the (key, value) pair associated with a 
    // given Handle. (This result is undefined if the handle no longer
    // refers to a pair in the queue.)
    //
    public T handleGetValue(Handle h)
    {
    	int i = h.GetIndex();
    	if(i>m_nHeapSize)
    		return null;
    	return PQElements.get(i).GetValue();
    }
    
    // Print every element of the queue in the order in which it appears
    // in the implementation (i.e. the array representing the heap).
    public String toString()
    {
    	String s = new String();
    	for(int i=1;i<=m_nHeapSize;i++)
    	{
    		PQElement tmpElement = PQElements.get(i);
    		s = s + "("+ tmpElement.GetKey()+"," + tmpElement.GetValue() + ")"+"\n";
    	}
    	return s;
    }
    
	private class PQElement {
    	public PQElement(){;}
    	
    	public PQElement(int key, T value, Handle hValue)
        {
    		m_nkey = key;		
    		m_gvalue = value;
    		m_hValue = hValue;
        }
    	
    	public int GetKey(){
    		return m_nkey;}
    	
    	public T GetValue(){
    		return m_gvalue;}
    	
    	public void SetKey(int key){
    		m_nkey = key;}
    	
    	public Handle GetHandle(){
    		return m_hValue;}
    	
    	private int m_nkey;			//Key Value
    	private T m_gvalue;			//The object
    	private Handle m_hValue;	//Associating handle
    }
}
