

#include "json.h"

#ifndef LIB_JSONCPP_JSON_TOOL_H_INCLUDED
#define LIB_JSONCPP_JSON_TOOL_H_INCLUDED

namespace Json {

static inline std::string codePointToUTF8(unsigned int cp) {
    std::string result;

    if (cp <= 0x7f) {
        result.resize(1);
        result[0] = static_cast<char>(cp);
    } else if (cp <= 0x7FF) {
        result.resize(2);
        result[1] = static_cast<char>(0x80 | (0x3f & cp));
        result[0] = static_cast<char>(0xC0 | (0x1f & (cp >> 6)));
    } else if (cp <= 0xFFFF) {
        result.resize(3);
        result[2] = static_cast<char>(0x80 | (0x3f & cp));
        result[1] = 0x80 | static_cast<char>((0x3f & (cp >> 6)));
        result[0] = 0xE0 | static_cast<char>((0xf & (cp >> 12)));
    } else if (cp <= 0x10FFFF) {
        result.resize(4);
        result[3] = static_cast<char>(0x80 | (0x3f & cp));
        result[2] = static_cast<char>(0x80 | (0x3f & (cp >> 6)));
        result[1] = static_cast<char>(0x80 | (0x3f & (cp >> 12)));
        result[0] = static_cast<char>(0xF0 | (0x7 & (cp >> 18)));
    }

    return result;
}

static inline bool isControlCharacter(char ch) { return ch > 0 && ch <= 0x1F; }

enum {

    uintToStringBufferSize = 3 * sizeof(LargestUInt) + 1
};

typedef char UIntToStringBuffer[uintToStringBufferSize];

static inline void uintToString(LargestUInt value, char *&current) {
    *--current = 0;
    do {
        *--current = char(value % 10) + '0';
        value /= 10;
    } while (value != 0);
}

static inline void fixNumericLocale(char *begin, char *end) {
    while (begin < end) {
        if (*begin == ',') { *begin = '.'; }
        ++begin;
    }
}

}  // namespace Json

#endif

#if !defined(JSON_IS_AMALGAMATION)
#include <json/assertions.h>
#include <json/reader.h>
#include <json/value.h>
#include "json_tool.h"
#endif

#include <cassert>
#include <cstdio>
#include <cstring>
#include <istream>
#include <utility>

#if defined(_MSC_VER) && _MSC_VER < 1500
#define snprintf _snprintf
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1400

#pragma warning(disable : 4996)
#endif

namespace Json {

Features::Features()
    : allowComments_(true), strictRoot_(false), allowDroppedNullPlaceholders_(false), allowNumericKeys_(false) {}

Features Features::all() { return Features(); }

Features Features::strictMode() {
    Features features;
    features.allowComments_ = false;
    features.strictRoot_ = true;
    features.allowDroppedNullPlaceholders_ = false;
    features.allowNumericKeys_ = false;
    return features;
}

static inline bool in(Reader::Char c, Reader::Char c1, Reader::Char c2, Reader::Char c3, Reader::Char c4) {
    return c == c1 || c == c2 || c == c3 || c == c4;
}

static inline bool in(Reader::Char c,
                      Reader::Char c1,
                      Reader::Char c2,
                      Reader::Char c3,
                      Reader::Char c4,
                      Reader::Char c5) {
    return c == c1 || c == c2 || c == c3 || c == c4 || c == c5;
}

static bool containsNewLine(Reader::Location begin, Reader::Location end) {
    for (; begin < end; ++begin)
        if (*begin == '\n' || *begin == '\r') return true;
    return false;
}

Reader::Reader()
    : errors_(),
      document_(),
      begin_(),
      end_(),
      current_(),
      lastValueEnd_(),
      lastValue_(),
      commentsBefore_(),
      features_(Features::all()),
      collectComments_() {}

Reader::Reader(const Features &features)
    : errors_(),
      document_(),
      begin_(),
      end_(),
      current_(),
      lastValueEnd_(),
      lastValue_(),
      commentsBefore_(),
      features_(features),
      collectComments_() {}

bool Reader::parse(const std::string &document, Value &root, bool collectComments) {
    document_ = document;
    const char *begin = document_.c_str();
    const char *end = begin + document_.length();
    return parse(begin, end, root, collectComments);
}

bool Reader::parse(std::istream &sin, Value &root, bool collectComments) {
    std::string doc;
    std::getline(sin, doc, (char)EOF);
    return parse(doc, root, collectComments);
}

bool Reader::parse(const char *beginDoc, const char *endDoc, Value &root, bool collectComments) {
    if (!features_.allowComments_) { collectComments = false; }

    begin_ = beginDoc;
    end_ = endDoc;
    collectComments_ = collectComments;
    current_ = begin_;
    lastValueEnd_ = 0;
    lastValue_ = 0;
    commentsBefore_ = "";
    errors_.clear();
    while (!nodes_.empty()) nodes_.pop();
    nodes_.push(&root);

    bool successful = readValue();
    Token token;
    skipCommentTokens(token);
    if (collectComments_ && !commentsBefore_.empty()) root.setComment(commentsBefore_, commentAfter);
    if (features_.strictRoot_) {
        if (!root.isArray() && !root.isObject()) {
            token.type_ = tokenError;
            token.start_ = beginDoc;
            token.end_ = endDoc;
            addError(
                "A valid JSON document must be either an array or an object "
                "value.",
                token);
            return false;
        }
    }
    return successful;
}

bool Reader::readValue() {
    Token token;
    skipCommentTokens(token);
    bool successful = true;

    if (collectComments_ && !commentsBefore_.empty()) {
        size_t lastNonNewline = commentsBefore_.find_last_not_of("\r\n");
        if (lastNonNewline != std::string::npos) {
            commentsBefore_.erase(lastNonNewline + 1);
        } else {
            commentsBefore_.clear();
        }

        currentValue().setComment(commentsBefore_, commentBefore);
        commentsBefore_ = "";
    }

    switch (token.type_) {
        case tokenObjectBegin:
            successful = readObject(token);
            currentValue().setOffsetLimit(current_ - begin_);
            break;
        case tokenArrayBegin:
            successful = readArray(token);
            currentValue().setOffsetLimit(current_ - begin_);
            break;
        case tokenNumber:
            successful = decodeNumber(token);
            break;
        case tokenString:
            successful = decodeString(token);
            break;
        case tokenTrue:
            currentValue() = true;
            currentValue().setOffsetStart(token.start_ - begin_);
            currentValue().setOffsetLimit(token.end_ - begin_);
            break;
        case tokenFalse:
            currentValue() = false;
            currentValue().setOffsetStart(token.start_ - begin_);
            currentValue().setOffsetLimit(token.end_ - begin_);
            break;
        case tokenNull:
            currentValue() = Value();
            currentValue().setOffsetStart(token.start_ - begin_);
            currentValue().setOffsetLimit(token.end_ - begin_);
            break;
        case tokenArraySeparator:
            if (features_.allowDroppedNullPlaceholders_) {
                current_--;
                currentValue() = Value();
                currentValue().setOffsetStart(current_ - begin_ - 1);
                currentValue().setOffsetLimit(current_ - begin_);
                break;
            }

        default:
            currentValue().setOffsetStart(token.start_ - begin_);
            currentValue().setOffsetLimit(token.end_ - begin_);
            return addError("Syntax error: value, object or array expected.", token);
    }

    if (collectComments_) {
        lastValueEnd_ = current_;
        lastValue_ = &currentValue();
    }

    return successful;
}

void Reader::skipCommentTokens(Token &token) {
    if (features_.allowComments_) {
        do { readToken(token); } while (token.type_ == tokenComment);
    } else {
        readToken(token);
    }
}

bool Reader::expectToken(TokenType type, Token &token, const char *message) {
    readToken(token);
    if (token.type_ != type) return addError(message, token);
    return true;
}

bool Reader::readToken(Token &token) {
    skipSpaces();
    token.start_ = current_;
    Char c = getNextChar();
    bool ok = true;
    switch (c) {
        case '{':
            token.type_ = tokenObjectBegin;
            break;
        case '}':
            token.type_ = tokenObjectEnd;
            break;
        case '[':
            token.type_ = tokenArrayBegin;
            break;
        case ']':
            token.type_ = tokenArrayEnd;
            break;
        case '"':
            token.type_ = tokenString;
            ok = readString();
            break;
        case '/':
            token.type_ = tokenComment;
            ok = readComment();
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            token.type_ = tokenNumber;
            readNumber();
            break;
        case 't':
            token.type_ = tokenTrue;
            ok = match("rue", 3);
            break;
        case 'f':
            token.type_ = tokenFalse;
            ok = match("alse", 4);
            break;
        case 'n':
            token.type_ = tokenNull;
            ok = match("ull", 3);
            break;
        case ',':
            token.type_ = tokenArraySeparator;
            break;
        case ':':
            token.type_ = tokenMemberSeparator;
            break;
        case 0:
            token.type_ = tokenEndOfStream;
            break;
        default:
            ok = false;
            break;
    }
    if (!ok) token.type_ = tokenError;
    token.end_ = current_;
    return true;
}

void Reader::skipSpaces() {
    while (current_ != end_) {
        Char c = *current_;
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
            ++current_;
        else
            break;
    }
}

bool Reader::match(Location pattern, int patternLength) {
    if (end_ - current_ < patternLength) return false;
    int index = patternLength;
    while (index--)
        if (current_[index] != pattern[index]) return false;
    current_ += patternLength;
    return true;
}

bool Reader::readComment() {
    Location commentBegin = current_ - 1;
    Char c = getNextChar();
    bool successful = false;
    if (c == '*')
        successful = readCStyleComment();
    else if (c == '/')
        successful = readCppStyleComment();
    if (!successful) return false;

    if (collectComments_) {
        CommentPlacement placement = commentBefore;
        if (lastValueEnd_ && !containsNewLine(lastValueEnd_, commentBegin)) {
            if (c != '*' || !containsNewLine(commentBegin, current_)) placement = commentAfterOnSameLine;
        }

        addComment(commentBegin, current_, placement);
    }
    return true;
}

void Reader::addComment(Location begin, Location end, CommentPlacement placement) {
    assert(collectComments_);
    if (placement == commentAfterOnSameLine) {
        assert(lastValue_ != 0);
        lastValue_->setComment(std::string(begin, end), placement);
    } else {
        commentsBefore_ += std::string(begin, end);
    }
}

bool Reader::readCStyleComment() {
    while (current_ != end_) {
        Char c = getNextChar();
        if (c == '*' && *current_ == '/') break;
    }
    return getNextChar() == '/';
}

bool Reader::readCppStyleComment() {
    while (current_ != end_) {
        Char c = getNextChar();
        if (c == '\r' || c == '\n') break;
    }
    return true;
}

void Reader::readNumber() {
    while (current_ != end_) {
        if (!(*current_ >= '0' && *current_ <= '9') && !in(*current_, '.', 'e', 'E', '+', '-')) break;
        ++current_;
    }
}

bool Reader::readString() {
    Char c = 0;
    while (current_ != end_) {
        c = getNextChar();
        if (c == '\\')
            getNextChar();
        else if (c == '"')
            break;
    }
    return c == '"';
}

bool Reader::readObject(Token &tokenStart) {
    Token tokenName;
    std::string name;
    currentValue() = Value(objectValue);
    currentValue().setOffsetStart(tokenStart.start_ - begin_);
    while (readToken(tokenName)) {
        bool initialTokenOk = true;
        while (tokenName.type_ == tokenComment && initialTokenOk) initialTokenOk = readToken(tokenName);
        if (!initialTokenOk) break;
        if (tokenName.type_ == tokenObjectEnd && name.empty()) return true;
        name = "";
        if (tokenName.type_ == tokenString) {
            if (!decodeString(tokenName, name)) return recoverFromError(tokenObjectEnd);
        } else if (tokenName.type_ == tokenNumber && features_.allowNumericKeys_) {
            Value numberName;
            if (!decodeNumber(tokenName, numberName)) return recoverFromError(tokenObjectEnd);
            name = numberName.asString();
        } else {
            break;
        }

        Token colon;
        if (!readToken(colon) || colon.type_ != tokenMemberSeparator) {
            return addErrorAndRecover("Missing ':' after object member name", colon, tokenObjectEnd);
        }
        Value &value = currentValue()[name];
        nodes_.push(&value);
        bool ok = readValue();
        nodes_.pop();
        if (!ok) return recoverFromError(tokenObjectEnd);

        Token comma;
        if (!readToken(comma) ||
            (comma.type_ != tokenObjectEnd && comma.type_ != tokenArraySeparator && comma.type_ != tokenComment)) {
            return addErrorAndRecover("Missing ',' or '}' in object declaration", comma, tokenObjectEnd);
        }
        bool finalizeTokenOk = true;
        while (comma.type_ == tokenComment && finalizeTokenOk) finalizeTokenOk = readToken(comma);
        if (comma.type_ == tokenObjectEnd) return true;
    }
    return addErrorAndRecover("Missing '}' or object member name", tokenName, tokenObjectEnd);
}

bool Reader::readArray(Token &tokenStart) {
    currentValue() = Value(arrayValue);
    currentValue().setOffsetStart(tokenStart.start_ - begin_);
    skipSpaces();
    if (*current_ == ']') {
        Token endArray;
        readToken(endArray);
        return true;
    }
    int index = 0;
    for (;;) {
        Value &value = currentValue()[index++];
        nodes_.push(&value);
        bool ok = readValue();
        nodes_.pop();
        if (!ok) return recoverFromError(tokenArrayEnd);

        Token token;

        ok = readToken(token);
        while (token.type_ == tokenComment && ok) { ok = readToken(token); }
        bool badTokenType = (token.type_ != tokenArraySeparator && token.type_ != tokenArrayEnd);
        if (!ok || badTokenType) {
            return addErrorAndRecover("Missing ',' or ']' in array declaration", token, tokenArrayEnd);
        }
        if (token.type_ == tokenArrayEnd) break;
    }
    return true;
}

bool Reader::decodeNumber(Token &token) {
    Value decoded;
    if (!decodeNumber(token, decoded)) return false;
    currentValue() = decoded;
    currentValue().setOffsetStart(token.start_ - begin_);
    currentValue().setOffsetLimit(token.end_ - begin_);
    return true;
}

bool Reader::decodeNumber(Token &token, Value &decoded) {
    bool isDouble = false;
    for (Location inspect = token.start_; inspect != token.end_; ++inspect) {
        isDouble = isDouble || in(*inspect, '.', 'e', 'E', '+') || (*inspect == '-' && inspect != token.start_);
    }
    if (isDouble) return decodeDouble(token, decoded);

    Location current = token.start_;
    bool isNegative = *current == '-';
    if (isNegative) ++current;
    Value::LargestUInt maxIntegerValue = isNegative ? Value::LargestUInt(-Value::minLargestInt) : Value::maxLargestUInt;
    Value::LargestUInt threshold = maxIntegerValue / 10;
    Value::LargestUInt value = 0;
    while (current < token.end_) {
        Char c = *current++;
        if (c < '0' || c > '9')
            return addError("'" + std::string(token.start_, token.end_) + "' is not a number.", token);
        Value::UInt digit(c - '0');
        if (value >= threshold) {
            if (value > threshold || current != token.end_ || digit > maxIntegerValue % 10) {
                return decodeDouble(token, decoded);
            }
        }
        value = value * 10 + digit;
    }
    if (isNegative)
        decoded = -Value::LargestInt(value);
    else if (value <= Value::LargestUInt(Value::maxInt))
        decoded = Value::LargestInt(value);
    else
        decoded = value;
    return true;
}

bool Reader::decodeDouble(Token &token) {
    Value decoded;
    if (!decodeDouble(token, decoded)) return false;
    currentValue() = decoded;
    currentValue().setOffsetStart(token.start_ - begin_);
    currentValue().setOffsetLimit(token.end_ - begin_);
    return true;
}

bool Reader::decodeDouble(Token &token, Value &decoded) {
    double value = 0;
    const int bufferSize = 32;
    int count;
    int length = int(token.end_ - token.start_);

    if (length < 0) { return addError("Unable to parse token length", token); }

    char format[] = "%lf";

    if (length <= bufferSize) {
        Char buffer[bufferSize + 1];
        memcpy(buffer, token.start_, length);
        buffer[length] = 0;
        count = sscanf(buffer, format, &value);
    } else {
        std::string buffer(token.start_, token.end_);
        count = sscanf(buffer.c_str(), format, &value);
    }

    if (count != 1) return addError("'" + std::string(token.start_, token.end_) + "' is not a number.", token);
    decoded = value;
    return true;
}

bool Reader::decodeString(Token &token) {
    std::string decoded;
    if (!decodeString(token, decoded)) return false;
    currentValue() = decoded;
    currentValue().setOffsetStart(token.start_ - begin_);
    currentValue().setOffsetLimit(token.end_ - begin_);
    return true;
}

bool Reader::decodeString(Token &token, std::string &decoded) {
    decoded.reserve(token.end_ - token.start_ - 2);
    Location current = token.start_ + 1;
    Location end = token.end_ - 1;
    while (current != end) {
        Char c = *current++;
        if (c == '"')
            break;
        else if (c == '\\') {
            if (current == end) return addError("Empty escape sequence in string", token, current);
            Char escape = *current++;
            switch (escape) {
                case '"':
                    decoded += '"';
                    break;
                case '/':
                    decoded += '/';
                    break;
                case '\\':
                    decoded += '\\';
                    break;
                case 'b':
                    decoded += '\b';
                    break;
                case 'f':
                    decoded += '\f';
                    break;
                case 'n':
                    decoded += '\n';
                    break;
                case 'r':
                    decoded += '\r';
                    break;
                case 't':
                    decoded += '\t';
                    break;
                case 'u': {
                    unsigned int unicode;
                    if (!decodeUnicodeCodePoint(token, current, end, unicode)) return false;
                    decoded += codePointToUTF8(unicode);
                } break;
                default:
                    return addError("Bad escape sequence in string", token, current);
            }
        } else {
            decoded += c;
        }
    }
    return true;
}

bool Reader::decodeUnicodeCodePoint(Token &token, Location &current, Location end, unsigned int &unicode) {
    if (!decodeUnicodeEscapeSequence(token, current, end, unicode)) return false;
    if (unicode >= 0xD800 && unicode <= 0xDBFF) {
        if (end - current < 6)
            return addError(
                "additional six characters expected to parse unicode surrogate "
                "pair.",
                token, current);
        unsigned int surrogatePair;
        if (*(current++) == '\\' && *(current++) == 'u') {
            if (decodeUnicodeEscapeSequence(token, current, end, surrogatePair)) {
                unicode = 0x10000 + ((unicode & 0x3FF) << 10) + (surrogatePair & 0x3FF);
            } else
                return false;
        } else
            return addError(
                "expecting another \\u token to begin the second half of "
                "a unicode surrogate pair",
                token, current);
    }
    return true;
}

bool Reader::decodeUnicodeEscapeSequence(Token &token, Location &current, Location end, unsigned int &unicode) {
    if (end - current < 4)
        return addError("Bad unicode escape sequence in string: four digits expected.", token, current);
    unicode = 0;
    for (int index = 0; index < 4; ++index) {
        Char c = *current++;
        unicode *= 16;
        if (c >= '0' && c <= '9')
            unicode += c - '0';
        else if (c >= 'a' && c <= 'f')
            unicode += c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
            unicode += c - 'A' + 10;
        else
            return addError(
                "Bad unicode escape sequence in string: hexadecimal digit "
                "expected.",
                token, current);
    }
    return true;
}

bool Reader::addError(const std::string &message, Token &token, Location extra) {
    ErrorInfo info;
    info.token_ = token;
    info.message_ = message;
    info.extra_ = extra;
    errors_.push_back(info);
    return false;
}

bool Reader::recoverFromError(TokenType skipUntilToken) {
    int errorCount = int(errors_.size());
    Token skip;
    for (;;) {
        if (!readToken(skip)) errors_.resize(errorCount);
        if (skip.type_ == skipUntilToken || skip.type_ == tokenEndOfStream) break;
    }
    errors_.resize(errorCount);
    return false;
}

bool Reader::addErrorAndRecover(const std::string &message, Token &token, TokenType skipUntilToken) {
    addError(message, token);
    return recoverFromError(skipUntilToken);
}

Value &Reader::currentValue() { return *(nodes_.top()); }

Reader::Char Reader::getNextChar() {
    if (current_ == end_) return 0;
    return *current_++;
}

void Reader::getLocationLineAndColumn(Location location, int &line, int &column) const {
    Location current = begin_;
    Location lastLineStart = current;
    line = 0;
    while (current < location && current != end_) {
        Char c = *current++;
        if (c == '\r') {
            if (*current == '\n') ++current;
            lastLineStart = current;
            ++line;
        } else if (c == '\n') {
            lastLineStart = current;
            ++line;
        }
    }

    column = int(location - lastLineStart) + 1;
    ++line;
}

std::string Reader::getLocationLineAndColumn(Location location) const {
    int line, column;
    getLocationLineAndColumn(location, line, column);
    char buffer[18 + 16 + 16 + 1];
#if defined(_MSC_VER) && defined(__STDC_SECURE_LIB__)
#if defined(WINCE)
    _snprintf(buffer, sizeof(buffer), "Line %d, Column %d", line, column);
#else
    sprintf_s(buffer, sizeof(buffer), "Line %d, Column %d", line, column);
#endif
#else
    snprintf(buffer, sizeof(buffer), "Line %d, Column %d", line, column);
#endif
    return buffer;
}

std::string Reader::getFormatedErrorMessages() const { return getFormattedErrorMessages(); }

std::string Reader::getFormattedErrorMessages() const {
    std::string formattedMessage;
    for (Errors::const_iterator itError = errors_.begin(); itError != errors_.end(); ++itError) {
        const ErrorInfo &error = *itError;
        formattedMessage += "* " + getLocationLineAndColumn(error.token_.start_) + "\n";
        formattedMessage += "  " + error.message_ + "\n";
        if (error.extra_) formattedMessage += "See " + getLocationLineAndColumn(error.extra_) + " for detail.\n";
    }
    return formattedMessage;
}

std::vector<Reader::StructuredError> Reader::getStructuredErrors() const {
    std::vector<Reader::StructuredError> allErrors;
    for (Errors::const_iterator itError = errors_.begin(); itError != errors_.end(); ++itError) {
        const ErrorInfo &error = *itError;
        Reader::StructuredError structured;
        structured.offset_start = error.token_.start_ - begin_;
        structured.offset_limit = error.token_.end_ - begin_;
        structured.message = error.message_;
        allErrors.push_back(structured);
    }
    return allErrors;
}

std::istream &operator>>(std::istream &sin, Value &root) {
    Json::Reader reader;
    bool ok = reader.parse(sin, root, true);
    if (!ok) {
        fprintf(stderr, "Error from reader: %s", reader.getFormattedErrorMessages().c_str());

        JSON_FAIL_MESSAGE("reader error");
    }
    return sin;
}

}  // namespace Json

#ifndef JSONCPP_BATCHALLOCATOR_H_INCLUDED
#define JSONCPP_BATCHALLOCATOR_H_INCLUDED

#include <assert.h>
#include <stdlib.h>

#ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION

namespace Json {

template <typename AllocatedType, const unsigned int objectPerAllocation>
class BatchAllocator {
   public:
    BatchAllocator(unsigned int objectsPerPage = 255) : freeHead_(0), objectsPerPage_(objectsPerPage) {
        assert(sizeof(AllocatedType) * objectPerAllocation >= sizeof(AllocatedType *));

        assert(objectsPerPage >= 16);
        batches_ = allocateBatch(0);
        currentBatch_ = batches_;
    }

    ~BatchAllocator() {
        for (BatchInfo *batch = batches_; batch;) {
            BatchInfo *nextBatch = batch->next_;
            free(batch);
            batch = nextBatch;
        }
    }

    AllocatedType *allocate() {
        if (freeHead_) {
            AllocatedType *object = freeHead_;
            freeHead_ = *(AllocatedType **)object;
            return object;
        }
        if (currentBatch_->used_ == currentBatch_->end_) {
            currentBatch_ = currentBatch_->next_;
            while (currentBatch_ && currentBatch_->used_ == currentBatch_->end_) currentBatch_ = currentBatch_->next_;

            if (!currentBatch_) {
                currentBatch_ = allocateBatch(objectsPerPage_);
                currentBatch_->next_ = batches_;
                batches_ = currentBatch_;
            }
        }
        AllocatedType *allocated = currentBatch_->used_;
        currentBatch_->used_ += objectPerAllocation;
        return allocated;
    }

    void release(AllocatedType *object) {
        assert(object != 0);
        *(AllocatedType **)object = freeHead_;
        freeHead_ = object;
    }

   private:
    struct BatchInfo {
        BatchInfo *next_;
        AllocatedType *used_;
        AllocatedType *end_;
        AllocatedType buffer_[objectPerAllocation];
    };

    BatchAllocator(const BatchAllocator &);

    void operator=(const BatchAllocator &);

    static BatchInfo *allocateBatch(unsigned int objectsPerPage) {
        const unsigned int mallocSize = sizeof(BatchInfo) - sizeof(AllocatedType) * objectPerAllocation +
                                        sizeof(AllocatedType) * objectPerAllocation * objectsPerPage;
        BatchInfo *batch = static_cast<BatchInfo *>(malloc(mallocSize));
        batch->next_ = 0;
        batch->used_ = batch->buffer_;
        batch->end_ = batch->buffer_ + objectsPerPage;
        return batch;
    }

    BatchInfo *batches_;
    BatchInfo *currentBatch_;

    AllocatedType *freeHead_;
    unsigned int objectsPerPage_;
};

}  // namespace Json

#endif

#endif

namespace Json {

ValueIteratorBase::ValueIteratorBase()
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    : current_(), isNull_(true) {
}

#else
    : isArray_(true), isNull_(true) {
    iterator_.array_ = ValueInternalArray::IteratorState();
}
#endif

#ifndef JSON_VALUE_USE_INTERNAL_MAP

ValueIteratorBase::ValueIteratorBase(const Value::ObjectValues::iterator &current)
    : current_(current), isNull_(false) {}

#else
ValueIteratorBase::ValueIteratorBase(const ValueInternalArray::IteratorState &state) : isArray_(true) {
    iterator_.array_ = state;
}

ValueIteratorBase::ValueIteratorBase(const ValueInternalMap::IteratorState &state) : isArray_(false) {
    iterator_.map_ = state;
}
#endif

Value &ValueIteratorBase::deref() const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    return current_->second;
#else
    if (isArray_) return ValueInternalArray::dereference(iterator_.array_);
    return ValueInternalMap::value(iterator_.map_);
#endif
}

void ValueIteratorBase::increment() {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    ++current_;
#else
    if (isArray_) ValueInternalArray::increment(iterator_.array_);
    ValueInternalMap::increment(iterator_.map_);
#endif
}

void ValueIteratorBase::decrement() {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    --current_;
#else
    if (isArray_) ValueInternalArray::decrement(iterator_.array_);
    ValueInternalMap::decrement(iterator_.map_);
#endif
}

ValueIteratorBase::difference_type ValueIteratorBase::computeDistance(const SelfType &other) const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
#ifdef JSON_USE_CPPTL_SMALLMAP
    return current_ - other.current_;
#else

    if (isNull_ && other.isNull_) { return 0; }

    difference_type myDistance = 0;
    for (Value::ObjectValues::iterator it = current_; it != other.current_; ++it) { ++myDistance; }
    return myDistance;
#endif
#else
    if (isArray_) return ValueInternalArray::distance(iterator_.array_, other.iterator_.array_);
    return ValueInternalMap::distance(iterator_.map_, other.iterator_.map_);
#endif
}

bool ValueIteratorBase::isEqual(const SelfType &other) const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    if (isNull_) { return other.isNull_; }
    return current_ == other.current_;
#else
    if (isArray_) return ValueInternalArray::equals(iterator_.array_, other.iterator_.array_);
    return ValueInternalMap::equals(iterator_.map_, other.iterator_.map_);
#endif
}

void ValueIteratorBase::copy(const SelfType &other) {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    current_ = other.current_;
    isNull_ = other.isNull_;
#else
    if (isArray_) iterator_.array_ = other.iterator_.array_;
    iterator_.map_ = other.iterator_.map_;
#endif
}

Value ValueIteratorBase::key() const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    const Value::CZString czstring = (*current_).first;
    if (czstring.c_str()) {
        if (czstring.isStaticString()) return Value(StaticString(czstring.c_str()));
        return Value(czstring.c_str());
    }
    return Value(czstring.index());
#else
    if (isArray_) return Value(ValueInternalArray::indexOf(iterator_.array_));
    bool isStatic;
    const char *memberName = ValueInternalMap::key(iterator_.map_, isStatic);
    if (isStatic) return Value(StaticString(memberName));
    return Value(memberName);
#endif
}

UInt ValueIteratorBase::index() const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    const Value::CZString czstring = (*current_).first;
    if (!czstring.c_str()) return czstring.index();
    return Value::UInt(-1);
#else
    if (isArray_) return Value::UInt(ValueInternalArray::indexOf(iterator_.array_));
    return Value::UInt(-1);
#endif
}

const char *ValueIteratorBase::memberName() const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    const char *name = (*current_).first.c_str();
    return name ? name : "";
#else
    if (!isArray_) return ValueInternalMap::key(iterator_.map_);
    return "";
#endif
}

ValueConstIterator::ValueConstIterator() {}

#ifndef JSON_VALUE_USE_INTERNAL_MAP

ValueConstIterator::ValueConstIterator(const Value::ObjectValues::iterator &current) : ValueIteratorBase(current) {}

#else
ValueConstIterator::ValueConstIterator(const ValueInternalArray::IteratorState &state) : ValueIteratorBase(state) {}

ValueConstIterator::ValueConstIterator(const ValueInternalMap::IteratorState &state) : ValueIteratorBase(state) {}
#endif

ValueConstIterator &ValueConstIterator::operator=(const ValueIteratorBase &other) {
    copy(other);
    return *this;
}

ValueIterator::ValueIterator() {}

#ifndef JSON_VALUE_USE_INTERNAL_MAP

ValueIterator::ValueIterator(const Value::ObjectValues::iterator &current) : ValueIteratorBase(current) {}

#else
ValueIterator::ValueIterator(const ValueInternalArray::IteratorState &state) : ValueIteratorBase(state) {}

ValueIterator::ValueIterator(const ValueInternalMap::IteratorState &state) : ValueIteratorBase(state) {}
#endif

ValueIterator::ValueIterator(const ValueConstIterator &other) : ValueIteratorBase(other) {}

ValueIterator::ValueIterator(const ValueIterator &other) : ValueIteratorBase(other) {}

ValueIterator &ValueIterator::operator=(const SelfType &other) {
    copy(other);
    return *this;
}

}  // namespace Json

#if !defined(JSON_IS_AMALGAMATION)
#include <json/assertions.h>
#include <json/value.h>
#include <json/writer.h>
#ifndef JSON_USE_SIMPLE_INTERNAL_ALLOCATOR
#include "json_batchallocator.h"
#endif
#endif

#include <math.h>
#include <cassert>
#include <cstring>
#include <sstream>
#include <utility>

#ifdef JSON_USE_CPPTL
#include <cpptl/conststring.h>
#endif

#include <cstddef>

#define JSON_ASSERT_UNREACHABLE assert(false)

namespace Json {

#if defined(__ARMEL__)
#define ALIGNAS(byte_alignment) __attribute__((aligned(byte_alignment)))
#else
#define ALIGNAS(byte_alignment)
#endif
static const unsigned char ALIGNAS(8) kNull[sizeof(Value)] = {0};
const unsigned char &kNullRef = kNull[0];
const Value &Value::null = reinterpret_cast<const Value &>(kNullRef);

const Int Value::minInt = Int(~(UInt(-1) / 2));
const Int Value::maxInt = Int(UInt(-1) / 2);
const UInt Value::maxUInt = UInt(-1);
#if defined(JSON_HAS_INT64)
const Int64 Value::minInt64 = Int64(~(UInt64(-1) / 2));
const Int64 Value::maxInt64 = Int64(UInt64(-1) / 2);
const UInt64 Value::maxUInt64 = UInt64(-1);

static const double maxUInt64AsDouble = 18446744073709551615.0;
#endif
const LargestInt Value::minLargestInt = LargestInt(~(LargestUInt(-1) / 2));
const LargestInt Value::maxLargestInt = LargestInt(LargestUInt(-1) / 2);
const LargestUInt Value::maxLargestUInt = LargestUInt(-1);

static const unsigned int unknown = (unsigned)-1;

#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)

template <typename T, typename U>
static inline bool InRange(double d, T min, U max) {
    return d >= min && d <= max;
}

#else
static inline double integerToDouble(Json::UInt64 value) {
    return static_cast<double>(Int64(value / 2)) * 2.0 + Int64(value & 1);
}

template <typename T>
static inline double integerToDouble(T value) {
    return static_cast<double>(value);
}

template <typename T, typename U>
static inline bool InRange(double d, T min, U max) {
    return d >= integerToDouble(min) && d <= integerToDouble(max);
}
#endif

static inline char *duplicateStringValue(const char *value, unsigned int length = unknown) {
    if (length == unknown) length = (unsigned int)strlen(value);

    if (length >= (unsigned)Value::maxInt) length = Value::maxInt - 1;

    char *newString = static_cast<char *>(malloc(length + 1));
    JSON_ASSERT_MESSAGE(newString != 0,
                        "in Json::Value::duplicateStringValue(): "
                        "Failed to allocate string value buffer");
    memcpy(newString, value, length);
    newString[length] = 0;
    return newString;
}

static inline void releaseStringValue(char *value) { free(value); }

}  // namespace Json

#if !defined(JSON_IS_AMALGAMATION)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
#include "json_internalarray.inl"
#include "json_internalmap.inl"
#endif

#include "json_valueiterator.inl"
#endif

namespace Json {

Value::CommentInfo::CommentInfo() : comment_(0) {}

Value::CommentInfo::~CommentInfo() {
    if (comment_) releaseStringValue(comment_);
}

void Value::CommentInfo::setComment(const char *text) {
    if (comment_) releaseStringValue(comment_);
    JSON_ASSERT(text != 0);
    JSON_ASSERT_MESSAGE(text[0] == '\0' || text[0] == '/', "in Json::Value::setComment(): Comments must start with /");

    comment_ = duplicateStringValue(text);
}

#ifndef JSON_VALUE_USE_INTERNAL_MAP

Value::CZString::CZString(ArrayIndex index) : cstr_(0), index_(index) {}

Value::CZString::CZString(const char *cstr, DuplicationPolicy allocate)
    : cstr_(allocate == duplicate ? duplicateStringValue(cstr) : cstr), index_(allocate) {}

Value::CZString::CZString(const CZString &other)
    : cstr_(other.index_ != noDuplication && other.cstr_ != 0 ? duplicateStringValue(other.cstr_) : other.cstr_),
      index_(other.cstr_ ? (other.index_ == noDuplication ? noDuplication : duplicate) : other.index_) {}

Value::CZString::~CZString() {
    if (cstr_ && index_ == duplicate) releaseStringValue(const_cast<char *>(cstr_));
}

void Value::CZString::swap(CZString &other) {
    std::swap(cstr_, other.cstr_);
    std::swap(index_, other.index_);
}

Value::CZString &Value::CZString::operator=(CZString other) {
    swap(other);
    return *this;
}

bool Value::CZString::operator<(const CZString &other) const {
    if (cstr_) return strcmp(cstr_, other.cstr_) < 0;
    return index_ < other.index_;
}

bool Value::CZString::operator==(const CZString &other) const {
    if (cstr_) return strcmp(cstr_, other.cstr_) == 0;
    return index_ == other.index_;
}

ArrayIndex Value::CZString::index() const { return index_; }

const char *Value::CZString::c_str() const { return cstr_; }

bool Value::CZString::isStaticString() const { return index_ == noDuplication; }

#endif

Value::Value(ValueType type)
    : type_(type),
      allocated_(false)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
      ,
      itemIsUsed_(0)
#endif
      ,
      comments_(0),
      start_(0),
      limit_(0) {
    switch (type) {
        case nullValue:
            break;
        case intValue:
        case uintValue:
            value_.int_ = 0;
            break;
        case realValue:
            value_.real_ = 0.0;
            break;
        case stringValue:
            value_.string_ = 0;
            break;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
        case objectValue:
            value_.map_ = new ObjectValues();
            break;
#else
        case arrayValue:
            value_.array_ = arrayAllocator()->newArray();
            break;
        case objectValue:
            value_.map_ = mapAllocator()->newMap();
            break;
#endif
        case booleanValue:
            value_.bool_ = false;
            break;
        default:
            JSON_ASSERT_UNREACHABLE;
    }
}

Value::Value(UInt value)
    : type_(uintValue),
      allocated_(false)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
      ,
      itemIsUsed_(0)
#endif
      ,
      comments_(0),
      start_(0),
      limit_(0) {
    value_.uint_ = value;
}

Value::Value(Int value)
    : type_(intValue),
      allocated_(false)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
      ,
      itemIsUsed_(0)
#endif
      ,
      comments_(0),
      start_(0),
      limit_(0) {
    value_.int_ = value;
}

#if defined(JSON_HAS_INT64)

Value::Value(Int64 value)
    : type_(intValue),
      allocated_(false)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
      ,
      itemIsUsed_(0)
#endif
      ,
      comments_(0),
      start_(0),
      limit_(0) {
    value_.int_ = value;
}

Value::Value(UInt64 value)
    : type_(uintValue),
      allocated_(false)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
      ,
      itemIsUsed_(0)
#endif
      ,
      comments_(0),
      start_(0),
      limit_(0) {
    value_.uint_ = value;
}

#endif

Value::Value(double value)
    : type_(realValue),
      allocated_(false)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
      ,
      itemIsUsed_(0)
#endif
      ,
      comments_(0),
      start_(0),
      limit_(0) {
    value_.real_ = value;
}

Value::Value(const char *value)
    : type_(stringValue),
      allocated_(true)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
      ,
      itemIsUsed_(0)
#endif
      ,
      comments_(0),
      start_(0),
      limit_(0) {
    value_.string_ = duplicateStringValue(value);
}

Value::Value(const char *beginValue, const char *endValue)
    : type_(stringValue),
      allocated_(true)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
      ,
      itemIsUsed_(0)
#endif
      ,
      comments_(0),
      start_(0),
      limit_(0) {
    value_.string_ = duplicateStringValue(beginValue, (unsigned int)(endValue - beginValue));
}

Value::Value(const std::string &value)
    : type_(stringValue),
      allocated_(true)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
      ,
      itemIsUsed_(0)
#endif
      ,
      comments_(0),
      start_(0),
      limit_(0) {
    value_.string_ = duplicateStringValue(value.c_str(), (unsigned int)value.length());
}

Value::Value(const StaticString &value)
    : type_(stringValue),
      allocated_(false)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
      ,
      itemIsUsed_(0)
#endif
      ,
      comments_(0),
      start_(0),
      limit_(0) {
    value_.string_ = const_cast<char *>(value.c_str());
}

#ifdef JSON_USE_CPPTL
Value::Value(const CppTL::ConstString &value)
    : type_(stringValue),
      allocated_(true)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
      ,
      itemIsUsed_(0)
#endif
      ,
      comments_(0),
      start_(0),
      limit_(0) {
    value_.string_ = duplicateStringValue(value, value.length());
}
#endif

Value::Value(bool value)
    : type_(booleanValue),
      allocated_(false)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
      ,
      itemIsUsed_(0)
#endif
      ,
      comments_(0),
      start_(0),
      limit_(0) {
    value_.bool_ = value;
}

Value::Value(const Value &other)
    : type_(other.type_),
      allocated_(false)
#ifdef JSON_VALUE_USE_INTERNAL_MAP
      ,
      itemIsUsed_(0)
#endif
      ,
      comments_(0),
      start_(other.start_),
      limit_(other.limit_) {
    switch (type_) {
        case nullValue:
        case intValue:
        case uintValue:
        case realValue:
        case booleanValue:
            value_ = other.value_;
            break;
        case stringValue:
            if (other.value_.string_) {
                value_.string_ = duplicateStringValue(other.value_.string_);
                allocated_ = true;
            } else {
                value_.string_ = 0;
                allocated_ = false;
            }
            break;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
        case objectValue:
            value_.map_ = new ObjectValues(*other.value_.map_);
            break;
#else
        case arrayValue:
            value_.array_ = arrayAllocator()->newArrayCopy(*other.value_.array_);
            break;
        case objectValue:
            value_.map_ = mapAllocator()->newMapCopy(*other.value_.map_);
            break;
#endif
        default:
            JSON_ASSERT_UNREACHABLE;
    }
    if (other.comments_) {
        comments_ = new CommentInfo[numberOfCommentPlacement];
        for (int comment = 0; comment < numberOfCommentPlacement; ++comment) {
            const CommentInfo &otherComment = other.comments_[comment];
            if (otherComment.comment_) comments_[comment].setComment(otherComment.comment_);
        }
    }
}

Value::~Value() {
    switch (type_) {
        case nullValue:
        case intValue:
        case uintValue:
        case realValue:
        case booleanValue:
            break;
        case stringValue:
            if (allocated_) releaseStringValue(value_.string_);
            break;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
        case objectValue:
            delete value_.map_;
            break;
#else
        case arrayValue:
            arrayAllocator()->destructArray(value_.array_);
            break;
        case objectValue:
            mapAllocator()->destructMap(value_.map_);
            break;
#endif
        default:
            JSON_ASSERT_UNREACHABLE;
    }

    if (comments_) delete[] comments_;
}

Value &Value::operator=(Value other) {
    swap(other);
    return *this;
}

void Value::swap(Value &other) {
    ValueType temp = type_;
    type_ = other.type_;
    other.type_ = temp;
    std::swap(value_, other.value_);
    int temp2 = allocated_;
    allocated_ = other.allocated_;
    other.allocated_ = temp2;
    std::swap(start_, other.start_);
    std::swap(limit_, other.limit_);
}

ValueType Value::type() const { return type_; }

int Value::compare(const Value &other) const {
    if (*this < other) return -1;
    if (*this > other) return 1;
    return 0;
}

bool Value::operator<(const Value &other) const {
    int typeDelta = type_ - other.type_;
    if (typeDelta) return typeDelta < 0 ? true : false;
    switch (type_) {
        case nullValue:
            return false;
        case intValue:
            return value_.int_ < other.value_.int_;
        case uintValue:
            return value_.uint_ < other.value_.uint_;
        case realValue:
            return value_.real_ < other.value_.real_;
        case booleanValue:
            return value_.bool_ < other.value_.bool_;
        case stringValue:
            return (value_.string_ == 0 && other.value_.string_) ||
                   (other.value_.string_ && value_.string_ && strcmp(value_.string_, other.value_.string_) < 0);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
        case objectValue: {
            int delta = int(value_.map_->size() - other.value_.map_->size());
            if (delta) return delta < 0;
            return (*value_.map_) < (*other.value_.map_);
        }
#else
        case arrayValue:
            return value_.array_->compare(*(other.value_.array_)) < 0;
        case objectValue:
            return value_.map_->compare(*(other.value_.map_)) < 0;
#endif
        default:
            JSON_ASSERT_UNREACHABLE;
    }
    return false;
}

bool Value::operator<=(const Value &other) const { return !(other < *this); }

bool Value::operator>=(const Value &other) const { return !(*this < other); }

bool Value::operator>(const Value &other) const { return other < *this; }

bool Value::operator==(const Value &other) const {
    int temp = other.type_;
    if (type_ != temp) return false;
    switch (type_) {
        case nullValue:
            return true;
        case intValue:
            return value_.int_ == other.value_.int_;
        case uintValue:
            return value_.uint_ == other.value_.uint_;
        case realValue:
            return value_.real_ == other.value_.real_;
        case booleanValue:
            return value_.bool_ == other.value_.bool_;
        case stringValue:
            return (value_.string_ == other.value_.string_) ||
                   (other.value_.string_ && value_.string_ && strcmp(value_.string_, other.value_.string_) == 0);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
        case objectValue:
            return value_.map_->size() == other.value_.map_->size() && (*value_.map_) == (*other.value_.map_);
#else
        case arrayValue:
            return value_.array_->compare(*(other.value_.array_)) == 0;
        case objectValue:
            return value_.map_->compare(*(other.value_.map_)) == 0;
#endif
        default:
            JSON_ASSERT_UNREACHABLE;
    }
    return false;
}

bool Value::operator!=(const Value &other) const { return !(*this == other); }

const char *Value::asCString() const {
    JSON_ASSERT_MESSAGE(type_ == stringValue, "in Json::Value::asCString(): requires stringValue");
    return value_.string_;
}

std::string Value::asString() const {
    switch (type_) {
        case nullValue:
            return "";
        case stringValue:
            return value_.string_ ? value_.string_ : "";
        case booleanValue:
            return value_.bool_ ? "true" : "false";
        case intValue:
            return valueToString(value_.int_);
        case uintValue:
            return valueToString(value_.uint_);
        case realValue:
            return valueToString(value_.real_);
        default:
            JSON_FAIL_MESSAGE("Type is not convertible to string");
    }
}

#ifdef JSON_USE_CPPTL
CppTL::ConstString Value::asConstString() const { return CppTL::ConstString(asString().c_str()); }
#endif

Value::Int Value::asInt() const {
    switch (type_) {
        case intValue:
            JSON_ASSERT_MESSAGE(isInt(), "LargestInt out of Int range");
            return Int(value_.int_);
        case uintValue:
            JSON_ASSERT_MESSAGE(isInt(), "LargestUInt out of Int range");
            return Int(value_.uint_);
        case realValue:
            JSON_ASSERT_MESSAGE(InRange(value_.real_, minInt, maxInt), "double out of Int range");
            return Int(value_.real_);
        case nullValue:
            return 0;
        case booleanValue:
            return value_.bool_ ? 1 : 0;
        default:
            break;
    }
    JSON_FAIL_MESSAGE("Value is not convertible to Int.");
}

Value::UInt Value::asUInt() const {
    switch (type_) {
        case intValue:
            JSON_ASSERT_MESSAGE(isUInt(), "LargestInt out of UInt range");
            return UInt(value_.int_);
        case uintValue:
            JSON_ASSERT_MESSAGE(isUInt(), "LargestUInt out of UInt range");
            return UInt(value_.uint_);
        case realValue:
            JSON_ASSERT_MESSAGE(InRange(value_.real_, 0, maxUInt), "double out of UInt range");
            return UInt(value_.real_);
        case nullValue:
            return 0;
        case booleanValue:
            return value_.bool_ ? 1 : 0;
        default:
            break;
    }
    JSON_FAIL_MESSAGE("Value is not convertible to UInt.");
}

#if defined(JSON_HAS_INT64)

Value::Int64 Value::asInt64() const {
    switch (type_) {
        case intValue:
            return Int64(value_.int_);
        case uintValue:
            JSON_ASSERT_MESSAGE(isInt64(), "LargestUInt out of Int64 range");
            return Int64(value_.uint_);
        case realValue:
            JSON_ASSERT_MESSAGE(InRange(value_.real_, minInt64, maxInt64), "double out of Int64 range");
            return Int64(value_.real_);
        case nullValue:
            return 0;
        case booleanValue:
            return value_.bool_ ? 1 : 0;
        default:
            break;
    }
    JSON_FAIL_MESSAGE("Value is not convertible to Int64.");
}

Value::UInt64 Value::asUInt64() const {
    switch (type_) {
        case intValue:
            JSON_ASSERT_MESSAGE(isUInt64(), "LargestInt out of UInt64 range");
            return UInt64(value_.int_);
        case uintValue:
            return UInt64(value_.uint_);
        case realValue:
            JSON_ASSERT_MESSAGE(InRange(value_.real_, 0, maxUInt64), "double out of UInt64 range");
            return UInt64(value_.real_);
        case nullValue:
            return 0;
        case booleanValue:
            return value_.bool_ ? 1 : 0;
        default:
            break;
    }
    JSON_FAIL_MESSAGE("Value is not convertible to UInt64.");
}

#endif

LargestInt Value::asLargestInt() const {
#if defined(JSON_NO_INT64)
    return asInt();
#else
    return asInt64();
#endif
}

LargestUInt Value::asLargestUInt() const {
#if defined(JSON_NO_INT64)
    return asUInt();
#else
    return asUInt64();
#endif
}

double Value::asDouble() const {
    switch (type_) {
        case intValue:
            return static_cast<double>(value_.int_);
        case uintValue:
#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
            return static_cast<double>(value_.uint_);
#else
            return integerToDouble(value_.uint_);
#endif
        case realValue:
            return value_.real_;
        case nullValue:
            return 0.0;
        case booleanValue:
            return value_.bool_ ? 1.0 : 0.0;
        default:
            break;
    }
    JSON_FAIL_MESSAGE("Value is not convertible to double.");
}

float Value::asFloat() const {
    switch (type_) {
        case intValue:
            return static_cast<float>(value_.int_);
        case uintValue:
#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
            return static_cast<float>(value_.uint_);
#else
            return integerToDouble(value_.uint_);
#endif
        case realValue:
            return static_cast<float>(value_.real_);
        case nullValue:
            return 0.0;
        case booleanValue:
            return value_.bool_ ? 1.0f : 0.0f;
        default:
            break;
    }
    JSON_FAIL_MESSAGE("Value is not convertible to float.");
}

bool Value::asBool() const {
    switch (type_) {
        case booleanValue:
            return value_.bool_;
        case nullValue:
            return false;
        case intValue:
            return value_.int_ ? true : false;
        case uintValue:
            return value_.uint_ ? true : false;
        case realValue:
            return value_.real_ ? true : false;
        default:
            break;
    }
    JSON_FAIL_MESSAGE("Value is not convertible to bool.");
}

bool Value::isConvertibleTo(ValueType other) const {
    switch (other) {
        case nullValue:
            return (isNumeric() && asDouble() == 0.0) || (type_ == booleanValue && value_.bool_ == false) ||
                   (type_ == stringValue && asString() == "") || (type_ == arrayValue && value_.map_->size() == 0) ||
                   (type_ == objectValue && value_.map_->size() == 0) || type_ == nullValue;
        case intValue:
            return isInt() || (type_ == realValue && InRange(value_.real_, minInt, maxInt)) || type_ == booleanValue ||
                   type_ == nullValue;
        case uintValue:
            return isUInt() || (type_ == realValue && InRange(value_.real_, 0, maxUInt)) || type_ == booleanValue ||
                   type_ == nullValue;
        case realValue:
            return isNumeric() || type_ == booleanValue || type_ == nullValue;
        case booleanValue:
            return isNumeric() || type_ == booleanValue || type_ == nullValue;
        case stringValue:
            return isNumeric() || type_ == booleanValue || type_ == stringValue || type_ == nullValue;
        case arrayValue:
            return type_ == arrayValue || type_ == nullValue;
        case objectValue:
            return type_ == objectValue || type_ == nullValue;
    }
    JSON_ASSERT_UNREACHABLE;
    return false;
}

ArrayIndex Value::size() const {
    switch (type_) {
        case nullValue:
        case intValue:
        case uintValue:
        case realValue:
        case booleanValue:
        case stringValue:
            return 0;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
            if (!value_.map_->empty()) {
                ObjectValues::const_iterator itLast = value_.map_->end();
                --itLast;
                return (*itLast).first.index() + 1;
            }
            return 0;
        case objectValue:
            return ArrayIndex(value_.map_->size());
#else
        case arrayValue:
            return Int(value_.array_->size());
        case objectValue:
            return Int(value_.map_->size());
#endif
    }
    JSON_ASSERT_UNREACHABLE;
    return 0;
}

bool Value::empty() const {
    if (isNull() || isArray() || isObject())
        return size() == 0u;
    else
        return false;
}

bool Value::operator!() const { return isNull(); }

void Value::clear() {
    JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == arrayValue || type_ == objectValue,
                        "in Json::Value::clear(): requires complex value");
    start_ = 0;
    limit_ = 0;
    switch (type_) {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
        case objectValue:
            value_.map_->clear();
            break;
#else
        case arrayValue:
            value_.array_->clear();
            break;
        case objectValue:
            value_.map_->clear();
            break;
#endif
        default:
            break;
    }
}

void Value::resize(ArrayIndex newSize) {
    JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == arrayValue, "in Json::Value::resize(): requires arrayValue");
    if (type_ == nullValue) *this = Value(arrayValue);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    ArrayIndex oldSize = size();
    if (newSize == 0)
        clear();
    else if (newSize > oldSize)
        (*this)[newSize - 1];
    else {
        for (ArrayIndex index = newSize; index < oldSize; ++index) { value_.map_->erase(index); }
        assert(size() == newSize);
    }
#else
    value_.array_->resize(newSize);
#endif
}

Value &Value::operator[](ArrayIndex index) {
    JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == arrayValue,
                        "in Json::Value::operator[](ArrayIndex): requires arrayValue");
    if (type_ == nullValue) *this = Value(arrayValue);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    CZString key(index);
    ObjectValues::iterator it = value_.map_->lower_bound(key);
    if (it != value_.map_->end() && (*it).first == key) return (*it).second;

    ObjectValues::value_type defaultValue(key, null);
    it = value_.map_->insert(it, defaultValue);
    return (*it).second;
#else
    return value_.array_->resolveReference(index);
#endif
}

Value &Value::operator[](int index) {
    JSON_ASSERT_MESSAGE(index >= 0, "in Json::Value::operator[](int index): index cannot be negative");
    return (*this)[ArrayIndex(index)];
}

const Value &Value::operator[](ArrayIndex index) const {
    JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == arrayValue,
                        "in Json::Value::operator[](ArrayIndex)const: requires arrayValue");
    if (type_ == nullValue) return null;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    CZString key(index);
    ObjectValues::const_iterator it = value_.map_->find(key);
    if (it == value_.map_->end()) return null;
    return (*it).second;
#else
    Value *value = value_.array_->find(index);
    return value ? *value : null;
#endif
}

const Value &Value::operator[](int index) const {
    JSON_ASSERT_MESSAGE(index >= 0,
                        "in Json::Value::operator[](int index) const: index "
                        "cannot be negative");
    return (*this)[ArrayIndex(index)];
}

Value &Value::operator[](const char *key) { return resolveReference(key, false); }

Value &Value::resolveReference(const char *key, bool isStatic) {
    JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == objectValue,
                        "in Json::Value::resolveReference(): requires objectValue");
    if (type_ == nullValue) *this = Value(objectValue);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    CZString actualKey(key, isStatic ? CZString::noDuplication : CZString::duplicateOnCopy);
    ObjectValues::iterator it = value_.map_->lower_bound(actualKey);
    if (it != value_.map_->end() && (*it).first == actualKey) return (*it).second;

    ObjectValues::value_type defaultValue(actualKey, null);
    it = value_.map_->insert(it, defaultValue);
    Value &value = (*it).second;
    return value;
#else
    return value_.map_->resolveReference(key, isStatic);
#endif
}

Value Value::get(ArrayIndex index, const Value &defaultValue) const {
    const Value *value = &((*this)[index]);
    return value == &null ? defaultValue : *value;
}

bool Value::isValidIndex(ArrayIndex index) const { return index < size(); }

const Value &Value::operator[](const char *key) const {
    JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == objectValue,
                        "in Json::Value::operator[](char const*)const: requires objectValue");
    if (type_ == nullValue) return null;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    CZString actualKey(key, CZString::noDuplication);
    ObjectValues::const_iterator it = value_.map_->find(actualKey);
    if (it == value_.map_->end()) return null;
    return (*it).second;
#else
    const Value *value = value_.map_->find(key);
    return value ? *value : null;
#endif
}

Value &Value::operator[](const std::string &key) { return (*this)[key.c_str()]; }

const Value &Value::operator[](const std::string &key) const { return (*this)[key.c_str()]; }

Value &Value::operator[](const StaticString &key) { return resolveReference(key, true); }

#ifdef JSON_USE_CPPTL
Value &Value::operator[](const CppTL::ConstString &key) { return (*this)[key.c_str()]; }

const Value &Value::operator[](const CppTL::ConstString &key) const { return (*this)[key.c_str()]; }
#endif

Value &Value::append(const Value &value) { return (*this)[size()] = value; }

Value Value::get(const char *key, const Value &defaultValue) const {
    const Value *value = &((*this)[key]);
    return value == &null ? defaultValue : *value;
}

Value Value::get(const std::string &key, const Value &defaultValue) const { return get(key.c_str(), defaultValue); }

Value Value::removeMember(const char *key) {
    JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == objectValue,
                        "in Json::Value::removeMember(): requires objectValue");
    if (type_ == nullValue) return null;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    CZString actualKey(key, CZString::noDuplication);
    ObjectValues::iterator it = value_.map_->find(actualKey);
    if (it == value_.map_->end()) return null;
    Value old(it->second);
    value_.map_->erase(it);
    return old;
#else
    Value *value = value_.map_->find(key);
    if (value) {
        Value old(*value);
        value_.map_.remove(key);
        return old;
    } else {
        return null;
    }
#endif
}

Value Value::removeMember(const std::string &key) { return removeMember(key.c_str()); }

#ifdef JSON_USE_CPPTL
Value Value::get(const CppTL::ConstString &key, const Value &defaultValue) const {
    return get(key.c_str(), defaultValue);
}
#endif

bool Value::isMember(const char *key) const {
    const Value *value = &((*this)[key]);
    return value != &null;
}

bool Value::isMember(const std::string &key) const { return isMember(key.c_str()); }

#ifdef JSON_USE_CPPTL
bool Value::isMember(const CppTL::ConstString &key) const { return isMember(key.c_str()); }
#endif

Value::Members Value::getMemberNames() const {
    JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == objectValue,
                        "in Json::Value::getMemberNames(), value must be objectValue");
    if (type_ == nullValue) return Value::Members();
    Members members;
    members.reserve(value_.map_->size());
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    ObjectValues::const_iterator it = value_.map_->begin();
    ObjectValues::const_iterator itEnd = value_.map_->end();
    for (; it != itEnd; ++it) members.push_back(std::string((*it).first.c_str()));
#else
    ValueInternalMap::IteratorState it;
    ValueInternalMap::IteratorState itEnd;
    value_.map_->makeBeginIterator(it);
    value_.map_->makeEndIterator(itEnd);
    for (; !ValueInternalMap::equals(it, itEnd); ValueInternalMap::increment(it))
        members.push_back(std::string(ValueInternalMap::key(it)));
#endif
    return members;
}

static bool IsIntegral(double d) {
    double integral_part;
    return modf(d, &integral_part) == 0.0;
}

bool Value::isNull() const { return type_ == nullValue; }

bool Value::isBool() const { return type_ == booleanValue; }

bool Value::isInt() const {
    switch (type_) {
        case intValue:
            return value_.int_ >= minInt && value_.int_ <= maxInt;
        case uintValue:
            return value_.uint_ <= UInt(maxInt);
        case realValue:
            return value_.real_ >= minInt && value_.real_ <= maxInt && IsIntegral(value_.real_);
        default:
            break;
    }
    return false;
}

bool Value::isUInt() const {
    switch (type_) {
        case intValue:
            return value_.int_ >= 0 && LargestUInt(value_.int_) <= LargestUInt(maxUInt);
        case uintValue:
            return value_.uint_ <= maxUInt;
        case realValue:
            return value_.real_ >= 0 && value_.real_ <= maxUInt && IsIntegral(value_.real_);
        default:
            break;
    }
    return false;
}

bool Value::isInt64() const {
#if defined(JSON_HAS_INT64)
    switch (type_) {
        case intValue:
            return true;
        case uintValue:
            return value_.uint_ <= UInt64(maxInt64);
        case realValue:

            return value_.real_ >= double(minInt64) && value_.real_ < double(maxInt64) && IsIntegral(value_.real_);
        default:
            break;
    }
#endif
    return false;
}

bool Value::isUInt64() const {
#if defined(JSON_HAS_INT64)
    switch (type_) {
        case intValue:
            return value_.int_ >= 0;
        case uintValue:
            return true;
        case realValue:

            return value_.real_ >= 0 && value_.real_ < maxUInt64AsDouble && IsIntegral(value_.real_);
        default:
            break;
    }
#endif
    return false;
}

bool Value::isIntegral() const {
#if defined(JSON_HAS_INT64)
    return isInt64() || isUInt64();
#else
    return isInt() || isUInt();
#endif
}

bool Value::isDouble() const { return type_ == realValue || isIntegral(); }

bool Value::isNumeric() const { return isIntegral() || isDouble(); }

bool Value::isString() const { return type_ == stringValue; }

bool Value::isArray() const { return type_ == arrayValue; }

bool Value::isObject() const { return type_ == objectValue; }

void Value::setComment(const char *comment, CommentPlacement placement) {
    if (!comments_) comments_ = new CommentInfo[numberOfCommentPlacement];
    comments_[placement].setComment(comment);
}

void Value::setComment(const std::string &comment, CommentPlacement placement) {
    setComment(comment.c_str(), placement);
}

bool Value::hasComment(CommentPlacement placement) const {
    return comments_ != 0 && comments_[placement].comment_ != 0;
}

std::string Value::getComment(CommentPlacement placement) const {
    if (hasComment(placement)) return comments_[placement].comment_;
    return "";
}

void Value::setOffsetStart(size_t start) { start_ = start; }

void Value::setOffsetLimit(size_t limit) { limit_ = limit; }

size_t Value::getOffsetStart() const { return start_; }

size_t Value::getOffsetLimit() const { return limit_; }

std::string Value::toStyledString() const {
    StyledWriter writer;
    return writer.write(*this);
}

Value::const_iterator Value::begin() const {
    switch (type_) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
            if (value_.array_) {
                ValueInternalArray::IteratorState it;
                value_.array_->makeBeginIterator(it);
                return const_iterator(it);
            }
            break;
        case objectValue:
            if (value_.map_) {
                ValueInternalMap::IteratorState it;
                value_.map_->makeBeginIterator(it);
                return const_iterator(it);
            }
            break;
#else
        case arrayValue:
        case objectValue:
            if (value_.map_) return const_iterator(value_.map_->begin());
            break;
#endif
        default:
            break;
    }
    return const_iterator();
}

Value::const_iterator Value::end() const {
    switch (type_) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
            if (value_.array_) {
                ValueInternalArray::IteratorState it;
                value_.array_->makeEndIterator(it);
                return const_iterator(it);
            }
            break;
        case objectValue:
            if (value_.map_) {
                ValueInternalMap::IteratorState it;
                value_.map_->makeEndIterator(it);
                return const_iterator(it);
            }
            break;
#else
        case arrayValue:
        case objectValue:
            if (value_.map_) return const_iterator(value_.map_->end());
            break;
#endif
        default:
            break;
    }
    return const_iterator();
}

Value::iterator Value::begin() {
    switch (type_) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
            if (value_.array_) {
                ValueInternalArray::IteratorState it;
                value_.array_->makeBeginIterator(it);
                return iterator(it);
            }
            break;
        case objectValue:
            if (value_.map_) {
                ValueInternalMap::IteratorState it;
                value_.map_->makeBeginIterator(it);
                return iterator(it);
            }
            break;
#else
        case arrayValue:
        case objectValue:
            if (value_.map_) return iterator(value_.map_->begin());
            break;
#endif
        default:
            break;
    }
    return iterator();
}

Value::iterator Value::end() {
    switch (type_) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
            if (value_.array_) {
                ValueInternalArray::IteratorState it;
                value_.array_->makeEndIterator(it);
                return iterator(it);
            }
            break;
        case objectValue:
            if (value_.map_) {
                ValueInternalMap::IteratorState it;
                value_.map_->makeEndIterator(it);
                return iterator(it);
            }
            break;
#else
        case arrayValue:
        case objectValue:
            if (value_.map_) return iterator(value_.map_->end());
            break;
#endif
        default:
            break;
    }
    return iterator();
}

PathArgument::PathArgument() : key_(), index_(), kind_(kindNone) {}

PathArgument::PathArgument(ArrayIndex index) : key_(), index_(index), kind_(kindIndex) {}

PathArgument::PathArgument(const char *key) : key_(key), index_(), kind_(kindKey) {}

PathArgument::PathArgument(const std::string &key) : key_(key.c_str()), index_(), kind_(kindKey) {}

Path::Path(const std::string &path,
           const PathArgument &a1,
           const PathArgument &a2,
           const PathArgument &a3,
           const PathArgument &a4,
           const PathArgument &a5) {
    InArgs in;
    in.push_back(&a1);
    in.push_back(&a2);
    in.push_back(&a3);
    in.push_back(&a4);
    in.push_back(&a5);
    makePath(path, in);
}

void Path::makePath(const std::string &path, const InArgs &in) {
    const char *current = path.c_str();
    const char *end = current + path.length();
    InArgs::const_iterator itInArg = in.begin();
    while (current != end) {
        if (*current == '[') {
            ++current;
            if (*current == '%')
                addPathInArg(path, in, itInArg, PathArgument::kindIndex);
            else {
                ArrayIndex index = 0;
                for (; current != end && *current >= '0' && *current <= '9'; ++current)
                    index = index * 10 + ArrayIndex(*current - '0');
                args_.push_back(index);
            }
            if (current == end || *current++ != ']') invalidPath(path, int(current - path.c_str()));
        } else if (*current == '%') {
            addPathInArg(path, in, itInArg, PathArgument::kindKey);
            ++current;
        } else if (*current == '.') {
            ++current;
        } else {
            const char *beginName = current;
            while (current != end && !strchr("[.", *current)) ++current;
            args_.push_back(std::string(beginName, current));
        }
    }
}

void Path::addPathInArg(const std::string &,
                        const InArgs &in,
                        InArgs::const_iterator &itInArg,
                        PathArgument::Kind kind) {
    if (itInArg == in.end()) {
    } else if ((*itInArg)->kind_ != kind) {
    } else {
        args_.push_back(**itInArg);
    }
}

void Path::invalidPath(const std::string &, int) {}

const Value &Path::resolve(const Value &root) const {
    const Value *node = &root;
    for (Args::const_iterator it = args_.begin(); it != args_.end(); ++it) {
        const PathArgument &arg = *it;
        if (arg.kind_ == PathArgument::kindIndex) {
            if (!node->isArray() || !node->isValidIndex(arg.index_)) {}
            node = &((*node)[arg.index_]);
        } else if (arg.kind_ == PathArgument::kindKey) {
            if (!node->isObject()) {}
            node = &((*node)[arg.key_]);
            if (node == &Value::null) {}
        }
    }
    return *node;
}

Value Path::resolve(const Value &root, const Value &defaultValue) const {
    const Value *node = &root;
    for (Args::const_iterator it = args_.begin(); it != args_.end(); ++it) {
        const PathArgument &arg = *it;
        if (arg.kind_ == PathArgument::kindIndex) {
            if (!node->isArray() || !node->isValidIndex(arg.index_)) return defaultValue;
            node = &((*node)[arg.index_]);
        } else if (arg.kind_ == PathArgument::kindKey) {
            if (!node->isObject()) return defaultValue;
            node = &((*node)[arg.key_]);
            if (node == &Value::null) return defaultValue;
        }
    }
    return *node;
}

Value &Path::make(Value &root) const {
    Value *node = &root;
    for (Args::const_iterator it = args_.begin(); it != args_.end(); ++it) {
        const PathArgument &arg = *it;
        if (arg.kind_ == PathArgument::kindIndex) {
            if (!node->isArray()) {}
            node = &((*node)[arg.index_]);
        } else if (arg.kind_ == PathArgument::kindKey) {
            if (!node->isObject()) {}
            node = &((*node)[arg.key_]);
        }
    }
    return *node;
}

}  // namespace Json

#if !defined(JSON_IS_AMALGAMATION)
#include <json/writer.h>
#include "json_tool.h"
#endif

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <iomanip>
#include <sstream>
#include <utility>

#if defined(_MSC_VER) && _MSC_VER < 1500
#include <float.h>
#define isfinite _finite
#define snprintf _snprintf
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1400

#pragma warning(disable : 4996)
#endif

namespace Json {

static bool containsControlCharacter(const char *str) {
    while (*str) {
        if (isControlCharacter(*(str++))) return true;
    }
    return false;
}

std::string valueToString(LargestInt value) {
    UIntToStringBuffer buffer;
    char *current = buffer + sizeof(buffer);
    bool isNegative = value < 0;
    if (isNegative) value = -value;
    uintToString(LargestUInt(value), current);
    if (isNegative) *--current = '-';
    assert(current >= buffer);
    return current;
}

std::string valueToString(LargestUInt value) {
    UIntToStringBuffer buffer;
    char *current = buffer + sizeof(buffer);
    uintToString(value, current);
    assert(current >= buffer);
    return current;
}

#if defined(JSON_HAS_INT64)

std::string valueToString(Int value) { return valueToString(LargestInt(value)); }

std::string valueToString(UInt value) { return valueToString(LargestUInt(value)); }

#endif

std::string valueToString(double value) {
    char buffer[32];
    int len = -1;

#if defined(_MSC_VER) && defined(__STDC_SECURE_LIB__)

#if defined(WINCE)
    len = _snprintf(buffer, sizeof(buffer), "%.16g", value);
#else
    len = sprintf_s(buffer, sizeof(buffer), "%.16g", value);
#endif
#else
    using namespace std;
    if (isfinite(value)) {
        len = snprintf(buffer, sizeof(buffer), "%.16g", value);
    } else {
        if (value != value) {
            len = snprintf(buffer, sizeof(buffer), "null");
        } else if (value < 0) {
            len = snprintf(buffer, sizeof(buffer), "-1e+9999");
        } else {
            len = snprintf(buffer, sizeof(buffer), "1e+9999");
        }
    }
#endif
    assert(len >= 0);
    fixNumericLocale(buffer, buffer + len);
    return buffer;
}

std::string valueToString(bool value) { return value ? "true" : "false"; }

std::string valueToQuotedString(const char *value) {
    if (value == NULL) return "";

    if (strpbrk(value, "\"\\\b\f\n\r\t") == NULL && !containsControlCharacter(value))
        return std::string("\"") + value + "\"";

    std::string::size_type maxsize = strlen(value) * 2 + 3;
    std::string result;
    result.reserve(maxsize);
    result += "\"";
    for (const char *c = value; *c != 0; ++c) {
        switch (*c) {
            case '\"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\b':
                result += "\\b";
                break;
            case '\f':
                result += "\\f";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;

            default:
                if (isControlCharacter(*c)) {
                    std::ostringstream oss;
                    oss << "\\u" << std::hex << std::uppercase << std::setfill('0') << std::setw(4)
                        << static_cast<int>(*c);
                    result += oss.str();
                } else {
                    result += *c;
                }
                break;
        }
    }
    result += "\"";
    return result;
}

Writer::~Writer() {}

FastWriter::FastWriter() : yamlCompatiblityEnabled_(false), dropNullPlaceholders_(false), omitEndingLineFeed_(false) {}

void FastWriter::enableYAMLCompatibility() { yamlCompatiblityEnabled_ = true; }

void FastWriter::dropNullPlaceholders() { dropNullPlaceholders_ = true; }

void FastWriter::omitEndingLineFeed() { omitEndingLineFeed_ = true; }

std::string FastWriter::write(const Value &root) {
    document_ = "";
    writeValue(root);
    if (!omitEndingLineFeed_) document_ += "\n";
    return document_;
}

void FastWriter::writeValue(const Value &value) {
    switch (value.type()) {
        case nullValue:
            if (!dropNullPlaceholders_) document_ += "null";
            break;
        case intValue:
            document_ += valueToString(value.asLargestInt());
            break;
        case uintValue:
            document_ += valueToString(value.asLargestUInt());
            break;
        case realValue:
            document_ += valueToString(value.asDouble());
            break;
        case stringValue:
            document_ += valueToQuotedString(value.asCString());
            break;
        case booleanValue:
            document_ += valueToString(value.asBool());
            break;
        case arrayValue: {
            document_ += "[";
            int size = value.size();
            for (int index = 0; index < size; ++index) {
                if (index > 0) document_ += ",";
                writeValue(value[index]);
            }
            document_ += "]";
        } break;
        case objectValue: {
            Value::Members members(value.getMemberNames());
            document_ += "{";
            for (Value::Members::iterator it = members.begin(); it != members.end(); ++it) {
                const std::string &name = *it;
                if (it != members.begin()) document_ += ",";
                document_ += valueToQuotedString(name.c_str());
                document_ += yamlCompatiblityEnabled_ ? ": " : ":";
                writeValue(value[name]);
            }
            document_ += "}";
        } break;
    }
}

StyledWriter::StyledWriter() : rightMargin_(74), indentSize_(3), addChildValues_() {}

std::string StyledWriter::write(const Value &root) {
    document_ = "";
    addChildValues_ = false;
    indentString_ = "";
    writeCommentBeforeValue(root);
    writeValue(root);
    writeCommentAfterValueOnSameLine(root);
    document_ += "\n";
    return document_;
}

void StyledWriter::writeValue(const Value &value) {
    switch (value.type()) {
        case nullValue:
            pushValue("null");
            break;
        case intValue:
            pushValue(valueToString(value.asLargestInt()));
            break;
        case uintValue:
            pushValue(valueToString(value.asLargestUInt()));
            break;
        case realValue:
            pushValue(valueToString(value.asDouble()));
            break;
        case stringValue:
            pushValue(valueToQuotedString(value.asCString()));
            break;
        case booleanValue:
            pushValue(valueToString(value.asBool()));
            break;
        case arrayValue:
            writeArrayValue(value);
            break;
        case objectValue: {
            Value::Members members(value.getMemberNames());
            if (members.empty())
                pushValue("{}");
            else {
                writeWithIndent("{");
                indent();
                Value::Members::iterator it = members.begin();
                for (;;) {
                    const std::string &name = *it;
                    const Value &childValue = value[name];
                    writeCommentBeforeValue(childValue);
                    writeWithIndent(valueToQuotedString(name.c_str()));
                    document_ += " : ";
                    writeValue(childValue);
                    if (++it == members.end()) {
                        writeCommentAfterValueOnSameLine(childValue);
                        break;
                    }
                    document_ += ",";
                    writeCommentAfterValueOnSameLine(childValue);
                }
                unindent();
                writeWithIndent("}");
            }
        } break;
    }
}

void StyledWriter::writeArrayValue(const Value &value) {
    unsigned size = value.size();
    if (size == 0)
        pushValue("[]");
    else {
        bool isArrayMultiLine = isMultineArray(value);
        if (isArrayMultiLine) {
            writeWithIndent("[");
            indent();
            bool hasChildValue = !childValues_.empty();
            unsigned index = 0;
            for (;;) {
                const Value &childValue = value[index];
                writeCommentBeforeValue(childValue);
                if (hasChildValue)
                    writeWithIndent(childValues_[index]);
                else {
                    writeIndent();
                    writeValue(childValue);
                }
                if (++index == size) {
                    writeCommentAfterValueOnSameLine(childValue);
                    break;
                }
                document_ += ",";
                writeCommentAfterValueOnSameLine(childValue);
            }
            unindent();
            writeWithIndent("]");
        } else {
            assert(childValues_.size() == size);
            document_ += "[ ";
            for (unsigned index = 0; index < size; ++index) {
                if (index > 0) document_ += ", ";
                document_ += childValues_[index];
            }
            document_ += " ]";
        }
    }
}

bool StyledWriter::isMultineArray(const Value &value) {
    int size = value.size();
    bool isMultiLine = size * 3 >= rightMargin_;
    childValues_.clear();
    for (int index = 0; index < size && !isMultiLine; ++index) {
        const Value &childValue = value[index];
        isMultiLine = isMultiLine || ((childValue.isArray() || childValue.isObject()) && childValue.size() > 0);
    }
    if (!isMultiLine) {
        childValues_.reserve(size);
        addChildValues_ = true;
        int lineLength = 4 + (size - 1) * 2;
        for (int index = 0; index < size; ++index) {
            writeValue(value[index]);
            lineLength += int(childValues_[index].length());
        }
        addChildValues_ = false;
        isMultiLine = isMultiLine || lineLength >= rightMargin_;
    }
    return isMultiLine;
}

void StyledWriter::pushValue(const std::string &value) {
    if (addChildValues_)
        childValues_.push_back(value);
    else
        document_ += value;
}

void StyledWriter::writeIndent() {
    if (!document_.empty()) {
        char last = document_[document_.length() - 1];
        if (last == ' ') return;
        if (last != '\n') document_ += '\n';
    }
    document_ += indentString_;
}

void StyledWriter::writeWithIndent(const std::string &value) {
    writeIndent();
    document_ += value;
}

void StyledWriter::indent() { indentString_ += std::string(indentSize_, ' '); }

void StyledWriter::unindent() {
    assert(int(indentString_.size()) >= indentSize_);
    indentString_.resize(indentString_.size() - indentSize_);
}

void StyledWriter::writeCommentBeforeValue(const Value &root) {
    if (!root.hasComment(commentBefore)) return;

    document_ += "\n";
    writeIndent();
    std::string normalizedComment = normalizeEOL(root.getComment(commentBefore));
    std::string::const_iterator iter = normalizedComment.begin();
    while (iter != normalizedComment.end()) {
        document_ += *iter;
        if (*iter == '\n' && *(iter + 1) == '/') writeIndent();
        ++iter;
    }

    document_ += "\n";
}

void StyledWriter::writeCommentAfterValueOnSameLine(const Value &root) {
    if (root.hasComment(commentAfterOnSameLine))
        document_ += " " + normalizeEOL(root.getComment(commentAfterOnSameLine));

    if (root.hasComment(commentAfter)) {
        document_ += "\n";
        document_ += normalizeEOL(root.getComment(commentAfter));
        document_ += "\n";
    }
}

bool StyledWriter::hasCommentForValue(const Value &value) {
    return value.hasComment(commentBefore) || value.hasComment(commentAfterOnSameLine) ||
           value.hasComment(commentAfter);
}

std::string StyledWriter::normalizeEOL(const std::string &text) {
    std::string normalized;
    normalized.reserve(text.length());
    const char *begin = text.c_str();
    const char *end = begin + text.length();
    const char *current = begin;
    while (current != end) {
        char c = *current++;
        if (c == '\r') {
            if (*current == '\n') ++current;
            normalized += '\n';
        } else
            normalized += c;
    }
    return normalized;
}

StyledStreamWriter::StyledStreamWriter(std::string indentation)
    : document_(NULL), rightMargin_(74), indentation_(indentation), addChildValues_() {}

void StyledStreamWriter::write(std::ostream &out, const Value &root) {
    document_ = &out;
    addChildValues_ = false;
    indentString_ = "";
    writeCommentBeforeValue(root);
    writeValue(root);
    writeCommentAfterValueOnSameLine(root);
    *document_ << "\n";
    document_ = NULL;
}

void StyledStreamWriter::writeValue(const Value &value) {
    switch (value.type()) {
        case nullValue:
            pushValue("null");
            break;
        case intValue:
            pushValue(valueToString(value.asLargestInt()));
            break;
        case uintValue:
            pushValue(valueToString(value.asLargestUInt()));
            break;
        case realValue:
            pushValue(valueToString(value.asDouble()));
            break;
        case stringValue:
            pushValue(valueToQuotedString(value.asCString()));
            break;
        case booleanValue:
            pushValue(valueToString(value.asBool()));
            break;
        case arrayValue:
            writeArrayValue(value);
            break;
        case objectValue: {
            Value::Members members(value.getMemberNames());
            if (members.empty())
                pushValue("{}");
            else {
                writeWithIndent("{");
                indent();
                Value::Members::iterator it = members.begin();
                for (;;) {
                    const std::string &name = *it;
                    const Value &childValue = value[name];
                    writeCommentBeforeValue(childValue);
                    writeWithIndent(valueToQuotedString(name.c_str()));
                    *document_ << " : ";
                    writeValue(childValue);
                    if (++it == members.end()) {
                        writeCommentAfterValueOnSameLine(childValue);
                        break;
                    }
                    *document_ << ",";
                    writeCommentAfterValueOnSameLine(childValue);
                }
                unindent();
                writeWithIndent("}");
            }
        } break;
    }
}

void StyledStreamWriter::writeArrayValue(const Value &value) {
    unsigned size = value.size();
    if (size == 0)
        pushValue("[]");
    else {
        bool isArrayMultiLine = isMultineArray(value);
        if (isArrayMultiLine) {
            writeWithIndent("[");
            indent();
            bool hasChildValue = !childValues_.empty();
            unsigned index = 0;
            for (;;) {
                const Value &childValue = value[index];
                writeCommentBeforeValue(childValue);
                if (hasChildValue)
                    writeWithIndent(childValues_[index]);
                else {
                    writeIndent();
                    writeValue(childValue);
                }
                if (++index == size) {
                    writeCommentAfterValueOnSameLine(childValue);
                    break;
                }
                *document_ << ",";
                writeCommentAfterValueOnSameLine(childValue);
            }
            unindent();
            writeWithIndent("]");
        } else {
            assert(childValues_.size() == size);
            *document_ << "[ ";
            for (unsigned index = 0; index < size; ++index) {
                if (index > 0) *document_ << ", ";
                *document_ << childValues_[index];
            }
            *document_ << " ]";
        }
    }
}

bool StyledStreamWriter::isMultineArray(const Value &value) {
    int size = value.size();
    bool isMultiLine = size * 3 >= rightMargin_;
    childValues_.clear();
    for (int index = 0; index < size && !isMultiLine; ++index) {
        const Value &childValue = value[index];
        isMultiLine = isMultiLine || ((childValue.isArray() || childValue.isObject()) && childValue.size() > 0);
    }
    if (!isMultiLine) {
        childValues_.reserve(size);
        addChildValues_ = true;
        int lineLength = 4 + (size - 1) * 2;
        for (int index = 0; index < size; ++index) {
            writeValue(value[index]);
            lineLength += int(childValues_[index].length());
        }
        addChildValues_ = false;
        isMultiLine = isMultiLine || lineLength >= rightMargin_;
    }
    return isMultiLine;
}

void StyledStreamWriter::pushValue(const std::string &value) {
    if (addChildValues_)
        childValues_.push_back(value);
    else
        *document_ << value;
}

void StyledStreamWriter::writeIndent() { *document_ << '\n' << indentString_; }

void StyledStreamWriter::writeWithIndent(const std::string &value) {
    writeIndent();
    *document_ << value;
}

void StyledStreamWriter::indent() { indentString_ += indentation_; }

void StyledStreamWriter::unindent() {
    assert(indentString_.size() >= indentation_.size());
    indentString_.resize(indentString_.size() - indentation_.size());
}

void StyledStreamWriter::writeCommentBeforeValue(const Value &root) {
    if (!root.hasComment(commentBefore)) return;
    *document_ << normalizeEOL(root.getComment(commentBefore));
    *document_ << "\n";
}

void StyledStreamWriter::writeCommentAfterValueOnSameLine(const Value &root) {
    if (root.hasComment(commentAfterOnSameLine))
        *document_ << " " + normalizeEOL(root.getComment(commentAfterOnSameLine));

    if (root.hasComment(commentAfter)) {
        *document_ << "\n";
        *document_ << normalizeEOL(root.getComment(commentAfter));
        *document_ << "\n";
    }
}

bool StyledStreamWriter::hasCommentForValue(const Value &value) {
    return value.hasComment(commentBefore) || value.hasComment(commentAfterOnSameLine) ||
           value.hasComment(commentAfter);
}

std::string StyledStreamWriter::normalizeEOL(const std::string &text) {
    std::string normalized;
    normalized.reserve(text.length());
    const char *begin = text.c_str();
    const char *end = begin + text.length();
    const char *current = begin;
    while (current != end) {
        char c = *current++;
        if (c == '\r') {
            if (*current == '\n') ++current;
            normalized += '\n';
        } else
            normalized += c;
    }
    return normalized;
}

std::ostream &operator<<(std::ostream &sout, const Value &root) {
    Json::StyledStreamWriter writer;
    writer.write(sout, root);
    return sout;
}

}  // namespace Json
