# Tank

现在可以分开写, 写完用```merge.py```合并一下就能用.

目前合并过的文件可以在botzone上正常操作, 不过文件巨大.

### 目录

```src/template```: 模板已经有的东西.

```src/rule```: ypy的代码.

```src/json```: json头文件直接放到project里了.

```src/bot```: Monte Carlo Tree Search的代码.

### 用法

**新增的代码不要忘了加到CMAKE列表里去**!

编译: ```./tool.sh make```.

合并: ```./tool.sh merge```.

代码格式化: ```./tool.sh format```.

清理: ```./tool.sh clean```.