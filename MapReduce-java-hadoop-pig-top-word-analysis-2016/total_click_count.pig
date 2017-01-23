-- TODO (A): Replace 'FIXME' to load the test_ad_data.txt file.
data = LOAD 'dualcore/ad_data[12]' AS 
(campaign_id:chararray,
date:chararray,
time:chararray,
keyword:chararray,
display_site:chararray,
placement:chararray,
was_clicked:int,
cpc:int);

data1 = DISTINCT data;
data2 = FILTER data1 BY was_clicked == 1;

data3 = GROUP data2 BY was_clicked;
data4 = FOREACH data3 GENERATE COUNT(data2);

ordered = ORDER data4 BY $0 DESC;

data5 = LIMIT ordered 1;
DUMP data5;

