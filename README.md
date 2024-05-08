# QuickTranslate
一个在windows环境下运行的悬浮翻译软件，用户可以快捷的查询单词并且获取发音，通过右键点击可以进行截图操作。
通过悬浮球作为服务端，管理端作为客户端建立进程间通信，进行数据的发送。
使用SQLite本地数据库进行数据的存储。
# 环境配置
windows下Qt Creator 11.0.2, MINGW6.7.0，通过QMake进行构建。
# 文件说明
#### QuickTranslate
该文件夹是悬浮球的代码文件，入口文件为floatingball，通过信号槽机制进行界面的切换和信息的发送。
#### QuickTranslateAdmin
该文件夹是管理端的代码文件。

