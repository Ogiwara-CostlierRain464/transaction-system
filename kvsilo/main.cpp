#include <iostream>
#include <cmath>
#include "primary_tree.h"

using KVSilo::PrimaryTree;

int main(){
  PrimaryTree tree;

  for(size_t i = 0; i <= 10000; ++i){
    int r = rand();
    tree.root = tree.insert(r, r);
  }

  //printf("%d", tree.find(1, nullptr)->value);

  tree.printTree();

  // さて、どのようなクラス構成にしようか
  // 上のPrimary Treeはなかなか扱いやすいいい例だね
  // クライアントからのbegin~endまでのシーケンスを、
  /**
   * まずはWorkerに投げる(Worker数はCPU数と同じに設定しよう)
   *
   * まあ空きが発生したWorkerに投げる
   * あとはtransaction objectの中身を見て、
   * よしなに実行する。
   *
   * なるべく値を見て、それに対応して柔軟に行えるようにはしたいよね
   * まあその前にまずはRead set, Write setのメンテだけするのも…
   * いやだめそうだ
   *
   * Workerにどうやって投げる？
   * Schedulingの問題だね、つまり、どのThreadに処理を割り当てるか？？？
   *
   * Thread間のmessagingがなかなか面倒な予感がする
   *
   * まずmainがDBにクエリを投げる
   * mainはfenceでそれをまつ
   *
   * DBは適当なWorkerに投げる(ここがボトルネックにならないように)
   * Workerでの処理のあと、Loggerに投げる
   *
   * Loggerは、書き込みをし、DEを更新して、最後にcommitを返す
   *
   * とりあえず、各threadはwhileしてyieldでもして待つか
   *
   * あと問題なのは、クエリをどのようにして実行するのか？
   *
   * これはそんなに難しくないのか？実行するだけだから
   *
   *
   */
}