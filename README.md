# QuickTranslate
1. 一个在windows环境下运行的悬浮翻译软件，用户可以快捷的查询单词并且获取发音，通过右键点击可以进行截图操作。
2. 通过悬浮球作为服务端，管理端作为客户端建立进程间通信，进行数据的发送。
3. 使用SQLite本地数据库进行数据的存储。
4. 通过TTS引擎进行语音发音。
5. 使用多线程对界面的加载进行优化，提高响应速度。
6. 调用百度翻译API进行语言的翻译。
# 环境配置
windows下Qt Creator 11.0.2, MINGW6.7.0，通过QMake进行构建。
# 文件说明
# QuickTranslate
该文件夹是悬浮球的代码文件，入口文件为floatingball，通过信号槽机制进行界面的切换和信息的发送。
# QuickTranslateAdmin
该文件夹是管理端的代码文件。
# 演示视频
[![NetFlix on UWP](https://res.cloudinary.com/marcomontalbano/image/upload/v1587315555/video_to_markdown/images/youtube--2qqYywttue4-c05b58ac6eb4c4700831b2b3070cd403.jpg)]([https://youtu.be/2qqYywttue4 "NetFlix on UWP"](https://www.youtube.com/watch?v=ldtSAj3ci_U))
