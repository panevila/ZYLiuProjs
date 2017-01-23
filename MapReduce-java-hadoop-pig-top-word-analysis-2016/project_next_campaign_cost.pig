-- TODO (A): Replace 'FIXME' to load the test_ad_data.txt file.
data = LOAD 'dualcore/ad_data[12].txt' AS 
(keyword:chararray,
campaign_id:chararray,
date:chararray,
time:chararray,
display_site:chararray,
was_clicked:int,
cpc:int,
country:chararray,
placement:chararray);

data1 = DISTINCT data;  --Remove same records
ordered = ORDER data1 BY cpc DESC;
HighestRecord = LIMIT ordered 1;
HighestCost = FOREACH HighestRecord GENERATE cpc;

--If expected total click is 50000;
result = FOREACH HighestCost GENERATE $0*50000;
DUMP result;

--If expected total click is 3 times the number of that of this compaign
data2 = FILTER data1 BY was_clicked == 1;
data3 = GROUP data2 BY was_clicked;
data4 = FOREACH data3 GENERATE COUNT(data2)*3 AS clicks; --Get the expected total click of next campagin (previous * 3)
ordered = ORDER data4 BY clicks DESC;
TargetClicks = LIMIT ordered 1;
data5 = CROSS TargetClicks, HighestCost;
result2 = FOREACH data5 GENERATE $0*$1;
DUMP result2;

