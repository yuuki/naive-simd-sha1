# naive-simd-sha1


## 構成ファイル

sha1.h: SSE-SHA1のAPI宣言.
sha1.c: SSE-SHA1のAPI実装.
sha1test.c: SSE-SHA1のAPIのテスト用コード.SHA-1を1回実行する.
sha1time.c: SSE-SHA1の計測用コード（シングルスレッド）. SHA-1を複数回実行する.
sha1mp.c: SSE-SHA1の計測用コード（マルチスレッド）.

## コンパイル

- テスト用コードをコンパイルする場合は
$ make
実行ファイルは sha1

- 計測用コード（シングルスレッド）をコンパイルする場合は
$ make time
実行ファイルは sha1time

- 計測用コード（マルチスレッド）をコンパイルする場合は
$ make omp
実行ファイルは sha1mp

## 使い方
いずれの実行ファイルもファイルを引数にとる.

```shell
$ sha1 testfile
$ sha1time testfile
$ sha1mp testfile
```

