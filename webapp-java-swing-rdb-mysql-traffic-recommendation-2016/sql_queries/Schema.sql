CREATE TABLE CITY (
    City_code CHAR(3) NOT NULL,
    PRIMARY KEY (City_Code)
);

CREATE TABLE AIRPORT (
    Airport_code CHAR(3) NOT NULL,
    ACity_code CHAR(3) NOT NULL,
    PRIMARY KEY (Airport_code),
    FOREIGN KEY (ACity_code)
        REFERENCES CITY (City_Code)
);

CREATE TABLE STATION (
    Station_code CHAR(3) NOT NULL,
    SCity_code CHAR(3) NOT NULL,
    PRIMARY KEY (Station_code),
    FOREIGN KEY (SCity_code)
        REFERENCES CITY (City_Code)
);

CREATE TABLE TRAIN (
    TNumber VARCHAR(10) NOT NULL,
    Sch_Depart TIME NOT NULL,
    Sch_Arrive TIME NOT NULL,
    Depart_Scode CHAR(3) NOT NULL,
    Arrive_Scode CHAR(3) NOT NULL,
    Crossed_Date INT NOT NULL,
    PRIMARY KEY (TNumber),
    FOREIGN KEY (Depart_Scode)
        REFERENCES STATION (Station_code),
    FOREIGN KEY (Arrive_Scode)
        REFERENCES STATION (Station_code)
);

CREATE TABLE FLIGHT (
    FNumber VARCHAR(10) NOT NULL,
    Sch_Depart TIME NOT NULL,
    Sch_Arrive TIME NOT NULL,
    Depart_Acode CHAR(3) NOT NULL,
    Arrive_Acode CHAR(3) NOT NULL,
    Crossed_Date INT NOT NULL,
    PRIMARY KEY (FNumber),
    FOREIGN KEY (Depart_Acode)
        REFERENCES AIRPORT (Airport_code),
    FOREIGN KEY (Arrive_Acode)
        REFERENCES AIRPORT (Airport_code)
);

CREATE TABLE TRAIN_OPERATION (
    TNumber_o VARCHAR(10) NOT NULL,
    Dep_Date DATE NOT NULL,
    Arr_Date DATE NOT NULL,
    Act_Depart TIME NOT NULL,
    Act_Arrive TIME NOT NULL,
    PRIMARY KEY (TNumber_o , Dep_Date),
    FOREIGN KEY (TNumber_o)
        REFERENCES TRAIN (TNumber)
);

CREATE TABLE FLIGHT_OPERATION (
    FNumber_o VARCHAR(10) NOT NULL,
    Dep_Date DATE NOT NULL,
    Arr_Date DATE NOT NULL,
    Act_Depart TIME NOT NULL,
    Act_Arrive TIME NOT NULL,
    Delay_main_reason VARCHAR(10),
    PRIMARY KEY (FNumber_o , Dep_Date),
    FOREIGN KEY (FNumber_o)
        REFERENCES FLIGHT (FNumber)
);

CREATE TABLE WEATHER (
    WDate DATE NOT NULL,
    WTime TIME NOT NULL,
    WCity_code CHAR(3) NOT NULL,
    Temperature VARCHAR(10) NOT NULL,
    Humidity INT NOT NULL,
    Visibility DECIMAL(3 , 1 ) NOT NULL,
    Wind_speed DECIMAL(3 , 1 ) NOT NULL,
    Wind_direction VARCHAR(5) NOT NULL,
    Precipitation DECIMAL(4 , 2 ),
    Conditions VARCHAR(50),
    PRIMARY KEY (WDate , WTime , WCity_code),
    FOREIGN KEY (WCity_code)
        REFERENCES CITY (City_code)
);
