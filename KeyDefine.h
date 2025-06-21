/**
 * @file KeyDefine.h
 * @brief ソフトウェアキーボードのキー定義ヘッダー
 * @details ソフトウェアキーボードで使用する定数、キーの種類を定義する列挙型、
 * キーの属性を保持する構造体、そしてキーボード全体のレイアウト配列を定義します。
 */
#pragma once

#include <Windows.h>

/// @brief キーボードのコントロールIDの基底値
#define IDC_KEY_BASE 20000

/**
 * @enum EKeyType
 * @brief キーの種類を定義する列挙型
 */
enum EKeyType
{
    KT_NORMAL,   ///< 通常の文字、数字、記号キー
    KT_MODIFIER, ///< Shift, Ctrl, Alt, CapsLock などの状態保持キー
    KT_ACTION,   ///< Enter, Esc, Backspace などの特殊動作キー
    KT_CLOSE,    ///< 閉じるボタン（現在は未使用、将来的な拡張用）
};

/**
 * @struct KEY_INFO
 * @brief 個々のキーの属性情報を保持する構造体
 */
struct KEY_INFO
{
    UINT uId;             ///< ボタンのコントロールID
    LPCWSTR szLabel;      ///< 通常時のラベル文字列
    LPCWSTR szShiftLabel; ///< Shiftキー押下時のラベル文字列
    BYTE bVirtKey;        ///< 対応する仮想キーコード (VK_...)
    EKeyType eKeyType;    ///< キーの種類 (EKeyType)
    int nColumnSpan;      ///< キーの横幅（標準キー何個分か）
};

/**
 * @brief キーボードのレイアウトを定義する静的定数配列
 * @details 5行x15列の2次元配列で、キーボードの物理的な配置、ラベル、機能、サイズ（スパン）を静的に定義します。
 * この定義に基づき、CSoftwareKeyboardDlgが起動時に動的にキーボタンを生成・配置します。
 * nColumnSpan > 1 のキーの右隣の要素は、プレースホルダーとして uId=0 となっています。
 */
static const KEY_INFO g_KeyLayout[5][15] = {
    // Row 1
    {{IDC_KEY_BASE + 0, L"Esc", L"", VK_ESCAPE, KT_ACTION, 1},
     {IDC_KEY_BASE + 1, L"`", L"~", VK_OEM_3, KT_NORMAL, 1},
     {IDC_KEY_BASE + 2, L"1", L"!", '1', KT_NORMAL, 1},
     {IDC_KEY_BASE + 3, L"2", L"@", '2', KT_NORMAL, 1},
     {IDC_KEY_BASE + 4, L"3", L"#", '3', KT_NORMAL, 1},
     {IDC_KEY_BASE + 5, L"4", L"$", '4', KT_NORMAL, 1},
     {IDC_KEY_BASE + 6, L"5", L"%", '5', KT_NORMAL, 1},
     {IDC_KEY_BASE + 7, L"6", L"^", '6', KT_NORMAL, 1},
     {IDC_KEY_BASE + 8, L"7", L"&", '7', KT_NORMAL, 1},
     {IDC_KEY_BASE + 9, L"8", L"*", '8', KT_NORMAL, 1},
     {IDC_KEY_BASE + 10, L"9", L"(", '9', KT_NORMAL, 1},
     {IDC_KEY_BASE + 11, L"0", L")", '0', KT_NORMAL, 1},
     {IDC_KEY_BASE + 12, L"-", L"_", VK_OEM_MINUS, KT_NORMAL, 1},
     {IDC_KEY_BASE + 13, L"=", L"+", VK_OEM_PLUS, KT_NORMAL, 1},
     {IDC_KEY_BASE + 14, L"Del", L"", VK_DELETE, KT_ACTION, 1}
    },
    // Row 2
    {{IDC_KEY_BASE + 15, L"Tab", L"", VK_TAB, KT_ACTION, 1},
     {IDC_KEY_BASE + 16, L"q", L"Q", 'Q', KT_NORMAL, 1},
     {IDC_KEY_BASE + 17, L"w", L"W", 'W', KT_NORMAL, 1},
     {IDC_KEY_BASE + 18, L"e", L"E", 'E', KT_NORMAL, 1},
     {IDC_KEY_BASE + 19, L"r", L"R", 'R', KT_NORMAL, 1},
     {IDC_KEY_BASE + 20, L"t", L"T", 'T', KT_NORMAL, 1},
     {IDC_KEY_BASE + 21, L"y", L"Y", 'Y', KT_NORMAL, 1},
     {IDC_KEY_BASE + 22, L"u", L"U", 'U', KT_NORMAL, 1},
     {IDC_KEY_BASE + 23, L"i", L"I", 'I', KT_NORMAL, 1},
     {IDC_KEY_BASE + 24, L"o", L"O", 'O', KT_NORMAL, 1},
     {IDC_KEY_BASE + 25, L"p", L"P", 'P', KT_NORMAL, 1},
     {IDC_KEY_BASE + 26, L"[", L"{", VK_OEM_4, KT_NORMAL, 1},
     {IDC_KEY_BASE + 27, L"]", L"}", VK_OEM_6, KT_NORMAL, 1},
     {IDC_KEY_BASE + 28, L"\\", L"|", VK_OEM_5, KT_NORMAL, 1},
     {IDC_KEY_BASE + 29, L"BS", L"", VK_BACK, KT_ACTION, 1}
    },
    // Row 3
    {{IDC_KEY_BASE + 30, L"Caps", L"", VK_CAPITAL, KT_MODIFIER, 2},
     {0}, // Span placeholder
     {IDC_KEY_BASE + 31, L"a", L"A", 'A', KT_NORMAL, 1},
     {IDC_KEY_BASE + 32, L"s", L"S", 'S', KT_NORMAL, 1},
     {IDC_KEY_BASE + 33, L"d", L"D", 'D', KT_NORMAL, 1},
     {IDC_KEY_BASE + 34, L"f", L"F", 'F', KT_NORMAL, 1},
     {IDC_KEY_BASE + 35, L"g", L"G", 'G', KT_NORMAL, 1},
     {IDC_KEY_BASE + 36, L"h", L"H", 'H', KT_NORMAL, 1},
     {IDC_KEY_BASE + 37, L"j", L"J", 'J', KT_NORMAL, 1},
     {IDC_KEY_BASE + 38, L"k", L"K", 'K', KT_NORMAL, 1},
     {IDC_KEY_BASE + 39, L"l", L"L", 'L', KT_NORMAL, 1},
     {IDC_KEY_BASE + 40, L";", L":", VK_OEM_1, KT_NORMAL, 1},
     {IDC_KEY_BASE + 41, L"'", L"\"", VK_OEM_7, KT_NORMAL, 1},
     {IDC_KEY_BASE + 42, L"Enter", L"", VK_RETURN, KT_ACTION, 2}
    },
    // Row 4
    {{IDC_KEY_BASE + 43, L"Shift", L"", VK_LSHIFT, KT_MODIFIER, 2},
     {0}, // Span placeholder
     {IDC_KEY_BASE + 44, L"z", L"Z", 'Z', KT_NORMAL, 1},
     {IDC_KEY_BASE + 45, L"x", L"X", 'X', KT_NORMAL, 1},
     {IDC_KEY_BASE + 46, L"c", L"C", 'C', KT_NORMAL, 1},
     {IDC_KEY_BASE + 47, L"v", L"V", 'V', KT_NORMAL, 1},
     {IDC_KEY_BASE + 48, L"b", L"B", 'B', KT_NORMAL, 1},
     {IDC_KEY_BASE + 49, L"n", L"N", 'N', KT_NORMAL, 1},
     {IDC_KEY_BASE + 50, L"m", L"M", 'M', KT_NORMAL, 1},
     {IDC_KEY_BASE + 51, L",", L"<", VK_OEM_COMMA, KT_NORMAL, 1},
     {IDC_KEY_BASE + 52, L".", L">", VK_OEM_PERIOD, KT_NORMAL, 1},
     {IDC_KEY_BASE + 53, L"/", L"?", VK_OEM_2, KT_NORMAL, 1},
     {IDC_KEY_BASE + 54, L"Shift", L"", VK_RSHIFT, KT_MODIFIER, 2}
    },
    // Row 5
    {{IDC_KEY_BASE + 55, L"Ctrl", L"", VK_LCONTROL, KT_MODIFIER, 2},
     {0}, // Span placeholder
     {IDC_KEY_BASE + 56, L"Fn", L"", 0, KT_MODIFIER, 1}, // No VK for Fn
     {IDC_KEY_BASE + 57, L"Alt", L"", VK_LMENU, KT_MODIFIER, 1},
     {IDC_KEY_BASE + 58, L" ", L"", VK_SPACE, KT_ACTION, 5},
     {0}, {0}, {0}, {0}, // Span placeholders
     {IDC_KEY_BASE + 59, L"Alt", L"", VK_RMENU, KT_MODIFIER, 1},
     {IDC_KEY_BASE + 60, L"Ctrl", L"", VK_RCONTROL, KT_MODIFIER, 2}
    }
};