/*-----------------------------------------------------------------------------
 *
 *  convert.c - 文字コード変換モジュール
 *
 *    Copyright (c) 2003, Ryo.Sugahara / Algorithm Collection
 *                                        All rights reserved
 *
 *    本モジュールはWebページ「Algorithm Collection」に掲載されたものを流用し
 *    再構成したものです。
 *
 *    Algorithm Collection
 *      http://alfin.mine.utsunomiya-u.ac.jp/~niy/algo/index.html
 *
 *    This program is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU General Public License
 *    as published by the Free Software Foundation; either version 2
 *    of the License, or (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307
 *
 *  Revision history:
 *    16-Apr-2003  初版
 *
 *----------------------------------------------------------------------------*/

/*
文字コード種別の判定

関数名

whatKanji  文字コードの種別を判定する

形式

int whatKanji(unsigned char *str);

引数

str  （入力）文字列

関数値

漢字の種別を表す値
・引数strの先頭文字が半角カナの場合、関数値は1
・引数strの先頭2文字がシフトJISコードの場合、関数値は2
・引数strの先頭2文字がEUC半角カナコードの場合、関数値は4
・引数strの先頭2文字がEUC全角コードの場合、関数値は8
・引数strの先頭3文字が新JIS(X0208-1983)シフトコードの場合、
　関数値は16
・引数strの先頭3文字が旧JIS(X0208-1978)シフトコードの場合、
　関数値は32
・引数strの先頭3文字がJISローマ字(X0201)シフトコードの場合、
　関数値は64
・引数strの先頭3文字がASCII シフトコードの場合、関数値は128
・上のケースが複数個同時に起きる場合は、関数値はそれらの論理和
・上のいずれでもない場合は、関数値は0

注意事項

用例（whatKanji-test.c）
    whatKanji("漢字");

説明
    半角カナコードは 0xa0 - 0xdf の間を使う。

    シフトJISコードは第1バイトとして 0x81 - 0x9f までと 0xe0 - 0xfc まで、第2バイトとして 0x40 - 0xfc まで（0x7fを除く）を使う。この ため、シフトJISは上記の半角コードと混在できる。しかし、制御文字 として使われる ＼（バックスラッシュまたは円記号）などが第2バイト の領域に入っているために問題が生じることがある。

    EUCコードは第1、第2バイトとも 0xa1 - 0xfe までであるが、半角の カナ文字については、0x8e を半角カナ文字の前につけ、2バイトで表現 する。

    一方、JISコードは7ビット系漢字コードであるため、つぎのようなシフト コードを使って、文字セットの切り替えを行い、かな漢字を表現する。

    　　　シフトコード一覧
    文字セット シフトコード
    新JIS X0208-1983 ESC $ B
    旧JIS X0208-1978 ESC $ @
    JISローマ字 X0201 ESC ( J
    ASCII ESC ( B
*/

int whatKanji(unsigned char *str)
{
    int val = 0;
    unsigned char b1, b2, b3;

    b1 = *str++;
    b2 = *str++;
    b3 = *str;
    if (b1 == 0x1b) {
        if (b2 == '$' && b3 == 'B') return 16;
        if (b2 == '$' && b3 == '@') return 32;
        if (b2 == '(' && b3 == 'J') return 64;
        if (b2 == '(' && b3 == 'B') return 128;
        return 0;
    }
    if ( b1 >= 0xa0 && b1 <= 0xdf) val |= 1;
    if ((b1 >= 0x81 && b1 <= 0x9f ||
         b1 >= 0xe0 && b1 <= 0xfc) && 
        (b2 >= 0x40 && b2 <= 0xfc && b2 != 0x7f)) val |= 2;
    if (b1 == 0x8e && (b2 >= 0xa0 && b2 <= 0xdf)) val |= 4; 
    if ((b1 >= 0xa1 && b1 <= 0xfe) &&
        (b2 >= 0xa1 && b1 <= 0xfe)) val |= 8;

    return val;
}


/*
半角カナを全角カナに変換する

関数名

hankaku2zen  半角カナを全角カナに変換する

形式

unsigned int hankaku2zen(int hankaku);

引数

hankaku  半角カナコード（0xA0-0xDF）

関数値

対応する全角カナJISコード。変換できないときは0。

注意事項
    半角カナコードの範囲は0xA0から0xDF。

用例（hankaku2zen-test.c）
    hankaku2zen(0xb1);
*/
unsigned int hankaku2zen(int hankaku)
{
    static unsigned int z[64] = {
        0x2121,0x2123,0x2156,0x2157,0x2122,0x2126,0x2572,0x2521,
        0x2523,0x2525,0x2527,0x2529,0x2563,0x2565,0x2567,0x2543,
        0x213c,0x2522,0x2524,0x2526,0x2528,0x252a,0x252b,0x252d,
        0x252f,0x2531,0x2533,0x2535,0x2537,0x2539,0x253b,0x253d,
        0x253f,0x2541,0x2544,0x2546,0x2548,0x254a,0x254b,0x254c,
        0x254d,0x254e,0x254f,0x2552,0x2555,0x2558,0x255b,0x255e,
        0x255f,0x2560,0x2561,0x2562,0x2564,0x2566,0x2568,0x2569,
        0x256a,0x256b,0x256c,0x256d,0x256f,0x2573,0x212b,0x212c };

    if (hankaku < 0xa0 || hankaku > 0xdf) return 0;
    return z[hankaku - 0xa0];
}

/*
半角カナを全角カナに変換する（濁音対応版）

関数名

han2zen  半角カナを全角カナに変換する（濁音対応版）

形式

int han2zen(unsigned int *zenkaku, unsigned char *str);

引数

zenkaku  （出力）全角カナJISコード
str      （入力）半角カナコード（0xA0-0xDF）を含めた文字列

関数値

変換結果についての付加情報
・引数strの先頭文字が半角カナでない場合、関数値は0
・引数strの先頭文字が半角清音カナである（つまり先頭の
　2文字が濁音・半濁音カナの組合せでない）場合、関数値は1
・引数strの先頭2文字が濁音・半濁音カナの組合せである場合、
　関数値は2

注意事項
    濁音・半濁音の場合は2文字分の半角カナが1文字分の全角カナに 変換される

用例（han2zen-test.c）
    unsigned int zenkaku;
    han2zen(&zenkaku, "\xc0\xde");

説明
    半角カナは使わないことにしよう。このことから、全角カナを 半角カナに戻す関数を用意するつもりはない。どうしても必要な方は 自作して下さい。
*/
int han2zen(unsigned int *zenkaku, unsigned char *str)
{
    static unsigned int z[64] = {
        0x2121,0x2123,0x2156,0x2157,0x2122,0x2126,0x2572,0x2521,
        0x2523,0x2525,0x2527,0x2529,0x2563,0x2565,0x2567,0x2543,
        0x213c,0x2522,0x2524,0x2526,0x2528,0x252a,0x252b,0x252d,
        0x252f,0x2531,0x2533,0x2535,0x2537,0x2539,0x253b,0x253d,
        0x253f,0x2541,0x2544,0x2546,0x2548,0x254a,0x254b,0x254c,
        0x254d,0x254e,0x254f,0x2552,0x2555,0x2558,0x255b,0x255e,
        0x255f,0x2560,0x2561,0x2562,0x2564,0x2566,0x2568,0x2569,
        0x256a,0x256b,0x256c,0x256d,0x256f,0x2573,0x212b,0x212c};
    typedef struct {
        unsigned char han;
        unsigned int zen;
    } TBL;
    static TBL daku[] = {
        {0xb3,0x2574},{0xb6,0x252c},{0xb7,0x252e},{0xb8,0x2530},
        {0xb9,0x2532},{0xba,0x2534},{0xbb,0x2536},{0xbc,0x2538},
        {0xbd,0x253a},{0xbe,0x253c},{0xbf,0x253e},{0xc0,0x2540},
        {0xc1,0x2542},{0xc2,0x2545},{0xc3,0x2547},{0xc4,0x2549},
        {0xca,0x2550},{0xcb,0x2553},{0xcc,0x2556},{0xcd,0x2559},
        {0xce,0x255c},{0,0}};
    static TBL handaku[] = {
        {0xca,0x2551},{0xcb,0x2554},{0xcc,0x2557},{0xcd,0x255a},
        {0xce,0x255d},{0,0}};
    int i;

    if (*str < 0xa0 || *str > 0xdf) return 0;
    if (*(str+1) == 0xde) {           /* 濁音符 */
        for (i = 0; daku[i].zen != 0; i++)
            if (*str == daku[i].han) {
                *zenkaku = daku[i].zen;
                return 2;
            }
    } else if (*(str+1) == 0xdf) {    /* 半濁音符 */
        for (i = 0; handaku[i].zen != 0; i++)
            if (*str == handaku[i].han) {
                *zenkaku = handaku[i].zen;
                return 2;
            }
    }
    
    *zenkaku = z[*str - 0xa0];
    return 1;
}

/*
シフトJISコードをJISコードに変換する

関数名

sjis2jis  シフトJISコードをJISコードに変換する

形式

unsigned int sjis2jis(unsigned int sjis);

引数

sjis  シフトJISコード

関数値

JISコード

注意事項

用例（sjis2jis-test.c）
    sjis2jis(0x8abf);

説明
    シフトJISコードは Microsoft漢字コードともよばれ、現在の ところ最も広く使用されている8ビット系漢字コードであり、パソコン 上の標準漢字コードでもある。

    一方、JISコードは7ビット系漢字コードであり、JIS規格により定め られた日本国内の標準漢字コード（のはず）である。とくに、かな・ 漢字を含めたインターネット・メールには普通、JISコードが使われ ている。
*/
unsigned int sjis2jis(unsigned int sjis)
{
    unsigned int hib, lob;
    
    hib = (sjis >> 8) & 0xff;
    lob = sjis & 0xff;
    hib -= (hib <= 0x9f) ? 0x71 : 0xb1;
    hib = (hib << 1) + 1;
    if (lob > 0x7f) lob--;
    if (lob >= 0x9e) {
        lob -= 0x7d;
        hib++;
    } else lob -= 0x1f;

    return (hib << 8) | lob;
}

/*
シフトJISコードをEUCコードに変換する

関数名

sjis2euc  シフトJISコードをEUCコードに変換する

形式

unsigned int sjis2euc(unsigned int sjis);

引数

sjis  シフトJISコード

関数値

EUCコード

注意事項

用例（sjis2euc-test.c）
    sjis2euc(0x8abf);

説明
    シフトJISコードは Microsoft漢字コードともよばれ、現在の ところ最も広く使用されている8ビット系漢字コードであり、パソコン 上の標準漢字コードでもある。

    一方、EUCコードはUNIX上で広く使われている8ビット系漢字コード であり、JISコードの第1バイト、第2バイトの両方に0x80を加算した ものを使用する。
*/
unsigned int sjis2euc(unsigned int sjis)
{
    unsigned int hib, lob;
    
    hib = (sjis >> 8) & 0xff;
    lob = sjis & 0xff;
    hib -= (hib <= 0x9f) ? 0x71 : 0xb1;
    hib = (hib << 1) + 1;
    if (lob >= 0x9f) {
        lob -= 0x7e;
        hib++;
    } else if (lob > 0x7f) lob -= 0x20;
    else lob -= 0x1f;

    hib |= 0x80;
    lob |= 0x80;

    return (hib << 8) | lob;
}

/*
JISコードをシフトJISコードに変換する

関数名

jis2sjis  JISコードをシフトJISコードに変換する

形式

unsigned int jis2sjis(unsigned int jis);

引数

jis  JISコード

関数値

シフトJISコード

注意事項

用例（jis2sjis-test.c）
    jis2sjis(0x3441);

説明
    JISコードは7ビット系漢字コードであり、JIS規格により定め られた日本国内の標準漢字コード（のはず）である。とくに、かな・ 漢字を含めたインターネット・メールには普通、JISコードが使われ ている。

    一方、シフトJISコードは Microsoft漢字コードともよばれ、現在の ところ最も広く使用されている8ビット系漢字コードであり、パソコン 上の標準漢字コードでもある。
*/
unsigned int jis2sjis(unsigned int jis)
{
    unsigned int hib, lob;
    
    hib = (jis >> 8) & 0xff;
    lob = jis & 0xff;
    lob += (hib & 1) ? 0x1f : 0x7d;
    if (lob >= 0x7f) lob++;
    hib = ((hib - 0x21) >> 1) + 0x81;
    if (hib > 0x9f) hib += 0x40;

    return (hib << 8) | lob;
}

/*
JISコードをEUCコードに変換する

関数名

jis2euc  JISコードをEUCコードに変換する

形式

unsigned int jis2euc(unsigned int jis);

引数

jis  JISコード

関数値

EUCコード

注意事項

用例（jis2euc-test.c）
    jis2euc(0x3441);

説明
    JISコードは7ビット系漢字コードであり、JIS規格により定め られた日本国内の標準漢字コード（のはず）である。とくに、かな・ 漢字を含めたインターネット・メールには普通、JISコードが使われ ている。

    一方、EUCコードはUNIX上で広く使われている8ビット系漢字コード であり、JISコードの第1バイト、第2バイトの両方に0x80を加算した ものを使用する。
*/
unsigned int jis2euc(unsigned int jis)
{
    return jis | 0x8080;
}

/*
EUCコードをJISコードに変換する

関数名

euc2jis  EUCコードをJISコードに変換する

形式

unsigned int euc2jis(unsigned int euc);

引数

euc  EUCコード

関数値

JISコード

注意事項
    半角カナ対応のEUCコードを全角カナJISコードに変換している。 そのために、関数 euc2jis-test.c） euc2jis(0xb4c1);

説明
    EUCコードはUNIX上で広く使われている8ビット系漢字コード であり、JISコードの第1バイト、第2バイトの両方に0x80を加算した ものを使用する。さらに、ANKの半角カナ文字については、前に1バイト (0x8e) をつけ、2バイトで半角カナ文字を表す。

    一方、JISコードは7ビット系漢字コードであり、JIS規格により定め られた日本国内の標準漢字コード（のはず）である。とくに、かな・ 漢字を含めたインターネット・メールには普通、JISコードが使われ ている。
*/
unsigned int euc2jis(unsigned int euc)
{
    unsigned int jis;

    if ((euc & 0xff00) == 0x8e00)
        jis = hankaku2zen(euc & 0xff);
    else jis = euc & ~0x8080;
    return jis;
}

/*
句点コードをJISコードに変換する

関数名

kuten2jis  JISコードを句点コードに変換する

形式

unsigned int kuten2jis(unsigned int kuten);

引数

kuten  句点コード

関数値

JISコード

注意事項

用例（kuten2jis-test.c）
    kuten2jis(0x3441);

説明
    句点コードは昔のミニコン等に利用される漢字コードで、コード から漢字を入力するときに使われる場合もある。

    一方、JISコードは7ビット系漢字コードであり、JIS規格により定め られた日本国内の標準漢字コード（のはず）である。とくに、かな・ 漢字を含めたインターネット・メールには普通、JISコードが使われ ている。
*/
unsigned int kuten2jis(unsigned int kuten)
{
    unsigned int hib, lob;
    
    hib = kuten / 100 + 32;
    lob = kuten % 100 + 32;

    return (hib << 8) | lob;
}

/*
JISコードを句点コードに変換する

関数名

jis2kuten  JISコードを句点コードに変換する

形式

unsigned int jis2kuten(unsigned int jis);

引数

jis  JISコード

関数値

句点コード

注意事項

用例（jis2kuten-test.c）
    jis2kuten(0x3441);

説明
    JISコードは7ビット系漢字コードであり、JIS規格により定め られた日本国内の標準漢字コード（のはず）である。とくに、かな・ 漢字を含めたインターネット・メールには普通、JISコードが使われ ている。

    一方、句点コードは昔のミニコン等に利用される漢字コードで、コード から漢字を入力するときに使われる場合もある。
*/
unsigned int jis2kuten(unsigned int jis)
{
    unsigned int hib, lob;
    
    hib = (jis >> 8) & 0xff;
    lob = jis & 0xff;

    return (hib-32) * 100 + (lob-32);
}
