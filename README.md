# neticon
サーバーの稼働状態を Windows のタスクバーの通知領域上のアイコンで監視する為のツールです。

## バージョン採番ルール

### バージョン表記のフォーマット

`A.BB.CCC`

### メジャーバージョン番号(`A`)

明らかな非互換の変更が行われた際にインクリメント。
桁数は不定。

### マイナーバージョン番号(`BB`)

機能追加や上位互換と判断できる仕様変更が行われた際にインクリメント。
桁数は2桁固定。

### ビルド番号(`CCC`)

バグフィックスや仕様変更というほどでもない微細な修正が行われた際にインクリ
メント。
桁数は3桁固定。

### 細則

* 各番号は0始まりとする。
* 固定桁に足りない場合は先頭を0埋めする。
* 番号が固定桁で足りなくなった場合は、上位の番号をインクリメントする。
* 上位の番号がインクリメントされた場合、下位の番号は0にリセットする。
