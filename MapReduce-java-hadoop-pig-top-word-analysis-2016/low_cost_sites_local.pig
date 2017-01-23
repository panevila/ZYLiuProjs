-- TODO (A): Replace 'FIXME' to load the test_ad_data.txt file.
data = LOAD '/home/training/training_materials/analyst/exercises/analyze_ads/test_ad_data.txt' AS 
(campaign_id:chararray,
date:chararray,
time:chararray,
keyword:chararray,
display_site:chararray,
placement:chararray,
was_clicked:int,
cpc:int);

-- TODO (B): Include only records where was_clicked has a value of 1
data1 = DISTINCT data;
data2 = FILTER data1 BY was_clicked == 1;

-- TODO (C): Group the data by the appropriate field
data3 = GROUP data2 BY display_site;
data4 = FOREACH data3 GENERATE group,SUM(data2.cpc);

-- TODO (E): Sort that new relation by cost (ascending)
ordered = ORDER data4 BY $1;

-- TODO (F): Display just the first three records to the screen
data5 = LIMIT ordered 4;
DUMP data5;

