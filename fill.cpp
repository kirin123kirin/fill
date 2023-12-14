#include <windows.h>
#include "argparser.hpp"

const wchar_t delim = L'\t';
const wchar_t* lineterminator = L"\r\n";

int wmain(int argc, wchar_t* argv[]) {
    std::wcout.imbue(std::locale("Japanese", std::locale::ctype));
    setlocale(LC_ALL, "");

    if(argc != 1) {
        std::wcerr << L"引数は指定できません" << std::endl;
        return 1;
    }

    if(OpenClipboard(nullptr) == false) {
        std::wcerr << L"クリップボードを開けませんでした。" << std::endl;
        return 1;
    }

    HANDLE hData = GetClipboardData(CF_UNICODETEXT);

    if(hData != nullptr) {
        wchar_t* txt = static_cast<wchar_t*>(GlobalLock(hData));
        const wchar_t *a = txt, *p = txt, *start = txt;
        std::vector<std::wstring> prev;
        std::vector<std::wstring> cur;

        if(txt != nullptr) {
            GlobalUnlock(hData);

            while(*p != L'\r') {
                if(*p == delim) {
                    prev.emplace_back(start, p);
                    start = p + 1;
                }
                ++p;
            }
            prev.emplace_back(start, p++);
            std::wstring ret(a, ++p);
            for(start = p; *p; ++p) {
                if(*p == L'\r') {
                    cur.emplace_back(start, p++);
                    for(int i = 0, len = cur.size(); i < len; ++i) {
                        if(cur[i].empty() && !prev[i].empty()) {
                            if(i == 0 || cur[i - 1] == prev[i - 1])
                                cur[i] = prev[i];
                        }
                        ret.append(cur[i] + delim);
                    }
                    ret.append(lineterminator);
                    prev = cur;
                    start = p + 1;
                    cur.clear();
                } else if(*p == delim) {
                    cur.emplace_back(start, p);
                    start = p + 1;
                }
            }
            GlobalUnlock(hData);

            if(!ret.empty()) {
                EmptyClipboard();
                hData = GlobalAlloc(GMEM_MOVEABLE, (ret.size() + 1) * sizeof(wchar_t));
                if(hData != nullptr) {
                    wchar_t* pData = static_cast<wchar_t*>(GlobalLock(hData));
                    wcscpy_s(pData, ret.size() + 1, ret.data());
                    GlobalUnlock(hData);

                    SetClipboardData(CF_UNICODETEXT, hData);
                } else {
                    std::wcerr << L"クリップボードデータのメモリ確保に失敗しました。" << std::endl;
                }

                if(hData != nullptr)
                    std::wcerr << L"クリップボードにデータを格納しました。Excelに貼り付けてください" << std::endl;
            } else {
                std::wcerr << L"fillに失敗しました" << std::endl;
            }

        } else {
            std::wcerr << L"クリップボードのテキストを取得できませんでした。" << std::endl;
        }
    }
    CloseClipboard();

    return 0;
}
