# About

**ABANDONED!**
Masstreeの新しいrepoは、 https://github.com/Ogiwara-CostlierRain464/Masstree です。

Masstreeの正当性についての議論をまとめる

Tree構造なので、single thread, multi threadの
順で検証？


# Key sliceについて

Key sliceといっても、Key構造内のkey sliceと、Border node内のkey slice、
Interior node内のkey sliceがある

最初のは、1~8
二つ目は、0~8
三つ目は、どうでもいい

とりあえず、論文中の記述にしたがい、簡単にコピーが取れるようにatomicは使わずにalignasだけまずは使ってみる
と思ったが、よくわからないのでここら辺は質問する

場合によっては、GCC atomic builtinを型でラップして提供しよう

まずは普通のB+ treeを作ることを目標に


どこでmemory fenceが必要なのかも考える


まず、version内の情報 is_rootとか
を更新し忘れている。

B+ treeのルールの理解があやふや。
しっかり確認し、B+ tree部分を完成させる。
当然、BorderとInteriorの違いには注意する。
