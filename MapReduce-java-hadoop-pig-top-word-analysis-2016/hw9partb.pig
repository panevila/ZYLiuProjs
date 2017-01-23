data = LOAD 'user/hw9/RoundedAvg.txt' AS 
(userID:chararray,
rating:double);

data1 = GROUP data BY rating;
data2 = FOREACH data1 GENERATE group AS rating, COUNT(data) AS times;

ordered = ORDER data2 BY rating;

DUMP ordered;
