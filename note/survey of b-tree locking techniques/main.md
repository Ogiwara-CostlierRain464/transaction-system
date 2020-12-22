# About
A survey of B-tree locking techniquesのまとめ

# 指針
- 後から何度でも見返せるように、プレゼン資料では失われがちな、最低限の「つなぎ」となる文章を入れる
- 具体例を十分にあげる
- B-treeの並行性制御とRange Queryに注目し、Recoverの話題は扱わない


# 3.1 B-Treeの二種類の「ロック」

まずは、よく混同しやすいlockとlatchの違いについてまとめておく

![](fig2.jpg)

lockは、
- pageやB-treeのkeyなどのデータベースのコンテンツを保護するために、
- shared, exclusiveなどのモードを使って 
- 複数のトランザクションからの操作を分ける。

ARIES/KVLでは論理ロックと呼んでいる。

deadlockは、
- wait for graphのチェック等によって検知し、
- transaction abort等によって解決される

lockの情報はlock managerのhash tableに保存される
lock処理は通常、高価なものとなる。これは、中央で管理されるhash tableに複数のスレッドからのアクセスが
発生し、キャッシュミスが多発するからである。lockの対象となるデータは、メモリ上になかったり、あるいはデータベース上
にないものも扱うので、latchと違いlock tableの導入が必要となる。

latchは
- buffer pool中のB-tree pageやbuffer poolの management tableなど、 メモリ中のデータ構造を保護するために、
- read, writeなどのモードを使って 
- 複数のスレッドからの操作を分ける

ARIES/KVLでは物理ロックと呼ぶ

dead latchは、
- 発生しないように十分注意してプログラムすることによって回避

latchの情報は、保護の対象となるデータ構造に埋め込む

latch処理は通常、安価なものとなる。これは、保護対象のデータ構造の中にlatchの情報が埋め込まれるため、
中央データ構造での管理が発生しないため、比較的キャッシュミスが発生しにくい。


# Recovery時のlockとlatchの違い

飛ばす。ARIES/KVL参照


# 3.3 Lock free 
定義が曖昧なlock-freeについておさらいする。
全体をMutexで保護し、並行処理を可能にしたデータ構造はlock-freeとは言えない。複数のスレッドが同時にそのデータ構造
を操作することができないからである。

通常のMutexは、スレッドの動作を一時停止させるようなOSのシステムコールを用いて実装している。
このように、スレッドの動作を一時停止させるようなOSのシステムコールをblocking callと呼ぶ。

Mutexはblocking callを用いずに実装することが可能である。例えば、以下のコードのようにtest and setで実装することができる

```c++
#include <atomic>

class spinlock_mutex{
  std::atomic_flag flag;
  
public:
  spinlock_mutex(): flag(ATOMIC_FLAG_INIT)
  {};
  
  void lock(){
    while(flag.test_and_set());
  }
  
  void unlock(){
    flag.clear();
  }
};
```

ここでは、test and setが成功するまでwhile loopをすることによりロックの確保を実現している。このようなlockの
実装をspin lockという。

spin lockにより、blockingでないMutexを実装することができた。しかしながら、このMutexを用いてもlock-freeには
なりえない。複数のスレッドからの操作が許容されていないことに変わりはないからである。

"C++ Concurrency in Action"では、lock-freeとその周辺の用語について以下のようにまとめている。

- Obstruction-Free: もしデータ構造を操作する他の全てのthreadが止まっているならば、一つのスレッドが特定ステップ数以下で処理を完了できるデータ構造
- Lock-Free: もしデータ構造を複数のスレッドが操作しているならば、そのうちの一つのスレッドが特定ステップ数以下で処理を完了できるデータ構造
- Wait-Free: データ構造を操作している全てのスレッドが、その処理を特定ステップ数以下で完了できるデータ構造

ここで重要なのは、まず「特定のステップ数以下」というのはソースコードから予測できる程度のステップ数以下で処理が完了することを表す。
lockの取得に1000回失敗したりなど、並行処理を実行するたびにステップ数に大きなばらつきがあってはいけない。

もう一つ重要なのは、lock(データベースの文脈ではlatch)の実装に依るのではなく、実行形態がどうなるか、つまり実行単位(スレッド)
とステップ数にのみ注目して定義付けがなされているということだ。blocking callを実装に使っているか否かは定義とはあまり関係がない。

deadlockやlivelock、starvationについても同様に、実行単位とステップ数を用いて定義できる。

形式的な定義にはお目に書かれたことはない。時相論理とか必要になりそう？

# 4.B-Treeの物理構造の保護

並行でアクセスできるようなB-Treeを設計する場合、主に以下のような4点の問題があげられる

1. メモリ中のページは、あるthreadが読んでいる間に他のthreadから内容を変更されてはいけない
2. あるスレッドがページ間のポインタを走査している間に、他のスレッドがそのポインタを無効化してはならない
3. ポインタの走査は親子間だけではなく兄弟間でも起こるため、deadlockが発生しないように十分に注意して設計する
4. 子ノードがオーバーフローして親ノードへのキーの挿入が求められる。最悪ケースでは、rootノードのsplitが発生する。

これらの問題に対する解決策をいくつかあげる。
## lock coupling

2つめの問題は
latch coupling, hand-over-hand lockingとも呼ばれる。


## B-link tree


