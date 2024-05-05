use school_management_system;
-- 创建学生成绩视图(学号，学生姓名，课程名字，成绩)
create view S_course_view
as
select S_course.Student_ID,student.Student_NAME,course.course_name,S_course.grade
from S_course,student,course
where S_course.Student_ID=student.Student_ID and S_course.course_ID=course.course_ID
group by S_course.course_ID,S_course.Student_ID,grade
order by S_course.course_ID ASC,grade DESC;


-- 学生的基本信息视图(学号，学生，年龄，班级，系，辅导员，学生联系方式):
CREATE VIEW S_basic_info_view AS
SELECT
    Student_ID,
    Student_NAME,
    TIMESTAMPDIFF(YEAR, student.bithday, CURDATE()) - (DATE_FORMAT(CURDATE(), '%m%d') < DATE_FORMAT(student.bithday, '%m%d')) AS age,
    Class_ID,
    Dname,
    telephone,
    Employee_name AS mentor_name
FROM
    student
    NATURAL LEFT OUTER JOIN s_class
    NATURAL LEFT OUTER JOIN class
    NATURAL LEFT OUTER JOIN student_mentor
    NATURAL LEFT OUTER JOIN department
GROUP BY
    Student_ID,
    Student_NAME,
    age,
    Class_ID,
    Dname,
    telephone,
    mentor_name
ORDER BY
    Class_ID ASC,
    Student_ID ASC;


-- 教师的基本信息视图（教师职工号，姓名，讲授课程，职称，研究方向，所属实验室，联系方式）:
CREATE VIEW t_basic_info_view AS
SELECT
    teacher_ID,
    teacher_name,
    course_name,
    Title,
    ResearchDirection,
    laboratory_NAME,
    Ttelephone
FROM
    teacher
    NATURAL LEFT OUTER JOIN teach
    NATURAL LEFT OUTER JOIN course
    NATURAL LEFT OUTER JOIN work
    NATURAL LEFT OUTER JOIN laboratory
GROUP BY
    teacher_ID,
    teacher_name,
    course_name,
    Title,
    ResearchDirection,
    laboratory_NAME,
    Ttelephone
ORDER BY
    laboratory_NAME ASC,
    ResearchDirection ASC;

-- 系的基本信息视图（系标识号，系名，系主任，系主任联系方式）
CREATE VIEW D_basic_info_view AS
SELECT
    Department_ID,
    Dname,
    LName,
    Ltelephone
FROM
    department
    NATURAL LEFT OUTER JOIN leader
GROUP BY
    Department_ID,
    Dname,
    LName,
    Ltelephone
ORDER BY
    Department_ID ASC;

-- 对系名建立索引；
CREATE INDEX Dname_index ON department(Dname ASC);

-- 对s_course的学分进行索引
CREATE INDEX credits_index ON course(credits ASC);

-- 对student的出生日期进行索引
CREATE INDEX birthday_index ON student(bithday ASC);

-- 展示视图
SELECT * FROM S_course_view;
SELECT * FROM S_basic_info_view;
SELECT * FROM t_basic_info_view;
SELECT * FROM D_basic_info_view;

-- 展示索引
SHOW INDEX FROM department;
SHOW INDEX FROM course;
SHOW INDEX FROM student;

-- 按索引查询
SELECT * FROM course WHERE credits >= 2 ORDER BY credits ASC;

-- 插入、删除操作（体现关系表的完整性约束，插入空值、重复值时需给予提示或警告）

-- 正常插入
INSERT INTO student (student_id, student_name, bithday, SGender, telephone)
VALUES ('123344', 'John Doe', '2023/01/15', 'Male', '987-5432');
-- 插入student_dname为空值的元组
INSERT INTO student (student_id, bithday, SGender, telephone)
VALUES ('123343','1992-05-20', 'Female', '87654321');
-- 重复插入
INSERT INTO student (student_id, student_name, bithday, SGender, telephone)
VALUES ('123344', 'Jane Smith', '1995-03-10', 'Female', '76543210');

-- 正常删除
DELETE FROM student WHERE student_id = '123344';
-- 删除空值
DELETE FROM laboratory WHERE laboratory_addr = 'E33'
-- 删除不满足完整性约束
DELETE FROM teacher WHERE teacher_ID = '1806'

-- 连接查询（查看某个班的学生的选课门数）
SELECT s.Student_ID, s.Student_NAME, COUNT(s_course.course_ID) AS CourseCount
FROM student s
natural left outer join s_class 
natural left outer join	s_course
WHERE s_class.Class_ID = '20210101'
GROUP BY s.Student_ID, s.Student_NAME;

-- 嵌套查询（查询辅导员“XXX”负责的学生）
SELECT s.Student_ID, s.Student_NAME
FROM student s natural left outer join s_class 
WHERE class_ID IN (SELECT class_ID FROM student_mentor WHERE Employee_name = 'Faker')

-- 分组查询（查看学生的学号，姓名，班级，院系）
SELECT s.Student_id, s.Student_name, sc.Class_ID, d.Dname
FROM s_class sc
natural join student s 
natural join class c 
natural join department d 
GROUP BY s.Student_id, sc.Class_ID, d.Dname, s.Student_name;

-- having语句
SELECT s.Student_ID, COUNT(sc.course_ID) AS CourseCount
FROM student s
natural join s_course sc
GROUP BY s.Student_ID
HAVING CourseCount >= 2;

-- 触发器（学生选了某课并且成绩>60的情况下，总学分=总学分+某课的学分）
ALTER TABLE student
ADD COLUMN total_credits INT DEFAULT 0;

CREATE TRIGGER update_total_credits
AFTER INSERT ON s_course
FOR EACH ROW
BEGIN
    DECLARE selected_credits INT;
    SELECT credits INTO selected_credits
    FROM course
    WHERE course_id = NEW.course_id;
    IF NEW.grade > 60 THEN
        UPDATE student
        SET total_credits = total_credits + selected_credits
        WHERE student_id = NEW.student_id;
    END IF;
END;

-- 插入一门课的成绩
INSERT INTO s_course (student_id, course_id, grade)
VALUES ('123456', '1005', 80);

INSERT INTO s_course (student_id, course_id, grade)
VALUES ('123456', '1001', 59);