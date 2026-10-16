# CURSE++ v0.2.1（日本語版）

**演算子がすべて嘘をつくC++20ヘッダーオンリーライブラリ。**
難読化プログラミングのコンテストや教材用です。実用システムへの投入は推奨しません。

## 使い始める

1. `include/cursepp/cursepp.hpp` をプロジェクトの `include/cursepp/` に置きます。
2. ソースコードに次の3行を書きます。

```cpp
#include <cursepp/cursepp.hpp>
using namespace cursepp;
using namespace cursepp::literals;
```

この状態で、たとえば以下の `main()` が動作します。

```cpp
int main() {
    Z _;
    ~(_[{3_q, 1_q}] << (!_, *_));
}
```

出力は `1`・`2`・`3` がそれぞれ改行付きで表示されます。

## 手動コンパイル

```sh
g++ -std=c++20 -Iinclude examples/fizzbuzz.cpp -o fizzbuzz
./fizzbuzz
```

## CMakeでビルド・テスト

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/cursepp_fizzbuzz
```

## 他プロジェクトのCMakeから利用

**方法A: ソースツリーを直接組み込む**

```cmake
add_subdirectory(external/cursepp-library)
target_link_libraries(myapp PRIVATE cursepp::cursepp)
```

**方法B: インストールしてから利用**

```sh
cmake -S . -B build -DCURSEPP_BUILD_EXAMPLES=OFF -DCURSEPP_BUILD_TESTS=OFF
cmake --install build --prefix "$PWD/local"
```

インストール先を `CMAKE_PREFIX_PATH` に指定したうえで、利用側のCMakeに以下を追記します。

```cmake
find_package(cursepp 0.2 CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE cursepp::cursepp)
```

## 対象型 `E` / `P` / `A` / `B` とは

演算子を本来と異なる意味にするための **CURSE++ 専用型** です。普通の `int` や `bool` の演算子は変わりません。

| 型 | 役割 | 例 |
| --- | --- | --- |
| `E` (*Expression*) | 遅延評価される **数値の式**。現在のループ番号を使って計算する | `3_q`、`_ + 1_q`、`++4_q` |
| `P` (*Predicate*) | 遅延評価される **条件式**。真偽値を返すが `bool` そのものではない | `(_ ^ 3_q) != 0_q` |
| `A` (*Action*) | 遅延実行される **命令**。数値や文字列の表示、改行などを表す | `*_`、`!_`、`(!_, *_)` |
| `B` (*Branches*) | `(条件, 命令)` の組を保持する **分岐リスト**。左から先に成立した分岐を実行する | `(p ->* a) \| (q ->* b)` |

`E` と `P` は `at(i)` で、`A` は `at(i, 出力先)` で評価・実行されます。通常は直接呼ばず、演算子で組み立てて最後に `~` または `run()` でまとめて実行します。

補助型として、`Z` は現在のループ番号（例：`Z _;`）、`R` はループ範囲、`K` は範囲と本体を組にしたプログラム、`W` は逆順文字列を表します。

例えば `(_ ^ 3_q) != 0_q` は、「剰余が0なら真」の `P` を生成します（`!=` は実際には等値比較）。これを `p ->* *_` とすると `B` ができ、`B | A` で不一致時の命令を追加できます。

## 演算子の仕様

| 見た目 | 動作 |
| --- | --- |
| `E + E` | 減算 |
| `E - E` | 乗算 |
| `E / E` | 加算 |
| `E ^ E` | 剰余 |
| `++E` / `--E` | -1 / +1した式の生成（元の値は変更しない） |
| `E != E` | 等しいかどうか |
| `P \|\| P` | AND判定（短絡なし） |
| `P ->* A` | 条件付き処理 |
| `B \| B` | 分岐を優先順位順に連結 |
| `B \| A` | 全部不一致だったときの処理 |
| `*E` / `*W` | 数値／逆順文字列を表示する遅延命令 |
| `!E` | 改行する遅延命令 |
| `A , A` | 右側の処理を先に実行 |
| `Z[{終点,始点}]` | ループ範囲を生成 |
| `R << A` | ループに処理を割り当て |
| `~K` | `std::cout` に向けて実行 |

`operator""_q` は数値と文字列の両方に定義されています。演算子群は `cursepp` 名前空間、リテラルは `cursepp::literals` 名前空間にあります。

## テスト等で出力先を変える

`~` は標準出力に表示しますが、通常の関数 `cursepp::run(プログラム, 出力ストリーム)` も用意しました。

```cpp
#include <sstream>
std::ostringstream capture;
Z _;
run(_[{3_q, 1_q}] << (!_, *_), capture);
// capture.str() は "1\n2\n3\n"
```

## 注意点

- 数値型は `long long` です。通常のC++と同様に符号付き整数オーバーフローには注意してください。
- 文字列は**バイト単位**で反転します。日本語などのUTF-8文字列を逆に書いても正常に復元できるとは限りません。
- C++本来の演算子の優先順位や文法は変更できません。
- オーバーロードされた `||` は組み込みの短絡評価を持ちません。
- どの演算子も対象型 `E` / `P` / `A` / `B` 等に対してのみ適用され、組み込み `int` の演算子は変更できません。

詳しいFizzBuzzの例は `examples/fizzbuzz.cpp` にあります。

## ライセンス

MIT License で公開します。全文は [`LICENSE`](LICENSE) にあります。再配布する場合は、著作権表示と許諾表示を残してください。

`LICENSE` の著作権者表記は仮に `CURSE++ contributors` としています。公開時には実際の著作権者名に変更してください。
