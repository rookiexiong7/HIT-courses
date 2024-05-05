-- 创建数据库
CREATE DATABASE COMPANY;
USE COMPANY;

show variables like '%char%';
set character_set_client = 'utf8';
set character_set_connection = 'utf8';
set character_set_results = 'utf8';
set character_set_results = 'utf8';
set character_set_server = 'utf8';
set character_set_database = 'utf8';

-- 建立关系
CREATE TABLE DEPARTMENT (
    DNAME VARCHAR(25),  
    DNO CHAR(3) NOT NULL,
    MGRSSN CHAR(20),
    MGRSTARTDATE DATE, 
    Primary key (DNO)
)charset=utf8;

LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab1\\dbms_lab\\department.txt" INTO TABLE DEPARTMENT LINES TERMINATED BY '\r\n';

CREATE TABLE EMPLOYEE(
    ENAME VARCHAR(15),  
    ESSN CHAR(20) NOT NULL,
    ADDRESS VARCHAR(15),
    SALARY INTEGER, 
    SUPERSSN CHAR(20), 
    DNO char(3),
    Primary key (ESSN),
    foreign key (DNO) references DEPARTMENT(DNO)
)charset=utf8;

LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab1\\dbms_lab\\employee.txt" INTO TABLE EMPLOYEE LINES TERMINATED BY '\r\n';

CREATE TABLE PROJECT (
    PNAME VARCHAR(10),  
    PNO VARCHAR(3) NOT NULL,
    PLOCATION CHAR(10),
    DNO char(3),
    Primary key (PNO),
    foreign key (DNO) references DEPARTMENT(DNO)
)charset=utf8;

LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab1\\dbms_lab\\project.txt" INTO TABLE PROJECT LINES TERMINATED BY '\r\n';

CREATE TABLE WORKS_ON (
    ESSN CHAR(20) NOT NULL, 
    PNO VARCHAR(3) NOT NULL,
    HOURS INT,
    Primary key (ESSN,PNO),
    foreign key (ESSN) references EMPLOYEE(ESSN),
    foreign key (PNO) references project(PNO)
)charset=utf8;

LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab1\\dbms_lab\\works_on.txt" INTO TABLE WORKS_ON LINES TERMINATED BY '\r\n';

-- 1: 参加了项目名为“SQL Project”的员工名字

SELECT ENAME
FROM EMPLOYEE, WORKS_ON, PROJECT
WHERE EMPLOYEE.ESSN=WORKS_ON.ESSN AND WORKS_ON.PNO=project.PNO AND PNAME='SQL';

-- 2：在“Research Department（研发部）”工作且工资低于3000元的员工名字和地址；
SELECT ENAME, ADDRESS
FROM EMPLOYEE, DEPARTMENT
WHERE EMPLOYEE.DNO=DEPARTMENT.DNO AND DNAME='研发部' AND SALARY<3000;

-- 3：没有参加项目编号为P1的项目的员工姓名；
SELECT ENAME
FROM EMPLOYEE
WHERE ESSN NOT IN (SELECT ESSN FROM WORKS_ON WHERE PNO='P1');

-- 4：由张红领导的工作人员的姓名和所在部门的名字；
SELECT ENAME, DNAME
FROM EMPLOYEE, DEPARTMENT
WHERE EMPLOYEE.DNO=DEPARTMENT.DNO AND SUPERSSN IN 
    (SELECT ESSN FROM EMPLOYEE WHERE ENAME='张红');

-- 5：至少参加了项目编号为P1和P2的项目的员工号；
SELECT ESSN
FROM WORKS_ON
WHERE PNO='P1' AND ESSN IN (SELECT ESSN FROM WORKS_ON WHERE PNO='P2');

-- 6：参加了全部项目的员工号码和姓名；
SELECT ESSN, ENAME
FROM EMPLOYEE
WHERE NOT EXISTS 
    (SELECT PNO FROM PROJECT WHERE NOT EXISTS 
        (SELECT PNO FROM WORKS_ON WHERE WORKS_ON.ESSN=EMPLOYEE.ESSN AND WORKS_ON.PNO=PROJECT.PNO));

-- 7：员工平均工资低于3000元的部门名称；
SELECT DNAME
FROM DEPARTMENT
WHERE DNO IN (SELECT DNO FROM EMPLOYEE GROUP BY DNO HAVING AVG(SALARY)<3000);

-- 8：至少参与了3个项目且工作总时间不超过8小时的员工名字；
SELECT ENAME
FROM EMPLOYEE
WHERE ESSN IN (SELECT ESSN FROM WORKS_ON GROUP BY ESSN HAVING COUNT(PNO)>=3 AND SUM(HOURS)<=8);

-- 9：每个部门的员工小时平均工资；
SELECT DNO, AVG(SALARY/HOURS)
FROM EMPLOYEE, WORKS_ON
WHERE EMPLOYEE.ESSN=WORKS_ON.ESSN
GROUP BY DNO;

