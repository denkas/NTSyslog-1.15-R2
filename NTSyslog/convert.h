/*-----------------------------------------------------------------------------
 *
 *  convert.h - 文字コード変換モジュール ヘッダファイル
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

/* 文字コード種別の判定*/
extern int whatKanji(unsigned char *str);
/* 半角カナを全角カナに変換する */
extern unsigned int hankaku2zen(int hankaku);
/* 半角カナを全角カナに変換する（濁音対応版）*/
extern int han2zen(unsigned int *zenkaku, unsigned char *str);
/* シフトJISコードをJISコードに変換する */
extern unsigned int sjis2jis(unsigned int sjis);
/* シフトJISコードをEUCコードに変換する */
extern unsigned int sjis2euc(unsigned int sjis);
/* JISコードをシフトJISコードに変換する */
extern unsigned int jis2sjis(unsigned int jis);
/* JISコードをEUCコードに変換する */
extern unsigned int jis2euc(unsigned int jis);
/* EUCコードをJISコードに変換する */
extern unsigned int euc2jis(unsigned int euc);
/* 句点コードをJISコードに変換する */
extern unsigned int kuten2jis(unsigned int kuten);
/* JISコードを句点コードに変換する */
extern unsigned int jis2kuten(unsigned int jis);
