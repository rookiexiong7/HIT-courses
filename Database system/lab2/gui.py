# -*- coding: utf-8 -*-
import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QIcon, QPixmap
import pymysql

conn = pymysql.connect(
    host="localhost",
    port=3306,
    user="root",
    passwd="20030216",
    db="school_management_system",
    charset='utf8'
)

cur = conn.cursor()

class StockDialog(QDialog):
    def __init__(self, parent=None):
        super(StockDialog, self).__init__(parent)

        self.setWindowTitle('School Info Management System')
        self.setWindowIcon(QIcon('pic/stupid.gif'))

        mainSplitter = QSplitter(Qt.Horizontal)
        mainSplitter.setOpaqueResize(True)

        listWidget = QListWidget(mainSplitter)
        listWidget.insertItem(0, self.tr("ER-Diagram"))
        listWidget.insertItem(1, self.tr("View"))
        listWidget.insertItem(2, self.tr("Insert"))
        listWidget.insertItem(3, self.tr("Delete"))
        listWidget.insertItem(4, self.tr("Query"))

        frame = QFrame(mainSplitter)
        stack = QStackedWidget()
        stack.setFrameStyle(QFrame.Panel | QFrame.Raised)

        show_ER = ShowER()
        view = View()
        insert = Insert()
        delete = Delete()
        query = Query()
        stack.addWidget(show_ER)
        stack.addWidget(view)
        stack.addWidget(insert)
        stack.addWidget(delete)
        stack.addWidget(query)

        closePushButton = QPushButton(self.tr("退出"))

        buttonLayout = QHBoxLayout()
        buttonLayout.addStretch(1)
        buttonLayout.addWidget(closePushButton)

        mainLayout = QVBoxLayout(frame)
        mainLayout.setContentsMargins(10, 10, 10, 10)
        mainLayout.setSpacing(6)
        mainLayout.addWidget(stack)
        mainLayout.addLayout(buttonLayout)

        listWidget.currentRowChanged.connect(stack.setCurrentIndex)
        closePushButton.clicked.connect(self.close)

        layout = QHBoxLayout(self)
        layout.addWidget(mainSplitter)
        self.setLayout(layout)


class ShowER(QWidget):
    def __init__(self, parent=None):
        super(ShowER, self).__init__(parent)
        pixmap = QPixmap("./E-R图.png")
        self.label = QLabel(self)
        self.label.setPixmap(pixmap)
        layout = QGridLayout(self)
        layout.addWidget(self.label, 0, 0)

    def execute(self):
        print("您已按下执行按钮。")


class View(QWidget):
    def __init__(self, parent=None):
        super(View, self).__init__(parent)

        label1 = QLabel(self.tr("视图名："))
        label2 = QLabel(self.tr("要查询属性："))
        label3 = QLabel(self.tr("限制："))
        button = QPushButton(self.tr("执行"))
        label4 = QLabel(self.tr("执行结果："))

        self.viewComboBox = QComboBox()
        self.attrEdit = QLineEdit()
        self.limitEdit = QLineEdit()
        self.resultEdit = QTextEdit()

        self.viewComboBox.addItem("d_basic_info_view")
        self.viewComboBox.addItem("s_basic_info_view")
        self.viewComboBox.addItem("s_course_view")
        self.viewComboBox.addItem("t_basic_info_view")

        layout = QGridLayout(self)
        layout.addWidget(label1, 0, 0)
        layout.addWidget(self.viewComboBox, 0, 1)
        layout.addWidget(label2, 1, 0)
        layout.addWidget(self.attrEdit, 1, 1)
        layout.addWidget(label3, 2, 0)
        layout.addWidget(self.limitEdit, 2, 1)
        layout.addWidget(button, 3, 0)
        layout.addWidget(label4, 4, 0)
        layout.addWidget(self.resultEdit, 4, 1)

        button.clicked.connect(self.execute)

    def execute(self):
        view = self.viewComboBox.currentText()
        attr = self.attrEdit.text()
        query = "select " + attr + " from " + view
        limit = self.limitEdit.text()
        if len(str(limit)) > 0:
            query += " where " + limit

        try:
            items = cur.execute(query)
            info = cur.fetchmany(items)
            conn.commit()
            print(info)
            result = ""
            # 获取列的最大宽度
            col_widths = [max(len(str(item)) for item in col) for col in zip(*info)]
            # 格式化数据行
            for row in info:
                result += " | ".join([str(item).ljust(width) for item, width in zip(row, col_widths)])
                result += '\n'

            if len(result) > 0:
                self.resultEdit.setText(result)
            else:
                self.resultEdit.setText("无法执行\"" + str(query) + "\"")

        except Exception as e:
            conn.rollback()
            error = e.args[1]
            self.resultEdit.setText(f"查询视图失败: {error}")

class Insert(QWidget):
    def __init__(self, parent=None):
        super(Insert, self).__init__(parent)

        label1 = QLabel(self.tr("表名："))
        label2 = QLabel(self.tr("值："))
        button = QPushButton(self.tr("执行"))
        label3 = QLabel(self.tr("执行结果："))

        self.tableEdit = QLineEdit()
        self.attrEdit = QLineEdit()
        self.resultEdit = QTextEdit()

        layout = QGridLayout(self)
        layout.addWidget(label1, 0, 0)
        layout.addWidget(self.tableEdit, 0, 1)
        layout.addWidget(label2, 1, 0)
        layout.addWidget(self.attrEdit, 1, 1)
        layout.addWidget(button, 2, 0)
        layout.addWidget(label3, 3, 0)
        layout.addWidget(self.resultEdit, 3, 1)

        button.clicked.connect(self.execute)

    def execute(self):
        table = self.tableEdit.text()
        value = self.attrEdit.text()
        query = "insert into " + str(table) + " values(" + str(value) + ")"

        try:
            cur.execute(query)
            conn.commit()
            self.resultEdit.setText(f"成功将值插入到{table}中")
        except pymysql.Error as e:
            conn.rollback()  # 回滚事务，取消插入操作
            error = e.args[1]
            self.resultEdit.setText(f"插入失败: {error}")


class Delete(QWidget):
    def __init__(self, parent=None):
        super(Delete, self).__init__(parent)

        label1 = QLabel(self.tr("表名："))
        label3 = QLabel(self.tr("限制："))
        button = QPushButton(self.tr("执行"))
        label4 = QLabel(self.tr("执行结果："))

        self.tableEdit = QLineEdit()
        self.attrEdit = QLineEdit()
        self.limitEdit = QLineEdit()
        self.resultEdit = QTextEdit()

        layout = QGridLayout(self)
        layout.addWidget(label1, 0, 0)
        layout.addWidget(self.tableEdit, 0, 1)
        layout.addWidget(label3, 1, 0)
        layout.addWidget(self.limitEdit, 1, 1)
        layout.addWidget(button, 2, 0)
        layout.addWidget(label4, 3, 0)
        layout.addWidget(self.resultEdit, 3, 1)

        button.clicked.connect(self.execute)

    def execute(self):
        table = self.tableEdit.text()
        attr = self.attrEdit.text()
        query = "delete from " + table
        limit = self.limitEdit.text()
        if len(limit) > 0:
            query += " where " + limit

        try:
            deleted_rows = cur.execute(query)
            conn.commit()
            if deleted_rows > 0:
                self.resultEdit.setText(f"成功删除 {table} 上满足条件 {limit} 的 {deleted_rows} 行数据")
            else:
                self.resultEdit.setText(f"未找到匹配的数据，没有进行删除操作")

        except Exception as e:
            # 回滚事务
            conn.rollback()
            error = e.args[1]
            self.resultEdit.setText(f"删除错误: {error}")


class Query(QWidget):
    def __init__(self, parent=None):
        super(Query, self).__init__(parent)

        label1 = QLabel(self.tr("表名："))
        label2 = QLabel(self.tr("要查询属性值："))
        label3 = QLabel(self.tr("限制："))
        label4 = QLabel(self.tr("分组："))
        label5 = QLabel(self.tr("HAVING："))
        button = QPushButton(self.tr("执行"))
        label6 = QLabel(self.tr("执行结果："))

        self.tableEdit = QLineEdit()
        self.attrEdit = QLineEdit()
        self.limitEdit = QLineEdit()
        self.groupEdit = QLineEdit()
        self.resultEdit = QTextEdit()
        self.havingEdit = QLineEdit()


        layout = QGridLayout(self)
        layout.addWidget(label1, 1, 0)
        layout.addWidget(self.tableEdit, 1, 1)
        layout.addWidget(label2, 0, 0)
        layout.addWidget(self.attrEdit, 0, 1)
        layout.addWidget(label3, 2, 0)
        layout.addWidget(self.limitEdit, 2, 1)
        layout.addWidget(label4, 3, 0)
        layout.addWidget(self.groupEdit, 3, 1)
        layout.addWidget(label5, 4, 0)
        layout.addWidget(self.havingEdit, 4, 1)
        layout.addWidget(button, 5, 0)
        layout.addWidget(label6, 6, 0)
        layout.addWidget(self.resultEdit, 6, 1)

        button.clicked.connect(self.execute)

    def execute(self):
        table = self.tableEdit.text()
        attr = self.attrEdit.text()
        query = "select " + attr + " from " + table
        limit = self.limitEdit.text()
        if len(limit) > 0:
            query += " where " + limit
        
        group = self.groupEdit.text()
        if len(group) > 0:
            query += " group by " + group

        having = self.havingEdit.text()
        if len(having) > 0:
            query += " having " + having

        print(query)    

        try:
            items = cur.execute(query)
            info = cur.fetchmany(items)
            conn.commit()
            print(info)
            result = ""
            # 获取列的最大宽度
            col_widths = [max(len(str(item)) for item in col) for col in zip(*info)]
            # 格式化数据行
            for row in info:
                result += " | ".join([str(item).ljust(width) for item, width in zip(row, col_widths)])
                result += '\n'

            if len(result) > 0:
                self.resultEdit.setText(result)
            else:
                self.resultEdit.setText("无法执行\"" + str(query) + "\"")

        except Exception as e:
            conn.rollback()
            error = e.args[1]
            self.resultEdit.setText(f"查询失败: {error}")


if __name__ == '__main__':
    app = QApplication(sys.argv)
    gui = StockDialog()
    gui.show()
    sys.exit(app.exec_())

    conn.commit()
    cur.close()
    conn.close()
