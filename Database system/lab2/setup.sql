-- change coding mode

-- Create database
CREATE DATABASE school_management_system;
USE school_management_system;

-- Create course table
CREATE TABLE course (
    course_ID CHAR(4) NOT NULL,
    course_name CHAR(15) NOT NULL,
    credits INT NOT NULL,
    PRIMARY KEY (course_ID)
);
LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab2\\data\\course.txt" INTO TABLE course;


-- Create teacher table
CREATE TABLE teacher (
    teacher_ID CHAR(4) NOT NULL,
    teacher_name CHAR(15) NOT NULL,
    Title CHAR(15),
    ResearchDirection CHAR(15),
    Ttelephone CHAR(8),
    PRIMARY KEY (teacher_ID)
);
LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab2\\data\\teacher.txt" INTO TABLE teacher;

-- Create teach table
CREATE TABLE teach (
    course_ID CHAR(4) NOT NULL,
    teacher_ID CHAR(4) NOT NULL,
    PRIMARY KEY (course_ID, teacher_ID),
    FOREIGN KEY (course_ID) REFERENCES course(course_ID),
    FOREIGN KEY (teacher_ID) REFERENCES teacher(teacher_ID)
);
LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab2\\data\\teach.txt" INTO TABLE teach;


-- Create student table
CREATE TABLE student (
    Student_ID CHAR(6) NOT NULL,
    Student_NAME CHAR(20) NOT NULL,
    bithday DATE,
    SGender varchar(6),
    telephone CHAR(8) NOT NULL,
    PRIMARY KEY (Student_ID)
);
LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab2\\data\\student.txt" INTO TABLE student;

-- Create laboratory table
CREATE TABLE laboratory (
    laboratory_ID CHAR(3) NOT NULL,
    laboratory_NAME CHAR(20) NOT NULL,
    laboratory_addr CHAR(3),
    PRIMARY KEY (laboratory_ID)
);

LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab2\\data\\laboratory.txt" INTO TABLE laboratory;


-- Create work table
CREATE TABLE work (
    teacher_ID CHAR(4) NOT NULL,
    laboratory_ID CHAR(3) NOT NULL,
    PRIMARY KEY (teacher_ID, laboratory_ID),
    FOREIGN KEY (teacher_ID) REFERENCES teacher(teacher_ID),
    FOREIGN KEY (laboratory_ID) REFERENCES laboratory(laboratory_ID)
);

LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab2\\data\\work.txt" INTO TABLE work;

-- Create leader table
CREATE TABLE leader (
    leaderID CHAR(4) NOT NULL,
    LName VARCHAR(15) NOT NULL,
    Appointment_Date CHAR(15),
    Ltelephone CHAR(8),
    L_officce_addr CHAR(3),
    PRIMARY KEY (leaderID)
);
LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab2\\data\\leader.txt" INTO TABLE leader;


-- Create department table
CREATE TABLE department (
    Department_ID CHAR(3) NOT NULL,
    Dname CHAR(30) NOT NULL,
    leaderID CHAR(4),
    PRIMARY KEY (Department_ID),
    FOREIGN KEY (leaderID) REFERENCES leader(leaderID)
);
LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab2\\data\\department.txt" INTO TABLE department;


-- Create class table
CREATE TABLE class (
    class_ID CHAR(8) NOT NULL,
    student_num INT,
    Department_ID CHAR(3),
    PRIMARY KEY (class_ID),
    FOREIGN KEY (Department_ID) REFERENCES department(Department_ID)
);
LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab2\\data\\class.txt" INTO TABLE class;


-- Create student_mentor table
CREATE TABLE student_mentor (
    Employee_ID CHAR(4) NOT NULL,
    Employee_name VARCHAR(15) NOT NULL,
    Etelephone CHAR(8),
    E_Office_Hours CHAR(15),
    E_Office_addr CHAR(3),
    class_ID CHAR(8),
    PRIMARY KEY (Employee_ID),
    FOREIGN KEY (class_ID) REFERENCES class(class_ID)
);

LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab2\\data\\student_mentor.txt" INTO TABLE student_mentor;


-- Create S_Class table
CREATE TABLE S_Class (
    Student_ID CHAR(6) NOT NULL,
    Class_ID CHAR(8) NOT NULL,
    PRIMARY KEY (Student_ID, Class_ID),
    FOREIGN KEY (Student_ID) REFERENCES student(Student_ID),
    FOREIGN KEY (Class_ID) REFERENCES class(class_ID)
);

LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab2\\data\\S_Class.txt" INTO TABLE S_Class;


-- Create S_course table
CREATE TABLE S_course (
    Student_ID CHAR(6) NOT NULL,
    course_ID CHAR(4) NOT NULL,
    grade INT,
    PRIMARY KEY (Student_ID, course_ID),
    FOREIGN KEY (Student_ID) REFERENCES student(Student_ID),
    FOREIGN KEY (course_ID) REFERENCES course(course_ID)
);

LOAD DATA LOCAL INFILE "D:\\study\\databaselab\\lab2\\data\\S_course.txt" INTO TABLE S_course;


create view S_course_view
as
select S_course.Student_ID,student.Student_NAME,course.course_name,S_course.grade
from S_course,student,course
where S_course.Student_ID=student.Student_ID and S_course.course_ID=course.course_ID
group by S_course.course_ID,S_course.Student_ID,grade
order by S_course.course_ID ASC,grade DESC;
select * from S_course_view;