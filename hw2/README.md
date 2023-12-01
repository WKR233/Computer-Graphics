# Hand

2100017727 王柯然
图形学课程第二次小作业代码
这个程序分为两种模式，一是自由视角模式，二是平滑转换模式，两者通过按键Q进行转换
默认为第一种模式，在这种模式下，鼠标不可见，且可以通过移动鼠标转换摄影机的角度
也可以通过鼠标滚轮调整摄影机的偏转角度（roll）
还可以通过W、A、S、D四个按键进行摄影机位置的移动，通过T、G进行竖直的位移
可以通过ctrl -和ctrl =来进行视角的缩放，范围是1°到60°
最后，可以通过R一键复原位置到初始情况
切换到第二种模式后，鼠标可见，不可通过移动鼠标转换摄影机角度
可以在左边调整A、B观察点的参数，程序给出了一组默认参数用于观察平滑移动的过程
这一平滑移动过程采用了四元数的球面线性插值，一次移动时间总计为3秒，且提供从A到B和从B到A两种移动模式
A、B的位置参数可以通过滑块调节，调节完毕之后直接按下按钮即可看到效果
当然，这两种模式都兼容三种手势的设置，即通过“1”，“2”，“3”种按键获得三种手势，可以通过按“0”复原

> 注：Assimp是经过裁剪的版本，只支持FBX文件的导入