# makeexport
make .def and .h files from Dynamic Link Library

## 说明
本程序可由带导出函数的dll生成C++头文件(.h)以及.def文件
要生成.lib文件，需要用到vs的link.exe，所以你需要安装vs来生成lib

由.def创建.lib命令：
link /LIB /DEF:C:\a.def /OUT:C:\a.lib
