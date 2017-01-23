-- Load only the ad_data1 and ad_data2 directories
data = LOAD 'dualcore/ad_data[12]' AS (campaign_id:chararray,
             date:chararray, time:chararray,
             keyword:chararray, display_site:chararray,
             placement:chararray, was_clicked:int, cpc:int);

grouped = GROUP data BY display_site;

by_site = FOREACH grouped {
  -- Include only records where the ad was clicked
	clicked = FILTER data BY was_clicked > 0;
	--Count number of click
	nClicked = COUNT(clicked);
	--Count rate of click
	ctr = (nClicked*1.0)/COUNT(data);
	--Return results
	GENERATE group AS display_site, ctr AS CTR;
}
--Problem (b)
ordered = ORDER by_site BY CTR;
problem_b = LIMIT ordered 3;
DUMP problem_b;

--Problem (c)
grouped2 = GROUP data BY keyword;
by_keyword = FOREACH grouped2 {
  -- Include only records where the ad was clicked
	clicked = FILTER data BY was_clicked > 0;
	--Count number of click
	nClicked = COUNT(clicked);
	--Count rate of click
	ctr = (nClicked*1.0)/COUNT(data);
	--Return results
	GENERATE group AS display_keyword, ctr AS CTR;
}
ordered2 = ORDER by_keyword BY CTR DESC;
problem_c = LIMIT ordered2 3;
DUMP problem_c;

