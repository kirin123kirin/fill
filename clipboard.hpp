/* clipboard.hpp | MIT License | https://github.com/kirin123kirin/clipboard/raw/main/LICENSE */
#ifndef _CLIPBOARD_HPP_
#define _CLIPBOARD_HPP_

#include <windows.h>
#include <cstddef>
#include <iostream>
#include <string>

template <typename CharT,
          std::size_t BUFLEN = 1024,
          int FLAG = std::is_same_v<CharT, wchar_t> ? CF_UNICODETEXT : CF_TEXT>
class ClipboardText {
    using valuetype = CharT;
    HANDLE HND;
    CharT* Buf;
    CharT* pos;
    bool opened;
    bool seted;
    const char mode;
    std::size_t len;
    std::size_t memlen;
    std::size_t memsize;

   public:
    ClipboardText()
        : HND(nullptr),
          Buf(NULL),
          pos(NULL),
          opened(false),
          seted(false),
          mode('r'),
          len(0),
          memlen(BUFLEN),
          memsize((BUFLEN + 1) * sizeof(CharT)) {}
    ClipboardText(const char _mode, std::size_t _memlen = BUFLEN + 1)
        : HND(nullptr),
          Buf(NULL),
          pos(NULL),
          opened(false),
          seted(false),
          mode(_mode),
          len(0),
          memlen(_memlen),
          memsize((_memlen + 1) * sizeof(CharT)) {
        this->open(_mode);
    }
    ClipboardText(std::size_t _memlen)
        : HND(nullptr),
          Buf(NULL),
          pos(NULL),
          opened(false),
          seted(false),
          mode('w'),
          len(0),
          memlen(_memlen),
          memsize((_memlen + 1) * sizeof(CharT)) {
        this->open(mode);
    }
    ~ClipboardText() {
        if(opened)
            this->close();
    }

    bool open(const char mode) {
        if(opened == false) {
            if((opened = OpenClipboard(nullptr)) == false) {
                std::cerr << "クリップボードを開けませんでした。" << std::endl;
                return false;
            }
        }
        if(mode == 'r') {
            if((HND = GetClipboardData(FLAG)) == nullptr)
                opened = false;
            if((Buf = static_cast<CharT*>(GlobalLock(HND))) == nullptr)
                std::wcerr << L"クリップボードのテキストを取得できませんでした。" << std::endl;
            len = std::char_traits<CharT>::length(Buf);
            memlen = len + 1;
            memsize = memlen * sizeof(CharT);
            pos = Buf;
        } else if(mode == 'w') {
            if(HND) {
                GlobalFree(HND);
                HND = NULL;
                Buf = NULL;
            }
            EmptyClipboard();
            if((HND = GlobalAlloc(GHND, memsize)) == nullptr) {
                std::wcerr << L"クリップボードデータのメモリ確保に失敗しました。" << std::endl;
                return false;
            }
            if((Buf = static_cast<CharT*>(GlobalLock(HND))) == NULL)
                return false;
            pos = Buf;
        } else {
            std::cerr << "Unknown mode. 'r' or 'w'" << std::endl;
            opened = false;
        }
        return opened;
    }

    bool close() {
        if(opened)
            GlobalUnlock(HND);
        if(mode == 'w' && tell() && seted == false && Buf)
            SetClipboardData(FLAG, Buf);
        opened = false;
        return CloseClipboard() != 0;
    }

    CharT* data() { return Buf; }
    const CharT* getvalues() { return Buf; }
    const CharT* read() { return pos; }
    const CharT* read(std::size_t _len) { return std::basic_string_view<CharT>(pos, _len).data(); }
    std::size_t tell() { return pos - Buf; }
    CharT* seek(int _len, int offset = 0) {
        if(offset == 0) {
            if(memlen < _len) {
                std::wcerr << L"メモリサイズを超えてます。" << memlen << L"以下の長さでお願いします。" << std::endl;
                return NULL;
            }
            pos = Buf + _len;
        } else if(offset == 1) {
            if((_len > 0 && memlen - len < _len) || (_len < 0 && len + _len < 0)) {
                std::wcerr << L"メモリサイズを超えてます。" << memlen - len << L"以下の長さでお願いします。"
                           << std::endl;
                return NULL;
            }
            pos += _len;
        } else if(offset == 2) {
            if(memlen < _len) {
                std::wcerr << L"メモリサイズを超えてます。" << memlen << L"以下の長さでお願いします。" << std::endl;
                return NULL;
            }
            pos = Buf + memlen - _len;
        } else {
            std::cerr << "Unknown offset code. 0 or 1 or 2" << std::endl;
            return NULL;
        }
        return pos;
    }
    std::size_t size() { return len; }

   private:
    bool resize(std::size_t _len) {
        if(mode == 'r') {
            std::cerr << "Unknown mode. 'r' or 'w'" << std::endl;
            return false;
        }
        if((HND = GlobalReAlloc(HND, (_len + 1) * sizeof(CharT), GHND)) == nullptr)
            return false;
        if((Buf = static_cast<CharT*>(GlobalLock(HND))) == NULL)
            return false;
        pos = Buf + len;
        memlen = _len;
        memsize = (memlen + 1) * sizeof(CharT);
        return true;
    }
    bool write_mode_check() {
        if(mode == 'r') {
            std::cerr << "Unknown mode. 'r' or 'w'" << std::endl;
            return false;
        }
        if(opened == false || HND == nullptr)
            return false;
        if(Buf == NULL) {
            std::wcerr << L"バッファに書き込めません" << std::endl;
            return false;
        }
        return true;
    }

   public:
    bool dump(const std::basic_string<CharT>& sv) {
        if(sv.empty() || this->write_mode_check() == false)
            return false;
        std::size_t _len = sv.size();
        if(memlen < _len + len)
            this->resize(memlen + (BUFLEN < _len ? _len * 2 : BUFLEN));
        std::char_traits<CharT>::copy(pos, sv.data(), _len);
        len += _len;
        pos += _len;
        if(SetClipboardData(FLAG, Buf))
            seted = true;
        return true;
    }

    bool dump(const std::basic_string_view<CharT>& sv) {
        if(sv.empty() || this->write_mode_check() == false)
            return false;
        std::size_t _len = sv.size();
        if(memlen < _len + len)
            this->resize(memlen + (BUFLEN < _len ? _len * 2 : BUFLEN));
        std::char_traits<CharT>::copy(pos, sv.data(), _len);
        len += _len;
        pos += _len;
        if(SetClipboardData(FLAG, Buf))
            seted = true;
        return true;
    }

    bool dump(const CharT* str, std::size_t _len) {
        if(str == NULL || this->write_mode_check() == false)
            return false;
        if(memlen < _len + len)
            this->resize(memlen + (BUFLEN < _len ? _len * 2 : BUFLEN));
        std::char_traits<CharT>::copy(pos, str, _len);
        len += _len;
        pos += _len;
        if(SetClipboardData(FLAG, Buf))
            seted = true;
        return true;
    }

    bool dump(const CharT* str) {
        if(str == NULL || this->write_mode_check() == false)
            return false;
        for(; *str; ++len) {
            if(memlen < len + 1)
                this->resize(memlen + BUFLEN);
            *pos++ = *str++;
        }
        if(SetClipboardData(FLAG, Buf))
            seted = true;
        return true;
    }

    bool dump() {
        if(this->write_mode_check() == false)
            return false;
        if(SetClipboardData(FLAG, Buf))
            seted = true;
        return true;
    }

    bool write(const std::basic_string_view<CharT>& sv) {
        if(sv.empty() || this->write_mode_check() == false)
            return false;
        std::size_t _len = sv.size();
        if(memlen < _len + len)
            this->resize(memlen + (BUFLEN < _len ? _len * 2 : BUFLEN));
        std::char_traits<CharT>::copy(pos, sv.data(), _len);
        len += _len;
        pos += _len;
        return true;
    }

    bool write(const std::basic_string<CharT>& sv) {
        if(sv.empty() || this->write_mode_check() == false)
            return false;
        std::size_t _len = sv.size();
        if(memlen < _len + len)
            this->resize(memlen + (BUFLEN < _len ? _len * 2 : BUFLEN));
        std::char_traits<CharT>::copy(pos, sv.data(), _len);
        len += _len;
        pos += _len;
        return true;
    }

    bool write(const CharT* str) {
        if(str == NULL || this->write_mode_check() == false)
            return false;

        for(; *str; ++len) {
            if(memlen < len + 1)
                this->resize(memlen + BUFLEN);
            *pos++ = *str++;
        }
        return true;
    }

    bool write(const CharT* str, std::size_t _len) {
        if(str == NULL || this->write_mode_check() == false)
            return false;
        if(memlen < _len + len)
            this->resize(memlen + (BUFLEN < _len ? _len * 2 : BUFLEN));
        std::char_traits<CharT>::copy(pos, str, _len);
        len += _len;
        pos += _len;
        return true;
    }

    bool write(CharT str) {
        if(str == NULL || this->write_mode_check() == false)
            return false;
        if(memlen < len + 1)
            this->resize(memlen + BUFLEN);
        *pos++ = str;
        ++len;
        return true;
    }
};

template <typename CharT>
bool readclip(CharT* dest) {
    ClipboardText<CharT> cb('r');
    dest = cb.data();
    return cb.close();
}

template <typename CharT>
bool writeclip(CharT* src) {
    ClipboardText<CharT> cb('w', std::char_traits<CharT>::length(src));
    cb.dump(src);
    return cb.close();
}

// int main() {
//     char h[10];
//     readclip(h);
//     return 0;
// }
#endif /* _CLIPBOARD_HPP_ */
