#include <iostream>
#include <memory>

import view.helper;

int main() {
    ViewHelper viewhelper;

    viewhelper.showSeparator('*', 50);
    viewhelper.showMenuTitle("ViewHelper测试程序");
    viewhelper.showError("发生一个错误");
    viewhelper.showSuccess("操作成功完成");
    viewhelper.showInfo("这是一条普通信息");
    viewhelper.waitForKeyPress();
    viewhelper.clearScreen();
    viewhelper.readInt("请输入您的年龄(默认20): ", 20);
    viewhelper.readDouble("请输入您的身高(米, 默认1.75): ", 1.75);
    viewhelper.readString("请输入您的姓名(默认张三): ", "张三");
    viewhelper.readPassword("请输入密码(不会显示): ");
    viewhelper.confirm("退出");

}