#include <windows.h>
#include <argparser.hpp>
#include <clipboard.hpp>

const wchar_t delim = L'\t';
const wchar_t* lineterminator = L"\r\n";

void retwrite(ClipboardText<wchar_t>& dest, const std::vector<std::wstring_view>& src) {
    for(auto& it : src) {
        dest.write(it);
        dest.write(delim);
    }
    dest.seek(-1, 1);
    dest.write(lineterminator, 2);
}

int wmain(int argc, wchar_t* argv[]) {
    std::wcout.imbue(std::locale("Japanese", std::locale::ctype));
    setlocale(LC_ALL, "");
    bool skip_row = false;
    wchar_t *txt = NULL;
    std::vector<std::wstring_view> pre, cur;
    std::size_t len = 0;
    int i = 0;

    auto ap = ArgParser(L"Excel表をコピーして、下の行の空白を上の行の値で値保管するプログラム.\n", argc, argv);
    ap.add(L"-s", L"--skip_row", &skip_row, L"詳細行のみ表示する\n");
    ap.parse();

    // const wchar_t txt[] = L"111\t\t\r\n\taaa\t\r\n\tbb\tA\r\n222\t\t\r\n\tc\t\r\n\t\tB\r\n";
    if(readclip(txt) == false)
        return 1;
    len = wcslen(txt);
    ClipboardText<wchar_t> dest('w', len * 2);

    for(const wchar_t *p = txt, *start = p; *p; ++p) {
        if(*p != L'\n')
            continue;
        if(i > 1)
            cur.clear();
        for(const wchar_t* pp = start; pp != p + 1; ++pp) {
            if(*pp == delim || *pp == L'\r') {
                if(i == 0)
                    pre.emplace_back(start, pp - start);
                else
                    cur.emplace_back(start, pp - start);
                start = pp + 1;
            }
        }
        if(i != 0) {
            bool writable = true;
            for(int j = 0, end = cur.size(); j < end; ++j) {
                if(j > 0 && cur[j - 1] != pre[j - 1])
                    continue;
                if(cur[j].empty()) {
                    if(!pre[j].empty())
                        cur[j] = pre[j];
                } else if(skip_row && pre[j].empty()) {
                    writable = false;
                }
            }
            if(writable)
                retwrite(dest, pre);
            pre = cur;
        }
        start = p + 1;
        ++i;
    }
    if(i == 0)
        return 1;
    else if(i == 1)
        dest.write(txt, len);
    else
        retwrite(dest, cur);

    dest.close();
    return 0;
}
