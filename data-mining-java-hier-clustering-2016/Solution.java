import java.io.*;
import java.util.*;

public class Solution {

	public static void main(String[] args) throws IOException {
		
		BufferedReader reader = new BufferedReader(new FileReader("./data/H1.csv"));
		int nFeatures = 200;
		
		List<double[]> dataList = new ArrayList<double[]>();
		
		String tmpLine = "";
		for(; (tmpLine = reader.readLine()) != null; ){
			//jump to data
			// store data
			String[] tokens = tmpLine.split(",");
			double[] dataVec = new double[nFeatures];
			for(int p=0;p<nFeatures;p++){
				try{
					dataVec[p] = Double.parseDouble(tokens[p]);
				}
				catch(NumberFormatException e){
					dataVec[p] = 0;		//missing data handling
				}
			}
			dataList.add(dataVec);
		}
		reader.close();

		int[] labels = new int[dataList.size()];
		reader = new BufferedReader(new FileReader("./data/all_labels.csv"));
		for(int i=0; (tmpLine = reader.readLine()) != null; i++){
			labels[i] = Integer.parseInt(tmpLine.trim());
		}
		reader.close();
		
		//kMeans
		int[] assign = divisiveClustering(dataList, 1, 1, "pcc");
		for(int i=0;i<assign.length;i++){
			System.out.println(assign[i]);
		}
		double crtRate = correctRate(assign, labels);
		System.out.println(crtRate);
	}
	
	static Random r = new Random(new Date().getTime());
	
	static int[] divisiveClustering(List<double[]> dataList, int levelRem, int classALabel, String distFunc){
		int nAllData = dataList.size();
		int[] assign;
		if(levelRem <=0){
			assign = new int[nAllData];
			Arrays.fill(assign, classALabel);
			return assign;
		}
		int labelDiffVal = (int) Math.pow(2,levelRem-1);
		//k means
		KMeanRetClass rs = basicKMeans(dataList, 2, 1000, 1, true, distFunc);
		assign = rs.clusterAssign;
		//create sub dataset
		List<double[]> subDataListA = new ArrayList<>();
		List<double[]> subDataListB = new ArrayList<>();
		for(int i=0;i<nAllData;i++){
			if(assign[i]==0)
				subDataListA.add(dataList.get(i));
			else
				subDataListB.add(dataList.get(i));
		}
		int[] assignA = divisiveClustering(subDataListA, levelRem-1, classALabel, distFunc);
		int[] assignB = divisiveClustering(subDataListB, levelRem-1, classALabel + labelDiffVal, distFunc);
		int[] assignRet = new int[nAllData];
		//get back the data
		int iA = 0;
		int iB = 0;
		for(int i=0;i<nAllData;i++){
			if(assign[i]==0){
				assignRet[i] = assignA[iA];
				iA ++;
			}
			else{
				assignRet[i] = assignB[iB];
				iB ++;
			}
		}
		return assignRet;
	}
	
	static double correctRate(int[] assign, int[] trueLabel){
		double rs = -1;
		int len = assign.length;
		if(len != trueLabel.length) return rs;
		int count = 0;
		for(int i=0;i<len;i++)
			if(assign[i] == trueLabel[i]) count++;
		rs = ((double)count)/len;
		if(rs<0.5) rs = 1-rs;
		return rs;
	}
	
	//kMeans Clustering
	static KMeanRetClass basicKMeans(List<double[]> dataList, 
			int nClusters, 
			int maxIter, 
			double randRangeRate, 
			boolean isNormalize, 
			String distFunc){
		
		int nData = dataList.size();
		if(nData == 0) return null;
		int nFeatures = dataList.get(0).length;
		int loopsRem = maxIter;
		
		List<double[]> centroidList = new ArrayList<double[]>();
		List<List<double[]>> clusterDataList = new ArrayList<>();
		
		double[] maxVec = new double[nFeatures];
		double[] minVec = new double[nFeatures];
		Arrays.fill(maxVec, Double.MIN_VALUE);
		Arrays.fill(minVec, Double.MAX_VALUE);
		
		int[] clusterAssign = new int[nData];
		int[] lastAssign = new int[nData];
		Arrays.fill(lastAssign,-1);
		

		double tmpDouble = 0;
		int tmpInt;
		//get min max
		for(int i=0;i<nData;i++){
			double[] tmpVec = dataList.get(i);
			for(int k=0;k<nFeatures;k++){
				maxVec[k] = Math.max(tmpVec[k], maxVec[k]);
				minVec[k] = Math.min(tmpVec[k], minVec[k]);
			}
		}
		if(isNormalize){
			dataList = normalizeData(dataList,maxVec,minVec);	
			Arrays.fill(maxVec, Double.MIN_VALUE);
			Arrays.fill(minVec, Double.MAX_VALUE);
			for(int i=0;i<nData;i++){
				double[] tmpVec = dataList.get(i);
				for(int k=0;k<nFeatures;k++){
					maxVec[k] = Math.max(tmpVec[k], maxVec[k]);
					minVec[k] = Math.min(tmpVec[k], minVec[k]);
				}
			}
		}
		//generate centroids
		for(int j=0;j<nClusters;j++){
			double[] tmpVec = genRandVec(minVec,maxVec,randRangeRate);
			centroidList.add(tmpVec);
		}
		//main loop
		while(loopsRem>0){
			//calculate distance and assign cluster
			for(int i=0;i<nData;i++){
				double minDist = Double.MAX_VALUE;
				for(int j=0;j<nClusters;j++){
					tmpDouble = getDist(dataList.get(i),centroidList.get(j), distFunc);
					if(tmpDouble < minDist){
						minDist = tmpDouble;
						clusterAssign[i] = j;
					}
				}
			}
			//get cluster data lists
			clusterDataList = new ArrayList<>();
			for(int j=0;j<nClusters;j++){
				List<double[]> newDataList = new ArrayList<>();
				clusterDataList.add(newDataList);
			}
			for(int i=0;i<nData;i++){
				tmpInt = clusterAssign[i];
				clusterDataList.get(tmpInt).add(dataList.get(i));
			}
			//recalculate centroid
			for(int j=0;j<nClusters;j++){
				if(clusterDataList.get(j).size()==0) continue;
				double[] tmpDoubleArr = getAvgVec(clusterDataList.get(j));
				centroidList.set(j, tmpDoubleArr);
			}
			
			//check convergence
			boolean isConverged = true;
			for(int i=0;i<nData;i++){
				int tmpA = clusterAssign[i];
				int tmpB = lastAssign[i];
				if(tmpA != tmpB){
					isConverged = false;
				}
				lastAssign[i] = clusterAssign[i];
			}
			if(isConverged == true){
				break;
			}
			loopsRem --;
		}
		double agreeRate = 0;
		
		return new KMeanRetClass(clusterAssign, centroidList, clusterDataList, agreeRate);
	}
	
	static class KMeanRetClass{
		public int[] clusterAssign;
		public List<double[]> centroidList;
		public List<List<double[]>> clusterDataList;
		public double agreeRate;
		KMeanRetClass(int[] clusterAssign, List<double[]> centroidList, List<List<double[]>> clusterDataList, double agreeRate){
			this.clusterAssign = clusterAssign;
			this.centroidList = centroidList;
			this.clusterDataList = clusterDataList;
			this.agreeRate = agreeRate;
		}
	}
	
	static double[] evalClustering(List<List<double[]>> clusterDataList, List<double[]> centroidList, int[] clusterAssign, int indexA, int indexB, String distFunc){
		
		List<double[]> clusterA = clusterDataList.get(indexA);
		List<double[]> clusterB = clusterDataList.get(indexB);
		int nDataA = clusterA.size();
		int nDataB = clusterB.size();
		double[] rs = new double[4];
		if(nDataA == 0 || nDataB == 0) return rs;
		
		double singleLink = Double.MAX_VALUE;
		double completeLink = Double.MIN_VALUE;
		double average = 0;
		double centroid = 0;
		
		double tmpDist = 0;
		
		for(int iA=0;iA<nDataA;iA++){
			for(int iB=0;iB<nDataB;iB++){
				tmpDist = getDist(clusterA.get(iA), clusterB.get(iB), distFunc);
				if(tmpDist > completeLink) completeLink = tmpDist;
				if(tmpDist < singleLink) singleLink = tmpDist;
				average += singleLink;
			}
		}
		average = average / (nDataA*nDataB);
		centroid = getDist(centroidList.get(indexA),centroidList.get(indexB), distFunc);
		
		rs[0] = singleLink;
		rs[1] = completeLink;
		rs[2] = average;
		rs[3] = centroid;
		
		return rs;
	}
	
	static double[] genRandVec(double[] minVec, double[] maxVec, double randRangeRate){
		int len = maxVec.length;
		if(len != minVec.length){
			return null;
		}
		double[] rs = new double[len];
		for(int k=0;k<len;k++){

			double randDouble = r.nextDouble();
			rs[k] = minVec[k] * randRangeRate + (maxVec[k] - minVec[k]) * randDouble * randRangeRate;
		}
		return rs;
	}
	
	static double[] getAvgVec(List<double[]> vecList){
		int nData = vecList.size();
		if(nData==0) return null;
		int dim = vecList.get(0).length;
		double[] rs = new double[dim];
		for(int i=0;i<nData;i++){
			double[] dataVec = vecList.get(i);
			for(int k=0;k<dim;k++){
				rs[k] += dataVec[k];
			}
		}
		for(int k=0;k<dim;k++){
			rs[k] = rs[k]/nData;
		}
		return rs;
	}
	
	static double getDist(double[] vec1, double[] vec2, String distFunc){
		double rs = 0;
		if(distFunc.equals("euDist"))
			rs = euDist(vec1,vec2);
		else if(distFunc.equals("dotProdDist"))
			rs = dotProdDist(vec1,vec2);
		else if(distFunc.equals("pcc"))
			rs = pearsonCorrelationCoefficient(vec1,vec2);
//		else if(distFunc.equals("cosineDist"))
//			rs = dotProdDist(vec1,vec1);
		return rs;
	}
	
	static double euDist(double[] vec1, double[] vec2){
		int len = vec1.length;
		if(len != vec2.length) return -1;
		double rs = 0;
		for(int k=0;k<len;k++){
			rs += Math.pow(vec1[k]-vec2[k], 2);
		}
		rs = Math.sqrt(rs);
		return rs;
	}
	
	static double dotProdDist(double[] vec1, double[] vec2){
		int len = vec1.length;
		if(len != vec2.length) return -1;
		double rs = 0;
		for(int k=0;k<len;k++){
			rs += vec1[k]*vec2[k];
		}
		return -rs;
	}
	
	static double pearsonCorrelationCoefficient(double[] vec1, double[] vec2){
		int len = vec1.length;
		if(len != vec2.length) return -1;
		double rs = 0;
		double avg1 = 0;
		double avg2 = 0;
		for(int k=0;k<len;k++){
			avg1 += vec1[k];
			avg2 += vec2[k];
		}
		avg1/=len;
		avg2/=len;
		double s1=0;
		double s2=0;
		double s3=0;
		for(int i=0;i<len;i++){
			s1+=(vec1[i]-avg1)*(vec2[i]-avg2);
			s2+=(vec1[i]-avg1)*(vec1[i]-avg1);
			s3+=(vec2[i]-avg2)*(vec2[i]-avg2);
		}
		rs = s1/(Math.sqrt(s2)*Math.sqrt(s3));
		return 2-rs;		//bigger is closer
	}
	
	static List<double[]> normalizeData(List<double[]> dataList, double[] maxVec, double[] minVec){
		int nData = dataList.size();
		int dim = dataList.get(0).length;
		List<double[]> rs = new ArrayList<>();
		for(int i=0;i<nData;i++){
			double[] tmpVec = new double[dim];
			for(int k=0;k<dim;k++){
				tmpVec[k] = (dataList.get(i)[k]-minVec[k])/(maxVec[k]-minVec[k]);
			}
			rs.add(tmpVec);
		}
		return rs;
	}
}
