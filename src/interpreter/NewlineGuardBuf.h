#pragma once

#include <ostream>
#include <streambuf>

/// A streambuf decorator that wraps an existing streambuf and tracks whether
/// the last character written to it was a newline.
///
/// This allows the interpreter's REPL to guarantee that the command prompt
/// always starts on a fresh line, even when a command does not end its output
/// with '\n' (per spec: "interpreter moves cursor to new line after command").
class NewlineGuardBuf : public std::streambuf {
public:

    /// Wraps the given streambuf.  The wrapped buf must outlive this object.
    explicit NewlineGuardBuf(std::streambuf* wrapped)
        : wrappedBuf(wrapped), lastChar('\n') {}

    /// If the last written character was not '\n', writes one to the wrapped
    /// buffer so the next output starts on a fresh line.
    void ensureNewline() {
        if (lastChar != '\n') {
            wrappedBuf->sputc('\n');
            lastChar = '\n';
        }
    }

protected:

    /// Forwards a single character to the wrapped streambuf and records it.
    int overflow(int ch) override {
        if (ch == EOF) return EOF;
        lastChar = static_cast<char>(ch);
        return wrappedBuf->sputc(static_cast<char>(ch));
    }

    /// Forwards a sequence of characters and records the last one.
    std::streamsize xsputn(const char* s, std::streamsize count) override {
        if (count > 0) {
            lastChar = s[count - 1];
        }
        return wrappedBuf->sputn(s, count);
    }

    /// Forwards flush requests to the wrapped streambuf.
    /// Without this override, guardOut.flush() would be silently swallowed
    /// and the prompt would never appear on screen before the program blocks
    /// waiting for user input.
    int sync() override {
        return wrappedBuf->pubsync();
    }

private:

    /// The underlying streambuf being wrapped.
    std::streambuf* wrappedBuf;

    /// The last character written through this buffer (initialised to '\n'
    /// so we never insert a spurious leading newline).
    char lastChar;
};
