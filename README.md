1. rt-thread 设置
  ![image](https://github.com/user-attachments/assets/3576c78d-530f-4384-a96c-32aec4a5396b)
2. at32手册查找定时器对应的channel, 需要定时器具备输入捕获 --直接使用work beanch确认
3. 通过work beanch 生成初始化代码，开启中断, 也可以开启DMA, 定时器频率可手动调整,滤波等参数
  ![image](https://github.com/user-attachments/assets/f1a809c1-0a1a-4876-b899-cb07391c7bf4)
4. 循迹模块, 测试中断捕获
  模块DO引脚连接定时器channel引脚输入 
