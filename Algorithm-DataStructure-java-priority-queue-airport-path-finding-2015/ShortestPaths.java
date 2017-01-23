import java.util.ArrayList;

//
// SHORTESTPATHS.JAVA
// Compute shortest paths in a graph.
//
// Your constructor should compute the actual shortest paths and
// maintain all the information needed to reconstruct them.  The
// returnPath() function should use this information to return the
// appropriate path of edge ID's from the start to the given end.
//
// Note that the start and end ID's should be mapped to vertices using
// the graph's get() function.
//
// You can ignore the input and startTime arguments to the constructor
// unless you are doing the extra credit.
//
class ShortestPaths {
    
    //
    // constructor
    //
	
	//Shortest path container, 
	//each "PathCell" contains a parent pointer and a path .
	//It is on the bottom of this file.
	private ArrayList<PathCell> ShortestPaths = new ArrayList<PathCell>();
	private int iStartID = -1;	//Init the start vertex index.
	
    public ShortestPaths(Multigraph G, int startId, 
			 Input input, int startTime) 
    {
	// your code here
    	
    	//Init
    	iStartID = startId;
    	int nVertices =  G.nVertices();	//Get the number of vertices
    	
    	//Vertex Queue
    	PriorityQueue<Vertex> qVertexQueue = new PriorityQueue<Vertex>();
    	
    	Handle hVertHandles[] = new Handle[nVertices]; //Store the handles
    	
    	for(int i=0;i<nVertices;i++)
    	{
    		hVertHandles[i] = qVertexQueue.insert(Integer.MAX_VALUE,G.get(i)); //Set all to +Inf
    		ShortestPaths.add(new PathCell(-1,-1)); //"-1" just means "no parent assigned"
    	}
    	ShortestPaths.set(startId, new PathCell(-2,-1)); //A special cell with parent"-2" for the start point
    	qVertexQueue.decreaseKey(hVertHandles[startId], 0); //Set the distance of start point to 0
    	
    	//Start Dijkstra's Algo.
    	while(!qVertexQueue.isEmpty())
    	{
    		int crtDistance = qVertexQueue.min();	//Check the next element, check it it is unconnected
    		if(crtDistance==Integer.MAX_VALUE)
    			break;								//If met a unchecked vertex, there is no other reachable
    		
    		Vertex tmpVertex = qVertexQueue.extractMin();		//Get vertex u
    		int tmpVertexID = tmpVertex.id();					//Get u's id
    		Vertex.EdgeIterator CheckIterator = tmpVertex.adj();	//check its edges
    		
    		while(CheckIterator.hasNext())		//Visit neighbors
    		{
    			Edge tmpEdge = CheckIterator.next();	//temporarily store the edge (for weights)
    			Vertex vNeighbor = tmpEdge.to();	//Get v
    			int vNeighborID = vNeighbor.id();   //Get v's ID
    			
    			if(hVertHandles[vNeighborID].GetIndex() == -1) 
    				continue;				//ignore vertices that has been fully worked on (Extracted)
    			
    			Handle hNeighbor = hVertHandles[vNeighbor.id()]; //Get the handle to a neighbor
    			
    			int newDistance = 0; 
    			if(crtDistance == Integer.MAX_VALUE)
    				newDistance = tmpEdge.weight();		//if it's the first update ,set it to weight
    			else
    				newDistance = crtDistance + tmpEdge.weight(); //else add the distance
    					
    			if(qVertexQueue.decreaseKey( hNeighbor, newDistance))  //decreaseKey
    				ShortestPaths.set(vNeighborID, new PathCell(tmpVertexID,tmpEdge.id())); //v.parent = u
    		}
    	}
    }
    
    //
    // returnPath()
    // Return an array containing a list of edge ID's forming
    // a shortest path from the start vertex to the specified
    // end vertex.
    //
    public int [] returnPath(int endId) 
    { 
	// your code here
    	if(iStartID == endId)				//if s = t return empty
    	{
			int empty[] = new int [0];
			return empty;
    	}
    	
    	//An array used to store the backward path
    	ArrayList<Integer> PathEdges = new ArrayList<Integer>();
    	
    	int y = endId;
    	PathEdges.add(ShortestPaths.get(endId).GetEdgeID()); 
    	
    	//from endId, trace backward until we reach the startpoint
    	while(y!=-1 && ShortestPaths.get(y).GetParent() != -2) //"-2" means this is a start point
    	{
    		y = ShortestPaths.get(y).GetParent();
    		int tmpEdge = ShortestPaths.get(y).GetEdgeID();
    		PathEdges.add(tmpEdge);
    	}
    	
    	int n = PathEdges.size()-1;
    	
    	//An array used to store the forward path
    	int retpath[] = new int [n];
    	for(int i=0; i<n; i++)
    		retpath[i]=PathEdges.get(n-i-1);
		return retpath;
    }
    
	private class PathCell {
    	public PathCell(int parentID, int edgeID)
        {
    		m_parentID = parentID;
    		m_edgeID = edgeID;
        }
    	
    	public int GetParent(){
    		return m_parentID;}
    	
    	public int GetEdgeID(){
    		return m_edgeID;}
    	
    	private int m_edgeID;
    	private int m_parentID;
    }
  
}
