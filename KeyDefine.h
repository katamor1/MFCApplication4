// KeyDefine.h
#pragma once

#include <Windows.h>

// キーボードのコントロールIDの基底値
#define IDC_KEY_BASE 20000

// キーの種類
enum EKeyType
{
    KT_NORMAL,   // 通常の文字、数字、記号キー
    KT_MODIFIER, // Shift, Ctrl, Alt などの状態保持キー
    KT_ACTION,   // Enter, Esc, Backspace などの特殊動作キー
    KT_CLOSE,    // 閉じるボタン（特別扱い）
};

// キー情報を保持する構造体
struct KEY_INFO
{
    UINT uId;             // ボタンのコントロールID
    LPCWSTR szLabel;      // 通常時のラベル
    LPCWSTR szShiftLabel; // Shiftキー押下時のラベル
    BYTE bVirtKey;        // 仮想キーコード (VK_...)
    EKeyType eKeyType;    // キーの種類
    int nColumnSpan;      // キーの横幅（標準キー何個分か）
};

// キーボードのレイアウト定義
// 仕様書の配列に基づき定義します。
static const KEY_INFO g_KeyLayout[5][15] = {
    // Row 1
    {{IDC_KEY_BASE + 0, _T("Esc"), _T(""), VK_ESCAPE, KT_ACTION, 1},
     {IDC_KEY_BASE + 1, _T("`"), _T("~"), VK_OEM_3, KT_NORMAL, 1},
     {IDC_KEY_BASE + 2, _T("1"), _T("!"), '1', KT_NORMAL, 1},
     {IDC_KEY_BASE + 3, _T("2"), _T("@"), '2', KT_NORMAL, 1},
     {IDC_KEY_BASE + 4, _T("3"), _T("#"), '3', KT_NORMAL, 1},
     {IDC_KEY_BASE + 5, _T("4"), _T("$"), '4', KT_NORMAL, 1},
     {IDC_KEY_BASE + 6, _T("5"), _T("%"), '5', KT_NORMAL, 1},
     {IDC_KEY_BASE + 7, _T("6"), _T("^"), '6', KT_NORMAL, 1},
     {IDC_KEY_BASE + 8, _T("7"), _T("&"), '7', KT_NORMAL, 1},
     {IDC_KEY_BASE + 9, _T("8"), _T("*"), '8', KT_NORMAL, 1},
     {IDC_KEY_BASE + 10, _T("9"), _T("("), '9', KT_NORMAL, 1},
     {IDC_KEY_BASE + 11, _T("0"), _T(")"), '0', KT_NORMAL, 1},
     {IDC_KEY_BASE + 12, _T("-"), _T("_"), VK_OEM_MINUS, KT_NORMAL, 1},
     {IDC_KEY_BASE + 13, _T("="), _T("+"), VK_OEM_PLUS, KT_NORMAL, 1},
     {IDC_KEY_BASE + 14, _T("De_T("), _T(")"), VK_DELETE, KT_ACTION, 1}},
    // Row 2
    {{IDC_KEY_BASE + 15, _T("Tab"), _T(""), VK_TAB, KT_ACTION, 1},
     {IDC_KEY_BASE + 16, _T("q"), _T("Q"), 'Q', KT_NORMAL, 1},
     {IDC_KEY_BASE + 17, _T("w"), _T("W"), 'W', KT_NORMAL, 1},
     {IDC_KEY_BASE + 18, _T("e"), _T("E"), 'E', KT_NORMAL, 1},
     {IDC_KEY_BASE + 19, _T("r"), _T("R"), 'R', KT_NORMAL, 1},
     {IDC_KEY_BASE + 20, _T("t"), _T("T"), 'T', KT_NORMAL, 1},
     {IDC_KEY_BASE + 21, _T("y"), _T("Y"), 'Y', KT_NORMAL, 1},
     {IDC_KEY_BASE + 22, _T("u"), _T("U"), 'U', KT_NORMAL, 1},
     {IDC_KEY_BASE + 23, _T("i"), _T("I"), 'I', KT_NORMAL, 1},
     {IDC_KEY_BASE + 24, _T("o"), _T("O"), 'O', KT_NORMAL, 1},
     {IDC_KEY_BASE + 25, _T("p"), _T("P"), 'P', KT_NORMAL, 1},
     {IDC_KEY_BASE + 26, _T("["), _T("{"), VK_OEM_4, KT_NORMAL, 1},
     {IDC_KEY_BASE + 27, _T("]"), _T("}"), VK_OEM_6, KT_NORMAL, 1},
     {IDC_KEY_BASE + 28, _T("\\"), _T("|"), VK_OEM_5, KT_NORMAL, 1},
     {IDC_KEY_BASE + 29, _T("BS"), _T(""), VK_BACK, KT_ACTION, 1}},
    // Row 3
    {
        {IDC_KEY_BASE + 30, _T("Caps"), _T(""), VK_CAPITAL, KT_MODIFIER, 2},
        {IDC_KEY_BASE + 31, _T("a"), _T("A"), 'A', KT_NORMAL, 1},
        {IDC_KEY_BASE + 32, _T("s"), _T("S"), 'S', KT_NORMAL, 1},
        {IDC_KEY_BASE + 33, _T("d"), _T("D"), 'D', KT_NORMAL, 1},
        {IDC_KEY_BASE + 34, _T("f"), _T("F"), 'F', KT_NORMAL, 1},
        {IDC_KEY_BASE + 35, _T("g"), _T("G"), 'G', KT_NORMAL, 1},
        {IDC_KEY_BASE + 36, _T("h"), _T("H"), 'H', KT_NORMAL, 1},
        {IDC_KEY_BASE + 37, _T("j"), _T("J"), 'J', KT_NORMAL, 1},
        {IDC_KEY_BASE + 38, _T("k"), _T("K"), 'K', KT_NORMAL, 1},
        {IDC_KEY_BASE + 39, _T("_T("), _T(")L"), 'L', KT_NORMAL, 1},
        {IDC_KEY_BASE + 40, _T(";"), _T(":"), VK_OEM_1, KT_NORMAL, 1},
        {IDC_KEY_BASE + 41, _T("'"), _T("\""), VK_OEM_7, KT_NORMAL, 1},
        {IDC_KEY_BASE + 42, _T("Ent"), _T(""), VK_RETURN, KT_ACTION, 2},
        {
            0,
        },
        {
            0,
        } // Span placeholders
    },
    // Row 4
    {
        {IDC_KEY_BASE + 43, _T("Shift"), _T(""), VK_LSHIFT, KT_MODIFIER, 3},
        {IDC_KEY_BASE + 44, _T("z"), _T("Z"), 'Z', KT_NORMAL, 1},
        {IDC_KEY_BASE + 45, _T("x"), _T("X"), 'X', KT_NORMAL, 1},
        {IDC_KEY_BASE + 46, _T("c"), _T("C"), 'C', KT_NORMAL, 1},
        {IDC_KEY_BASE + 47, _T("v"), _T("V"), 'V', KT_NORMAL, 1},
        {IDC_KEY_BASE + 48, _T("b"), _T("B"), 'B', KT_NORMAL, 1},
        {IDC_KEY_BASE + 49, _T("n"), _T("N"), 'N', KT_NORMAL, 1},
        {IDC_KEY_BASE + 50, _T("m"), _T("M"), 'M', KT_NORMAL, 1},
        {IDC_KEY_BASE + 51, _T(","), _T("<"), VK_OEM_COMMA, KT_NORMAL, 1},
        {IDC_KEY_BASE + 52, _T("."), _T(">"), VK_OEM_PERIOD, KT_NORMAL, 1},
        {IDC_KEY_BASE + 53, _T("/"), _T("?"), VK_OEM_2, KT_NORMAL, 1},
        {IDC_KEY_BASE + 54, _T("Shift"), _T(""), VK_RSHIFT, KT_MODIFIER, 2},
        {
            0,
        },
        {
            0,
        },
        {
            0,
        } // Span placeholders
    },
    // Row 5
    {
        {IDC_KEY_BASE + 55, _T("Ctr_T("), _T(")"), VK_LCONTROL, KT_MODIFIER, 2},
        {IDC_KEY_BASE + 56, _T("Fn"), _T(""), 0, KT_MODIFIER, 1}, // No VK for Fn
        {IDC_KEY_BASE + 57, _T("Alt"), _T(""), VK_LMENU, KT_MODIFIER, 1},
        {IDC_KEY_BASE + 58, _T(" "), _T(""), VK_SPACE, KT_ACTION, 8},
        {IDC_KEY_BASE + 59, _T("Alt"), _T(""), VK_RMENU, KT_MODIFIER, 1},
        {IDC_KEY_BASE + 60, _T("Ctr_T("), _T(")"), VK_RCONTROL, KT_MODIFIER, 2},
        {
            0,
        },
        {
            0,
        },
        {
            0,
        },
        {
            0,
        },
        {
            0,
        },
        {
            0,
        },
        {
            0,
        },
        {
            0,
        },
        {
            0,
        } // Span placeholders
    }};