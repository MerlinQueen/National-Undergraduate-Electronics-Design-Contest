＃-F 2019年全国电子设计大赛F题纸张计数显示装置，基于STM32F103和TI的FDC2214的软件代码
![Image text](https://github.com/MerlinQueen/National-Undergraduate-Electronics-Design-Contest/blob/master/image/logo.jpg)

以STM32F103C单片机结合电容传感器芯片FDC2214，设计并制作了一个纸张计数显示装置，利用亚克力板与铜板制作了测量装置。FDC2214是一个基于LC谐振电路原理的一个电容检测传感器芯片，在芯片每个检测通道的输入端连接一个电感和电容，组成一个完整的LC电路，被测电容传感器与LC电路连接，产生一个振荡频率，将振荡频率通过FDC2214进行处理，得到电容值传输至单片机计算，经处理后使用OLED显示屏进行显示
# 测量原理
测量原理：使采用测量两极板间容值，容值会随两极板距离变化而变化。极板距离会随纸张放入而增加。测量容值是通过检测两极板的电容大小。纸张的增加会使得板间距离增大，板间相对面积根据要求固定为50mm*50mm的纯铜板见图1，所以板间相对面积为一个定值，设计的夹具尽可以上下平行移动，由电容公式2，这时容量值只和板间距离有关。FDC2214 是基于 LC 谐振电路原理的一个电容检测传感器。在芯片每个检测通道的输入端连接一个电感和电容，组成LC电路，被测电容传感端与 LC 电路相连接，将产生一个振荡频率，传感器可计算出电容值

# 设计流程图
![Image text](https://github.com/MerlinQueen/National-Undergraduate-Electronics-Design-Contest/blob/master/image/liuchengtu.jpg)

# 主控电路与检测电路原理图

![Image text](https://github.com/MerlinQueen/National-Undergraduate-Electronics-Design-Contest/blob/master/image/FDC2214.png)
![Image text](https://github.com/MerlinQueen/National-Undergraduate-Electronics-Design-Contest/blob/master/image/yuanlitu.jpg)	

# 实物图
![Image text](https://github.com/MerlinQueen/National-Undergraduate-Electronics-Design-Contest/blob/master/image/shiwu.jpg)



# 测试数据
![Image text](https://github.com/MerlinQueen/National-Undergraduate-Electronics-Design-Contest/blob/master/image/data.jpg)
	
	
	
	
