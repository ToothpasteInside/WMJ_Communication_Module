# WMJ_Communication_Module
TX2 CAN communication module for RM2019 by WMJ


Usage:

```c++
TransportCan(std::string)
```
构造函数，参数为can设备名。

```c++
bool send_a_frame(Buffer&)
```
发送一帧数据，参数为一个uint8_t  vector，第一位为自定义的包头，与can id对应，其后为数据。

```c++
Buffer read_a_frame(int)
```
读取一帧数据，参数为所需包的包头。
