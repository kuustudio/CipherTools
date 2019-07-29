插件开发说明：
1.插件以MFC dll开发
2.stdafx.h中添加头文件引用
3.添加资源字符串IDR_MAINFRAME,为子窗口标题
4.添加menu菜单项
5.添加插件类，如DesToolPlugin类，继承CPlugin类
6.创建panel的对话框，边框为none,并添加对话框类
7.建议使用ChildFrm和ChildView来创建界面（复制过来）
8.添加菜单事件响应，代码类似
9.修改项目生成目录到plugins