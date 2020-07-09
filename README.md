# QianTangSolver
6x6 qiantang solver


破解6x6浅塘游戏

“浅塘”这个游戏也不知道有没有更正式一些的名字，暂时就先这么叫

程序不保证没有bug，如果有bug欢迎指出

6x6浅塘一共只有270亿局面，所以可以暴力枚举

结论：所有可通关的局面，均可在51步内通关。


使用方法：

每个局面都有一个64位整数ID和它对应

可能用到的函数：

Pos getPosID()手动以某个格式输入局面，返回值为局面ID

从右到左（！！！不是从左往右），从上到下为正方向，依次输入各行各列的滑块类型和位置（懒得细讲了，自己看代码悟应该也不难）

例如， 0xx0xx就是2020，xx0000就是00002，0xxx00是0030

int solve(Pos targetPos) 解一个局面，并输出步骤

void printPos(Pos pos) 输入局面id，显示这个id对应的局面

int main_solve(Pos targetPos) 可作为main函数，输入局面id，输出最短解决路径

int main_print() 可作为main函数，输入局面id，显示这个id对应的局面

int main_enumerate() 可作为main函数，枚举6x6浅塘所有局面，不少于25步的局面写入txt文件，运行一次大约两三天，电脑内存不要少于32G（其实稍微改一点就能省很多内存，但我懒得改，因为我的电脑内存够大）



有且仅有3个局面需要51步，这3个局面仅仅是一两个滑块的位置有区别，滑块的组合是相同的。下图是其中一个局面。

lkaah

lkj hg

l jccg

dddi g

  viee
  
rrvff
