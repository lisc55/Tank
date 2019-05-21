#ifndef JSON_AMALGATED_H_INCLUDED
#define JSON_AMALGATED_H_INCLUDED
#define JSON_IS_AMALGAMATION
#ifndef JSON_VERSION_H_INCLUDED
#define JSON_VERSION_H_INCLUDED

#define JSONCPP_VERSION_STRING "0.7.0"
#define JSONCPP_VERSION_MAJOR 0
#define JSONCPP_VERSION_MINOR 7
#define JSONCPP_VERSION_PATCH 0
#define JSONCPP_VERSION_QUALIFIER
#define JSONCPP_VERSION_HEXA                                                   \
    ((JSONCPP_VERSION_MAJOR << 24) | (JSONCPP_VERSION_MINOR << 16) |           \
     (JSONCPP_VERSION_PATCH << 8))

#endif
#ifndef JSON_CONFIG_H_INCLUDED
#define JSON_CONFIG_H_INCLUDED
#ifndef JSON_USE_EXCEPTION
#define JSON_USE_EXCEPTION 1
#endif
#ifdef JSON_IN_CPPTL
#include <cpptl/config.h>
#ifndef JSON_USE_CPPTL
#define JSON_USE_CPPTL 1
#endif
#endif

#ifdef JSON_IN_CPPTL
#define JSON_API CPPTL_API
#elif defined(JSON_DLL_BUILD)
#if defined(_MSC_VER)
#define JSON_API __declspec(dllexport)
#define JSONCPP_DISABLE_DLL_INTERFACE_WARNING
#endif
#elif defined(JSON_DLL)
#if defined(_MSC_VER)
#define JSON_API __declspec(dllimport)
#define JSONCPP_DISABLE_DLL_INTERFACE_WARNING
#endif
#endif
#if !defined(JSON_API)
#define JSON_API
#endif
#if defined(_MSC_VER) && _MSC_VER <= 1200
#define JSON_USE_INT64_DOUBLE_CONVERSION 1
#pragma warning(disable : 4786)
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1500
#define JSONCPP_DEPRECATED(message) __declspec(deprecated(message))
#endif

#if !defined(JSONCPP_DEPRECATED)
#define JSONCPP_DEPRECATED(message)
#endif

namespace Json {
typedef int Int;
typedef unsigned int UInt;
#if defined(JSON_NO_INT64)
typedef int LargestInt;
typedef unsigned int LargestUInt;
#undef JSON_HAS_INT64
#else

#if defined(_MSC_VER)
typedef __int64 Int64;
typedef unsigned __int64 UInt64;
#else
typedef long long int Int64;
typedef unsigned long long int UInt64;
#endif
typedef Int64 LargestInt;
typedef UInt64 LargestUInt;
#define JSON_HAS_INT64
#endif
}  // namespace Json

#endif

#ifndef JSON_FORWARDS_H_INCLUDED
#define JSON_FORWARDS_H_INCLUDED

#if !defined(JSON_IS_AMALGAMATION)
#endif

namespace Json {

class FastWriter;

class StyledWriter;

class Reader;

class Features;

typedef unsigned int ArrayIndex;

class StaticString;

class Path;

class PathArgument;

class Value;

class ValueIteratorBase;

class ValueIterator;

class ValueConstIterator;

#ifdef JSON_VALUE_USE_INTERNAL_MAP
class ValueMapAllocator;
class ValueInternalLink;
class ValueInternalArray;
class ValueInternalMap;
#endif

}  // namespace Json

#endif

#ifndef CPPTL_JSON_FEATURES_H_INCLUDED
#define CPPTL_JSON_FEATURES_H_INCLUDED

#if !defined(JSON_IS_AMALGAMATION)
#endif

namespace Json {

class JSON_API Features {
   public:
    static Features all();

    static Features strictMode();

    Features();

    bool allowComments_;

    bool strictRoot_;

    bool allowDroppedNullPlaceholders_;

    bool allowNumericKeys_;
};

}  // namespace Json

#endif

#ifndef CPPTL_JSON_H_INCLUDED
#define CPPTL_JSON_H_INCLUDED

#if !defined(JSON_IS_AMALGAMATION)
#endif

#include <string>
#include <vector>

#ifndef JSON_USE_CPPTL_SMALLMAP

#include <map>

#else
#include <cpptl/smallmap.h>
#endif
#ifdef JSON_USE_CPPTL
#include <cpptl/forwards.h>
#endif

#if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

namespace Json {

enum ValueType {
    nullValue = 0,
    intValue,
    uintValue,
    realValue,
    stringValue,
    booleanValue,
    arrayValue,
    objectValue
};

enum CommentPlacement {
    commentBefore = 0,
    commentAfterOnSameLine,
    commentAfter,

    numberOfCommentPlacement
};

class JSON_API StaticString {
   public:
    explicit StaticString(const char *czstring) : str_(czstring) {}

    operator const char *() const { return str_; }

    const char *c_str() const { return str_; }

   private:
    const char *str_;
};

class JSON_API Value {
    friend class ValueIteratorBase;

#ifdef JSON_VALUE_USE_INTERNAL_MAP
    friend class ValueInternalLink;
    friend class ValueInternalMap;
#endif
   public:
    typedef std::vector<std::string> Members;
    typedef ValueIterator iterator;
    typedef ValueConstIterator const_iterator;
    typedef Json::UInt UInt;
    typedef Json::Int Int;
#if defined(JSON_HAS_INT64)
    typedef Json::UInt64 UInt64;
    typedef Json::Int64 Int64;
#endif
    typedef Json::LargestInt LargestInt;
    typedef Json::LargestUInt LargestUInt;
    typedef Json::ArrayIndex ArrayIndex;

    static const Value &null;

    static const LargestInt minLargestInt;

    static const LargestInt maxLargestInt;

    static const LargestUInt maxLargestUInt;

    static const Int minInt;

    static const Int maxInt;

    static const UInt maxUInt;

#if defined(JSON_HAS_INT64)

    static const Int64 minInt64;

    static const Int64 maxInt64;

    static const UInt64 maxUInt64;
#endif

   private:
#ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION
#ifndef JSON_VALUE_USE_INTERNAL_MAP

    class CZString {
       public:
        enum DuplicationPolicy {
            noDuplication = 0,
            duplicate,
            duplicateOnCopy
        };

        CZString(ArrayIndex index);

        CZString(const char *cstr, DuplicationPolicy allocate);

        CZString(const CZString &other);

        ~CZString();

        CZString &operator=(CZString other);

        bool operator<(const CZString &other) const;

        bool operator==(const CZString &other) const;

        ArrayIndex index() const;

        const char *c_str() const;

        bool isStaticString() const;

       private:
        void swap(CZString &other);

        const char *cstr_;
        ArrayIndex index_;
    };

   public:
#ifndef JSON_USE_CPPTL_SMALLMAP
    typedef std::map<CZString, Value> ObjectValues;
#else
    typedef CppTL::SmallMap<CZString, Value> ObjectValues;
#endif
#endif
#endif

   public:
    Value(ValueType type = nullValue);

    Value(Int value);

    Value(UInt value);

#if defined(JSON_HAS_INT64)

    Value(Int64 value);

    Value(UInt64 value);

#endif

    Value(double value);

    Value(const char *value);

    Value(const char *beginValue, const char *endValue);

    Value(const StaticString &value);

    Value(const std::string &value);

#ifdef JSON_USE_CPPTL
    Value(const CppTL::ConstString &value);
#endif

    Value(bool value);

    Value(const Value &other);

    ~Value();

    Value &operator=(Value other);

    void swap(Value &other);

    ValueType type() const;

    bool operator<(const Value &other) const;

    bool operator<=(const Value &other) const;

    bool operator>=(const Value &other) const;

    bool operator>(const Value &other) const;

    bool operator==(const Value &other) const;

    bool operator!=(const Value &other) const;

    int compare(const Value &other) const;

    const char *asCString() const;

    std::string asString() const;

#ifdef JSON_USE_CPPTL
    CppTL::ConstString asConstString() const;
#endif

    Int asInt() const;

    UInt asUInt() const;

#if defined(JSON_HAS_INT64)

    Int64 asInt64() const;

    UInt64 asUInt64() const;

#endif

    LargestInt asLargestInt() const;

    LargestUInt asLargestUInt() const;

    float asFloat() const;

    double asDouble() const;

    bool asBool() const;

    bool isNull() const;

    bool isBool() const;

    bool isInt() const;

    bool isInt64() const;

    bool isUInt() const;

    bool isUInt64() const;

    bool isIntegral() const;

    bool isDouble() const;

    bool isNumeric() const;

    bool isString() const;

    bool isArray() const;

    bool isObject() const;

    bool isConvertibleTo(ValueType other) const;

    ArrayIndex size() const;

    bool empty() const;

    bool operator!() const;

    void clear();

    void resize(ArrayIndex size);

    Value &operator[](ArrayIndex index);

    Value &operator[](int index);

    const Value &operator[](ArrayIndex index) const;

    const Value &operator[](int index) const;

    Value get(ArrayIndex index, const Value &defaultValue) const;

    bool isValidIndex(ArrayIndex index) const;

    Value &append(const Value &value);

    Value &operator[](const char *key);

    const Value &operator[](const char *key) const;

    Value &operator[](const std::string &key);

    const Value &operator[](const std::string &key) const;

    Value &operator[](const StaticString &key);

#ifdef JSON_USE_CPPTL

    Value &operator[](const CppTL::ConstString &key);

    const Value &operator[](const CppTL::ConstString &key) const;
#endif

    Value get(const char *key, const Value &defaultValue) const;

    Value get(const std::string &key, const Value &defaultValue) const;

#ifdef JSON_USE_CPPTL

    Value get(const CppTL::ConstString &key, const Value &defaultValue) const;
#endif

    Value removeMember(const char *key);

    Value removeMember(const std::string &key);

    bool isMember(const char *key) const;

    bool isMember(const std::string &key) const;

#ifdef JSON_USE_CPPTL

    bool isMember(const CppTL::ConstString &key) const;
#endif

    Members getMemberNames() const;

    void setComment(const char *comment, CommentPlacement placement);

    void setComment(const std::string &comment, CommentPlacement placement);

    bool hasComment(CommentPlacement placement) const;

    std::string getComment(CommentPlacement placement) const;

    std::string toStyledString() const;

    const_iterator begin() const;

    const_iterator end() const;

    iterator begin();

    iterator end();

    void setOffsetStart(size_t start);

    void setOffsetLimit(size_t limit);

    size_t getOffsetStart() const;

    size_t getOffsetLimit() const;

   private:
    Value &resolveReference(const char *key, bool isStatic);

#ifdef JSON_VALUE_USE_INTERNAL_MAP
    inline bool isItemAvailable() const { return itemIsUsed_ == 0; }

    inline void setItemUsed(bool isUsed = true) {
        itemIsUsed_ = isUsed ? 1 : 0;
    }

    inline bool isMemberNameStatic() const { return memberNameIsStatic_ == 0; }

    inline void setMemberNameIsStatic(bool isStatic) {
        memberNameIsStatic_ = isStatic ? 1 : 0;
    }
#endif

   private:
    struct CommentInfo {
        CommentInfo();

        ~CommentInfo();

        void setComment(const char *text);

        char *comment_;
    };

    union ValueHolder {
        LargestInt int_;
        LargestUInt uint_;
        double real_;
        bool bool_;
        char *string_;
#ifdef JSON_VALUE_USE_INTERNAL_MAP
        ValueInternalArray *array_;
        ValueInternalMap *map_;
#else
        ObjectValues *map_;
#endif
    } value_;
    ValueType type_ : 8;
    int allocated_ : 1;
#ifdef JSON_VALUE_USE_INTERNAL_MAP
    unsigned int itemIsUsed_ : 1;
    int memberNameIsStatic_ : 1;
#endif
    CommentInfo *comments_;

    size_t start_;
    size_t limit_;
};

class JSON_API PathArgument {
   public:
    friend class Path;

    PathArgument();

    PathArgument(ArrayIndex index);

    PathArgument(const char *key);

    PathArgument(const std::string &key);

   private:
    enum Kind { kindNone = 0, kindIndex, kindKey };
    std::string key_;
    ArrayIndex index_;
    Kind kind_;
};

class JSON_API Path {
   public:
    Path(const std::string &path,
         const PathArgument &a1 = PathArgument(),
         const PathArgument &a2 = PathArgument(),
         const PathArgument &a3 = PathArgument(),
         const PathArgument &a4 = PathArgument(),
         const PathArgument &a5 = PathArgument());

    const Value &resolve(const Value &root) const;

    Value resolve(const Value &root, const Value &defaultValue) const;

    Value &make(Value &root) const;

   private:
    typedef std::vector<const PathArgument *> InArgs;
    typedef std::vector<PathArgument> Args;

    void makePath(const std::string &path, const InArgs &in);

    void addPathInArg(const std::string &path,
                      const InArgs &in,
                      InArgs::const_iterator &itInArg,
                      PathArgument::Kind kind);

    void invalidPath(const std::string &path, int location);

    Args args_;
};

#ifdef JSON_VALUE_USE_INTERNAL_MAP

class JSON_API ValueMapAllocator {
   public:
    virtual ~ValueMapAllocator();
    virtual ValueInternalMap *newMap() = 0;
    virtual ValueInternalMap *newMapCopy(const ValueInternalMap &other) = 0;
    virtual void destructMap(ValueInternalMap *map) = 0;
    virtual ValueInternalLink *allocateMapBuckets(unsigned int size) = 0;
    virtual void releaseMapBuckets(ValueInternalLink *links) = 0;
    virtual ValueInternalLink *allocateMapLink() = 0;
    virtual void releaseMapLink(ValueInternalLink *link) = 0;
};

class JSON_API ValueInternalLink {
   public:
    enum { itemPerLink = 6 };
    enum InternalFlags { flagAvailable = 0, flagUsed = 1 };

    ValueInternalLink();

    ~ValueInternalLink();

    Value items_[itemPerLink];
    char *keys_[itemPerLink];
    ValueInternalLink *previous_;
    ValueInternalLink *next_;
};

class JSON_API ValueInternalMap {
    friend class ValueIteratorBase;
    friend class Value;

   public:
    typedef unsigned int HashKey;
    typedef unsigned int BucketIndex;

#ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION
    struct IteratorState {
        IteratorState() : map_(0), link_(0), itemIndex_(0), bucketIndex_(0) {}
        ValueInternalMap *map_;
        ValueInternalLink *link_;
        BucketIndex itemIndex_;
        BucketIndex bucketIndex_;
    };
#endif

    ValueInternalMap();
    ValueInternalMap(const ValueInternalMap &other);
    ValueInternalMap &operator=(ValueInternalMap other);
    ~ValueInternalMap();

    void swap(ValueInternalMap &other);

    BucketIndex size() const;

    void clear();

    bool reserveDelta(BucketIndex growth);

    bool reserve(BucketIndex newItemCount);

    const Value *find(const char *key) const;

    Value *find(const char *key);

    Value &resolveReference(const char *key, bool isStatic);

    void remove(const char *key);

    void doActualRemove(ValueInternalLink *link,
                        BucketIndex index,
                        BucketIndex bucketIndex);

    ValueInternalLink *&getLastLinkInBucket(BucketIndex bucketIndex);

    Value &setNewItem(const char *key,
                      bool isStatic,
                      ValueInternalLink *link,
                      BucketIndex index);

    Value &unsafeAdd(const char *key, bool isStatic, HashKey hashedKey);

    HashKey hash(const char *key) const;

    int compare(const ValueInternalMap &other) const;

   private:
    void makeBeginIterator(IteratorState &it) const;
    void makeEndIterator(IteratorState &it) const;
    static bool equals(const IteratorState &x, const IteratorState &other);
    static void increment(IteratorState &iterator);
    static void incrementBucket(IteratorState &iterator);
    static void decrement(IteratorState &iterator);
    static const char *key(const IteratorState &iterator);
    static const char *key(const IteratorState &iterator, bool &isStatic);
    static Value &value(const IteratorState &iterator);
    static int distance(const IteratorState &x, const IteratorState &y);

   private:
    ValueInternalLink *buckets_;
    ValueInternalLink *tailLink_;
    BucketIndex bucketsSize_;
    BucketIndex itemCount_;
};

class JSON_API ValueInternalArray {
    friend class Value;
    friend class ValueIteratorBase;

   public:
    enum { itemsPerPage = 8 };
    typedef Value::ArrayIndex ArrayIndex;
    typedef unsigned int PageIndex;

#ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION
    struct IteratorState {
        IteratorState()
            : array_(0), currentPageIndex_(0), currentItemIndex_(0) {}
        ValueInternalArray *array_;
        Value **currentPageIndex_;
        unsigned int currentItemIndex_;
    };
#endif

    ValueInternalArray();
    ValueInternalArray(const ValueInternalArray &other);
    ValueInternalArray &operator=(ValueInternalArray other);
    ~ValueInternalArray();
    void swap(ValueInternalArray &other);

    void clear();
    void resize(ArrayIndex newSize);

    Value &resolveReference(ArrayIndex index);

    Value *find(ArrayIndex index) const;

    ArrayIndex size() const;

    int compare(const ValueInternalArray &other) const;

   private:
    static bool equals(const IteratorState &x, const IteratorState &other);
    static void increment(IteratorState &iterator);
    static void decrement(IteratorState &iterator);
    static Value &dereference(const IteratorState &iterator);
    static Value &unsafeDereference(const IteratorState &iterator);
    static int distance(const IteratorState &x, const IteratorState &y);
    static ArrayIndex indexOf(const IteratorState &iterator);
    void makeBeginIterator(IteratorState &it) const;
    void makeEndIterator(IteratorState &it) const;
    void makeIterator(IteratorState &it, ArrayIndex index) const;

    void makeIndexValid(ArrayIndex index);

    Value **pages_;
    ArrayIndex size_;
    PageIndex pageCount_;
};

class JSON_API ValueArrayAllocator {
   public:
    virtual ~ValueArrayAllocator();
    virtual ValueInternalArray *newArray() = 0;
    virtual ValueInternalArray *newArrayCopy(
        const ValueInternalArray &other) = 0;
    virtual void destructArray(ValueInternalArray *array) = 0;

    virtual void reallocateArrayPageIndex(
        Value **&indexes,
        ValueInternalArray::PageIndex &indexCount,
        ValueInternalArray::PageIndex minNewIndexCount) = 0;
    virtual void releaseArrayPageIndex(
        Value **indexes,
        ValueInternalArray::PageIndex indexCount) = 0;
    virtual Value *allocateArrayPage() = 0;
    virtual void releaseArrayPage(Value *value) = 0;
};
#endif

class JSON_API ValueIteratorBase {
   public:
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef unsigned int size_t;
    typedef int difference_type;
    typedef ValueIteratorBase SelfType;

    ValueIteratorBase();

#ifndef JSON_VALUE_USE_INTERNAL_MAP

    explicit ValueIteratorBase(const Value::ObjectValues::iterator &current);

#else
    ValueIteratorBase(const ValueInternalArray::IteratorState &state);
    ValueIteratorBase(const ValueInternalMap::IteratorState &state);
#endif

    bool operator==(const SelfType &other) const { return isEqual(other); }

    bool operator!=(const SelfType &other) const { return !isEqual(other); }

    difference_type operator-(const SelfType &other) const {
        return computeDistance(other);
    }

    Value key() const;

    UInt index() const;

    const char *memberName() const;

   protected:
    Value &deref() const;

    void increment();

    void decrement();

    difference_type computeDistance(const SelfType &other) const;

    bool isEqual(const SelfType &other) const;

    void copy(const SelfType &other);

   private:
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    Value::ObjectValues::iterator current_;

    bool isNull_;
#else
    union {
        ValueInternalArray::IteratorState array_;
        ValueInternalMap::IteratorState map_;
    } iterator_;
    bool isArray_;
#endif
};

class JSON_API ValueConstIterator : public ValueIteratorBase {
    friend class Value;

   public:
    typedef const Value value_type;
    typedef unsigned int size_t;
    typedef int difference_type;
    typedef const Value &reference;
    typedef const Value *pointer;
    typedef ValueConstIterator SelfType;

    ValueConstIterator();

   private:
#ifndef JSON_VALUE_USE_INTERNAL_MAP

    explicit ValueConstIterator(const Value::ObjectValues::iterator &current);

#else
    ValueConstIterator(const ValueInternalArray::IteratorState &state);
    ValueConstIterator(const ValueInternalMap::IteratorState &state);
#endif
   public:
    SelfType &operator=(const ValueIteratorBase &other);

    SelfType operator++(int) {
        SelfType temp(*this);
        ++*this;
        return temp;
    }

    SelfType operator--(int) {
        SelfType temp(*this);
        --*this;
        return temp;
    }

    SelfType &operator--() {
        decrement();
        return *this;
    }

    SelfType &operator++() {
        increment();
        return *this;
    }

    reference operator*() const { return deref(); }

    pointer operator->() const { return &deref(); }
};

class JSON_API ValueIterator : public ValueIteratorBase {
    friend class Value;

   public:
    typedef Value value_type;
    typedef unsigned int size_t;
    typedef int difference_type;
    typedef Value &reference;
    typedef Value *pointer;
    typedef ValueIterator SelfType;

    ValueIterator();

    ValueIterator(const ValueConstIterator &other);

    ValueIterator(const ValueIterator &other);

   private:
#ifndef JSON_VALUE_USE_INTERNAL_MAP

    explicit ValueIterator(const Value::ObjectValues::iterator &current);

#else
    ValueIterator(const ValueInternalArray::IteratorState &state);
    ValueIterator(const ValueInternalMap::IteratorState &state);
#endif
   public:
    SelfType &operator=(const SelfType &other);

    SelfType operator++(int) {
        SelfType temp(*this);
        ++*this;
        return temp;
    }

    SelfType operator--(int) {
        SelfType temp(*this);
        --*this;
        return temp;
    }

    SelfType &operator--() {
        decrement();
        return *this;
    }

    SelfType &operator++() {
        increment();
        return *this;
    }

    reference operator*() const { return deref(); }

    pointer operator->() const { return &deref(); }
};

}  // namespace Json

#if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)
#pragma warning(pop)
#endif

#endif

#ifndef CPPTL_JSON_READER_H_INCLUDED
#define CPPTL_JSON_READER_H_INCLUDED

#if !defined(JSON_IS_AMALGAMATION)

#endif

#include <deque>
#include <iosfwd>
#include <stack>
#include <string>

#if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

namespace Json {

class JSON_API Reader {
   public:
    typedef char Char;
    typedef const Char *Location;

    struct StructuredError {
        size_t offset_start;
        size_t offset_limit;
        std::string message;
    };

    Reader();

    Reader(const Features &features);

    bool parse(const std::string &document,
               Value &root,
               bool collectComments = true);

    bool parse(const char *beginDoc,
               const char *endDoc,
               Value &root,
               bool collectComments = true);

    bool parse(std::istream &is, Value &root, bool collectComments = true);

    JSONCPP_DEPRECATED("Use getFormattedErrorMessages instead")

    std::string getFormatedErrorMessages() const;

    std::string getFormattedErrorMessages() const;

    std::vector<StructuredError> getStructuredErrors() const;

   private:
    enum TokenType {
        tokenEndOfStream = 0,
        tokenObjectBegin,
        tokenObjectEnd,
        tokenArrayBegin,
        tokenArrayEnd,
        tokenString,
        tokenNumber,
        tokenTrue,
        tokenFalse,
        tokenNull,
        tokenArraySeparator,
        tokenMemberSeparator,
        tokenComment,
        tokenError
    };

    class Token {
       public:
        TokenType type_;
        Location start_;
        Location end_;
    };

    class ErrorInfo {
       public:
        Token token_;
        std::string message_;
        Location extra_;
    };

    typedef std::deque<ErrorInfo> Errors;

    bool expectToken(TokenType type, Token &token, const char *message);

    bool readToken(Token &token);

    void skipSpaces();

    bool match(Location pattern, int patternLength);

    bool readComment();

    bool readCStyleComment();

    bool readCppStyleComment();

    bool readString();

    void readNumber();

    bool readValue();

    bool readObject(Token &token);

    bool readArray(Token &token);

    bool decodeNumber(Token &token);

    bool decodeNumber(Token &token, Value &decoded);

    bool decodeString(Token &token);

    bool decodeString(Token &token, std::string &decoded);

    bool decodeDouble(Token &token);

    bool decodeDouble(Token &token, Value &decoded);

    bool decodeUnicodeCodePoint(Token &token,
                                Location &current,
                                Location end,
                                unsigned int &unicode);

    bool decodeUnicodeEscapeSequence(Token &token,
                                     Location &current,
                                     Location end,
                                     unsigned int &unicode);

    bool addError(const std::string &message, Token &token, Location extra = 0);

    bool recoverFromError(TokenType skipUntilToken);

    bool addErrorAndRecover(const std::string &message,
                            Token &token,
                            TokenType skipUntilToken);

    void skipUntilSpace();

    Value &currentValue();

    Char getNextChar();

    void getLocationLineAndColumn(Location location,
                                  int &line,
                                  int &column) const;

    std::string getLocationLineAndColumn(Location location) const;

    void addComment(Location begin, Location end, CommentPlacement placement);

    void skipCommentTokens(Token &token);

    typedef std::stack<Value *> Nodes;
    Nodes nodes_;
    Errors errors_;
    std::string document_;
    Location begin_;
    Location end_;
    Location current_;
    Location lastValueEnd_;
    Value *lastValue_;
    std::string commentsBefore_;
    Features features_;
    bool collectComments_;
};

JSON_API std::istream &operator>>(std::istream &, Value &);

}  // namespace Json

#if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)
#pragma warning(pop)
#endif

#endif

#ifndef JSON_WRITER_H_INCLUDED
#define JSON_WRITER_H_INCLUDED

#if !defined(JSON_IS_AMALGAMATION)
#endif

#include <string>
#include <vector>

#if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

namespace Json {

class Value;

class JSON_API Writer {
   public:
    virtual ~Writer();

    virtual std::string write(const Value &root) = 0;
};

class JSON_API FastWriter : public Writer {
   public:
    FastWriter();

    virtual ~FastWriter() {}

    void enableYAMLCompatibility();

    void dropNullPlaceholders();

    void omitEndingLineFeed();

   public:
    virtual std::string write(const Value &root);

   private:
    void writeValue(const Value &value);

    std::string document_;
    bool yamlCompatiblityEnabled_;
    bool dropNullPlaceholders_;
    bool omitEndingLineFeed_;
};

class JSON_API StyledWriter : public Writer {
   public:
    StyledWriter();

    virtual ~StyledWriter() {}

   public:
    virtual std::string write(const Value &root);

   private:
    void writeValue(const Value &value);

    void writeArrayValue(const Value &value);

    bool isMultineArray(const Value &value);

    void pushValue(const std::string &value);

    void writeIndent();

    void writeWithIndent(const std::string &value);

    void indent();

    void unindent();

    void writeCommentBeforeValue(const Value &root);

    void writeCommentAfterValueOnSameLine(const Value &root);

    bool hasCommentForValue(const Value &value);

    static std::string normalizeEOL(const std::string &text);

    typedef std::vector<std::string> ChildValues;

    ChildValues childValues_;
    std::string document_;
    std::string indentString_;
    int rightMargin_;
    int indentSize_;
    bool addChildValues_;
};

class JSON_API StyledStreamWriter {
   public:
    StyledStreamWriter(std::string indentation = "\t");

    ~StyledStreamWriter() {}

   public:
    void write(std::ostream &out, const Value &root);

   private:
    void writeValue(const Value &value);

    void writeArrayValue(const Value &value);

    bool isMultineArray(const Value &value);

    void pushValue(const std::string &value);

    void writeIndent();

    void writeWithIndent(const std::string &value);

    void indent();

    void unindent();

    void writeCommentBeforeValue(const Value &root);

    void writeCommentAfterValueOnSameLine(const Value &root);

    bool hasCommentForValue(const Value &value);

    static std::string normalizeEOL(const std::string &text);

    typedef std::vector<std::string> ChildValues;

    ChildValues childValues_;
    std::ostream *document_;
    std::string indentString_;
    int rightMargin_;
    std::string indentation_;
    bool addChildValues_;
};

#if defined(JSON_HAS_INT64)

std::string JSON_API valueToString(Int value);

std::string JSON_API valueToString(UInt value);

#endif

std::string JSON_API valueToString(LargestInt value);

std::string JSON_API valueToString(LargestUInt value);

std::string JSON_API valueToString(double value);

std::string JSON_API valueToString(bool value);

std::string JSON_API valueToQuotedString(const char *value);

JSON_API std::ostream &operator<<(std::ostream &, const Value &root);

}  // namespace Json

#if defined(JSONCPP_DISABLE_DLL_INTERFACE_WARNING)
#pragma warning(pop)
#endif

#endif

#ifndef CPPTL_JSON_ASSERTIONS_H_INCLUDED
#define CPPTL_JSON_ASSERTIONS_H_INCLUDED

#include <stdlib.h>

#if !defined(JSON_IS_AMALGAMATION)
#endif

#if JSON_USE_EXCEPTION

#include <stdexcept>

#define JSON_ASSERT(condition) assert(condition);
#define JSON_FAIL_MESSAGE(message) throw std::runtime_error(message);
#else
#define JSON_ASSERT(condition) assert(condition);

#define JSON_FAIL_MESSAGE(message)                                             \
    {                                                                          \
        assert(false && message);                                              \
        strcpy(reinterpret_cast<char *>(666), message);                        \
        exit(123);                                                             \
    }

#endif

#define JSON_ASSERT_MESSAGE(condition, message)                                \
    if (!(condition)) { JSON_FAIL_MESSAGE(message) }

#endif

#endif

#ifndef JSON_FORWARD_AMALGATED_H_INCLUDED
#define JSON_FORWARD_AMALGATED_H_INCLUDED

#define JSON_IS_AMALGAMATION

#ifndef JSON_CONFIG_H_INCLUDED
#define JSON_CONFIG_H_INCLUDED

#ifndef JSON_USE_EXCEPTION
#define JSON_USE_EXCEPTION 1
#endif

#ifdef JSON_IN_CPPTL
#include <cpptl/config.h>
#ifndef JSON_USE_CPPTL
#define JSON_USE_CPPTL 1
#endif
#endif

#ifdef JSON_IN_CPPTL
#define JSON_API CPPTL_API
#elif defined(JSON_DLL_BUILD)
#if defined(_MSC_VER)
#define JSON_API __declspec(dllexport)
#define JSONCPP_DISABLE_DLL_INTERFACE_WARNING
#endif
#elif defined(JSON_DLL)
#if defined(_MSC_VER)
#define JSON_API __declspec(dllimport)
#define JSONCPP_DISABLE_DLL_INTERFACE_WARNING
#endif
#endif
#if !defined(JSON_API)
#define JSON_API
#endif

#if defined(_MSC_VER) && _MSC_VER <= 1200

#define JSON_USE_INT64_DOUBLE_CONVERSION 1

#pragma warning(disable : 4786)
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1500

#define JSONCPP_DEPRECATED(message) __declspec(deprecated(message))
#endif

#if !defined(JSONCPP_DEPRECATED)
#define JSONCPP_DEPRECATED(message)
#endif

namespace Json {
typedef int Int;
typedef unsigned int UInt;
#if defined(JSON_NO_INT64)
typedef int LargestInt;
typedef unsigned int LargestUInt;
#undef JSON_HAS_INT64
#else

#if defined(_MSC_VER)
typedef __int64 Int64;
typedef unsigned __int64 UInt64;
#else
typedef long long int Int64;
typedef unsigned long long int UInt64;
#endif
typedef Int64 LargestInt;
typedef UInt64 LargestUInt;
#define JSON_HAS_INT64
#endif
}  // namespace Json

#endif

#ifndef JSON_FORWARDS_H_INCLUDED
#define JSON_FORWARDS_H_INCLUDED

#if !defined(JSON_IS_AMALGAMATION)
#endif

namespace Json {

class FastWriter;
class StyledWriter;

class Reader;

class Features;

typedef unsigned int ArrayIndex;
class StaticString;
class Path;
class PathArgument;
class Value;
class ValueIteratorBase;
class ValueIterator;
class ValueConstIterator;
#ifdef JSON_VALUE_USE_INTERNAL_MAP
class ValueMapAllocator;
class ValueInternalLink;
class ValueInternalArray;
class ValueInternalMap;
#endif

}  // namespace Json

#endif

#endif

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
    : allowComments_(true),
      strictRoot_(false),
      allowDroppedNullPlaceholders_(false),
      allowNumericKeys_(false) {}

Features Features::all() { return Features(); }

Features Features::strictMode() {
    Features features;
    features.allowComments_ = false;
    features.strictRoot_ = true;
    features.allowDroppedNullPlaceholders_ = false;
    features.allowNumericKeys_ = false;
    return features;
}

static inline bool in(Reader::Char c,
                      Reader::Char c1,
                      Reader::Char c2,
                      Reader::Char c3,
                      Reader::Char c4) {
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

bool Reader::parse(const std::string &document,
                   Value &root,
                   bool collectComments) {
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

bool Reader::parse(const char *beginDoc,
                   const char *endDoc,
                   Value &root,
                   bool collectComments) {
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
    if (collectComments_ && !commentsBefore_.empty())
        root.setComment(commentsBefore_, commentAfter);
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
            return addError("Syntax error: value, object or array expected.",
                            token);
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
            if (c != '*' || !containsNewLine(commentBegin, current_))
                placement = commentAfterOnSameLine;
        }

        addComment(commentBegin, current_, placement);
    }
    return true;
}

void Reader::addComment(Location begin,
                        Location end,
                        CommentPlacement placement) {
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
        if (!(*current_ >= '0' && *current_ <= '9') &&
            !in(*current_, '.', 'e', 'E', '+', '-'))
            break;
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
        while (tokenName.type_ == tokenComment && initialTokenOk)
            initialTokenOk = readToken(tokenName);
        if (!initialTokenOk) break;
        if (tokenName.type_ == tokenObjectEnd && name.empty()) return true;
        name = "";
        if (tokenName.type_ == tokenString) {
            if (!decodeString(tokenName, name))
                return recoverFromError(tokenObjectEnd);
        } else if (tokenName.type_ == tokenNumber &&
                   features_.allowNumericKeys_) {
            Value numberName;
            if (!decodeNumber(tokenName, numberName))
                return recoverFromError(tokenObjectEnd);
            name = numberName.asString();
        } else {
            break;
        }

        Token colon;
        if (!readToken(colon) || colon.type_ != tokenMemberSeparator) {
            return addErrorAndRecover("Missing ':' after object member name",
                                      colon, tokenObjectEnd);
        }
        Value &value = currentValue()[name];
        nodes_.push(&value);
        bool ok = readValue();
        nodes_.pop();
        if (!ok) return recoverFromError(tokenObjectEnd);

        Token comma;
        if (!readToken(comma) || (comma.type_ != tokenObjectEnd &&
                                  comma.type_ != tokenArraySeparator &&
                                  comma.type_ != tokenComment)) {
            return addErrorAndRecover(
                "Missing ',' or '}' in object declaration", comma,
                tokenObjectEnd);
        }
        bool finalizeTokenOk = true;
        while (comma.type_ == tokenComment && finalizeTokenOk)
            finalizeTokenOk = readToken(comma);
        if (comma.type_ == tokenObjectEnd) return true;
    }
    return addErrorAndRecover("Missing '}' or object member name", tokenName,
                              tokenObjectEnd);
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
        bool badTokenType = (token.type_ != tokenArraySeparator &&
                             token.type_ != tokenArrayEnd);
        if (!ok || badTokenType) {
            return addErrorAndRecover("Missing ',' or ']' in array declaration",
                                      token, tokenArrayEnd);
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
        isDouble = isDouble || in(*inspect, '.', 'e', 'E', '+') ||
                   (*inspect == '-' && inspect != token.start_);
    }
    if (isDouble) return decodeDouble(token, decoded);

    Location current = token.start_;
    bool isNegative = *current == '-';
    if (isNegative) ++current;
    Value::LargestUInt maxIntegerValue =
        isNegative ? Value::LargestUInt(-Value::minLargestInt) :
                     Value::maxLargestUInt;
    Value::LargestUInt threshold = maxIntegerValue / 10;
    Value::LargestUInt value = 0;
    while (current < token.end_) {
        Char c = *current++;
        if (c < '0' || c > '9')
            return addError("'" + std::string(token.start_, token.end_) +
                                "' is not a number.",
                            token);
        Value::UInt digit(c - '0');
        if (value >= threshold) {
            if (value > threshold || current != token.end_ ||
                digit > maxIntegerValue % 10) {
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

    if (count != 1)
        return addError(
            "'" + std::string(token.start_, token.end_) + "' is not a number.",
            token);
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
            if (current == end)
                return addError("Empty escape sequence in string", token,
                                current);
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
                    if (!decodeUnicodeCodePoint(token, current, end, unicode))
                        return false;
                    decoded += codePointToUTF8(unicode);
                } break;
                default:
                    return addError("Bad escape sequence in string", token,
                                    current);
            }
        } else {
            decoded += c;
        }
    }
    return true;
}

bool Reader::decodeUnicodeCodePoint(Token &token,
                                    Location &current,
                                    Location end,
                                    unsigned int &unicode) {
    if (!decodeUnicodeEscapeSequence(token, current, end, unicode))
        return false;
    if (unicode >= 0xD800 && unicode <= 0xDBFF) {
        if (end - current < 6)
            return addError(
                "additional six characters expected to parse unicode surrogate "
                "pair.",
                token, current);
        unsigned int surrogatePair;
        if (*(current++) == '\\' && *(current++) == 'u') {
            if (decodeUnicodeEscapeSequence(token, current, end,
                                            surrogatePair)) {
                unicode = 0x10000 + ((unicode & 0x3FF) << 10) +
                          (surrogatePair & 0x3FF);
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

bool Reader::decodeUnicodeEscapeSequence(Token &token,
                                         Location &current,
                                         Location end,
                                         unsigned int &unicode) {
    if (end - current < 4)
        return addError(
            "Bad unicode escape sequence in string: four digits expected.",
            token, current);
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

bool Reader::addError(const std::string &message,
                      Token &token,
                      Location extra) {
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
        if (skip.type_ == skipUntilToken || skip.type_ == tokenEndOfStream)
            break;
    }
    errors_.resize(errorCount);
    return false;
}

bool Reader::addErrorAndRecover(const std::string &message,
                                Token &token,
                                TokenType skipUntilToken) {
    addError(message, token);
    return recoverFromError(skipUntilToken);
}

Value &Reader::currentValue() { return *(nodes_.top()); }

Reader::Char Reader::getNextChar() {
    if (current_ == end_) return 0;
    return *current_++;
}

void Reader::getLocationLineAndColumn(Location location,
                                      int &line,
                                      int &column) const {
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

std::string Reader::getFormatedErrorMessages() const {
    return getFormattedErrorMessages();
}

std::string Reader::getFormattedErrorMessages() const {
    std::string formattedMessage;
    for (Errors::const_iterator itError = errors_.begin();
         itError != errors_.end(); ++itError) {
        const ErrorInfo &error = *itError;
        formattedMessage +=
            "* " + getLocationLineAndColumn(error.token_.start_) + "\n";
        formattedMessage += "  " + error.message_ + "\n";
        if (error.extra_)
            formattedMessage += "See " +
                                getLocationLineAndColumn(error.extra_) +
                                " for detail.\n";
    }
    return formattedMessage;
}

std::vector<Reader::StructuredError> Reader::getStructuredErrors() const {
    std::vector<Reader::StructuredError> allErrors;
    for (Errors::const_iterator itError = errors_.begin();
         itError != errors_.end(); ++itError) {
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
        fprintf(stderr, "Error from reader: %s",
                reader.getFormattedErrorMessages().c_str());

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
    BatchAllocator(unsigned int objectsPerPage = 255)
        : freeHead_(0), objectsPerPage_(objectsPerPage) {
        assert(sizeof(AllocatedType) * objectPerAllocation >=
               sizeof(AllocatedType *));

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
            while (currentBatch_ && currentBatch_->used_ == currentBatch_->end_)
                currentBatch_ = currentBatch_->next_;

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
        const unsigned int mallocSize =
            sizeof(BatchInfo) - sizeof(AllocatedType) * objectPerAllocation +
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

ValueIteratorBase::ValueIteratorBase(
    const Value::ObjectValues::iterator &current)
    : current_(current), isNull_(false) {}

#else
ValueIteratorBase::ValueIteratorBase(
    const ValueInternalArray::IteratorState &state)
    : isArray_(true) {
    iterator_.array_ = state;
}

ValueIteratorBase::ValueIteratorBase(
    const ValueInternalMap::IteratorState &state)
    : isArray_(false) {
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

ValueIteratorBase::difference_type ValueIteratorBase::computeDistance(
    const SelfType &other) const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
#ifdef JSON_USE_CPPTL_SMALLMAP
    return current_ - other.current_;
#else

    if (isNull_ && other.isNull_) { return 0; }

    difference_type myDistance = 0;
    for (Value::ObjectValues::iterator it = current_; it != other.current_;
         ++it) {
        ++myDistance;
    }
    return myDistance;
#endif
#else
    if (isArray_)
        return ValueInternalArray::distance(iterator_.array_,
                                            other.iterator_.array_);
    return ValueInternalMap::distance(iterator_.map_, other.iterator_.map_);
#endif
}

bool ValueIteratorBase::isEqual(const SelfType &other) const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    if (isNull_) { return other.isNull_; }
    return current_ == other.current_;
#else
    if (isArray_)
        return ValueInternalArray::equals(iterator_.array_,
                                          other.iterator_.array_);
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
        if (czstring.isStaticString())
            return Value(StaticString(czstring.c_str()));
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
    if (isArray_)
        return Value::UInt(ValueInternalArray::indexOf(iterator_.array_));
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

ValueConstIterator::ValueConstIterator(
    const Value::ObjectValues::iterator &current)
    : ValueIteratorBase(current) {}

#else
ValueConstIterator::ValueConstIterator(
    const ValueInternalArray::IteratorState &state)
    : ValueIteratorBase(state) {}

ValueConstIterator::ValueConstIterator(
    const ValueInternalMap::IteratorState &state)
    : ValueIteratorBase(state) {}
#endif

ValueConstIterator &ValueConstIterator::operator=(
    const ValueIteratorBase &other) {
    copy(other);
    return *this;
}

ValueIterator::ValueIterator() {}

#ifndef JSON_VALUE_USE_INTERNAL_MAP

ValueIterator::ValueIterator(const Value::ObjectValues::iterator &current)
    : ValueIteratorBase(current) {}

#else
ValueIterator::ValueIterator(const ValueInternalArray::IteratorState &state)
    : ValueIteratorBase(state) {}

ValueIterator::ValueIterator(const ValueInternalMap::IteratorState &state)
    : ValueIteratorBase(state) {}
#endif

ValueIterator::ValueIterator(const ValueConstIterator &other)
    : ValueIteratorBase(other) {}

ValueIterator::ValueIterator(const ValueIterator &other)
    : ValueIteratorBase(other) {}

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

static inline char *duplicateStringValue(const char *value,
                                         unsigned int length = unknown) {
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
    JSON_ASSERT_MESSAGE(
        text[0] == '\0' || text[0] == '/',
        "in Json::Value::setComment(): Comments must start with /");

    comment_ = duplicateStringValue(text);
}

#ifndef JSON_VALUE_USE_INTERNAL_MAP

Value::CZString::CZString(ArrayIndex index) : cstr_(0), index_(index) {}

Value::CZString::CZString(const char *cstr, DuplicationPolicy allocate)
    : cstr_(allocate == duplicate ? duplicateStringValue(cstr) : cstr),
      index_(allocate) {}

Value::CZString::CZString(const CZString &other)
    : cstr_(other.index_ != noDuplication && other.cstr_ != 0 ?
                duplicateStringValue(other.cstr_) :
                other.cstr_),
      index_(other.cstr_ ?
                 (other.index_ == noDuplication ? noDuplication : duplicate) :
                 other.index_) {}

Value::CZString::~CZString() {
    if (cstr_ && index_ == duplicate)
        releaseStringValue(const_cast<char *>(cstr_));
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
    value_.string_ =
        duplicateStringValue(beginValue, (unsigned int)(endValue - beginValue));
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
    value_.string_ =
        duplicateStringValue(value.c_str(), (unsigned int)value.length());
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
            value_.array_ =
                arrayAllocator()->newArrayCopy(*other.value_.array_);
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
            if (otherComment.comment_)
                comments_[comment].setComment(otherComment.comment_);
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
                   (other.value_.string_ && value_.string_ &&
                    strcmp(value_.string_, other.value_.string_) < 0);
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
                   (other.value_.string_ && value_.string_ &&
                    strcmp(value_.string_, other.value_.string_) == 0);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
        case objectValue:
            return value_.map_->size() == other.value_.map_->size() &&
                   (*value_.map_) == (*other.value_.map_);
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
    JSON_ASSERT_MESSAGE(type_ == stringValue,
                        "in Json::Value::asCString(): requires stringValue");
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
CppTL::ConstString Value::asConstString() const {
    return CppTL::ConstString(asString().c_str());
}
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
            JSON_ASSERT_MESSAGE(InRange(value_.real_, minInt, maxInt),
                                "double out of Int range");
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
            JSON_ASSERT_MESSAGE(InRange(value_.real_, 0, maxUInt),
                                "double out of UInt range");
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
            JSON_ASSERT_MESSAGE(InRange(value_.real_, minInt64, maxInt64),
                                "double out of Int64 range");
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
            JSON_ASSERT_MESSAGE(InRange(value_.real_, 0, maxUInt64),
                                "double out of UInt64 range");
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
            return (isNumeric() && asDouble() == 0.0) ||
                   (type_ == booleanValue && value_.bool_ == false) ||
                   (type_ == stringValue && asString() == "") ||
                   (type_ == arrayValue && value_.map_->size() == 0) ||
                   (type_ == objectValue && value_.map_->size() == 0) ||
                   type_ == nullValue;
        case intValue:
            return isInt() ||
                   (type_ == realValue &&
                    InRange(value_.real_, minInt, maxInt)) ||
                   type_ == booleanValue || type_ == nullValue;
        case uintValue:
            return isUInt() ||
                   (type_ == realValue && InRange(value_.real_, 0, maxUInt)) ||
                   type_ == booleanValue || type_ == nullValue;
        case realValue:
            return isNumeric() || type_ == booleanValue || type_ == nullValue;
        case booleanValue:
            return isNumeric() || type_ == booleanValue || type_ == nullValue;
        case stringValue:
            return isNumeric() || type_ == booleanValue ||
                   type_ == stringValue || type_ == nullValue;
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
    JSON_ASSERT_MESSAGE(
        type_ == nullValue || type_ == arrayValue || type_ == objectValue,
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
    JSON_ASSERT_MESSAGE(type_ == nullValue || type_ == arrayValue,
                        "in Json::Value::resize(): requires arrayValue");
    if (type_ == nullValue) *this = Value(arrayValue);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    ArrayIndex oldSize = size();
    if (newSize == 0)
        clear();
    else if (newSize > oldSize)
        (*this)[newSize - 1];
    else {
        for (ArrayIndex index = newSize; index < oldSize; ++index) {
            value_.map_->erase(index);
        }
        assert(size() == newSize);
    }
#else
    value_.array_->resize(newSize);
#endif
}

Value &Value::operator[](ArrayIndex index) {
    JSON_ASSERT_MESSAGE(
        type_ == nullValue || type_ == arrayValue,
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
    JSON_ASSERT_MESSAGE(
        index >= 0,
        "in Json::Value::operator[](int index): index cannot be negative");
    return (*this)[ArrayIndex(index)];
}

const Value &Value::operator[](ArrayIndex index) const {
    JSON_ASSERT_MESSAGE(
        type_ == nullValue || type_ == arrayValue,
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

Value &Value::operator[](const char *key) {
    return resolveReference(key, false);
}

Value &Value::resolveReference(const char *key, bool isStatic) {
    JSON_ASSERT_MESSAGE(
        type_ == nullValue || type_ == objectValue,
        "in Json::Value::resolveReference(): requires objectValue");
    if (type_ == nullValue) *this = Value(objectValue);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    CZString actualKey(
        key, isStatic ? CZString::noDuplication : CZString::duplicateOnCopy);
    ObjectValues::iterator it = value_.map_->lower_bound(actualKey);
    if (it != value_.map_->end() && (*it).first == actualKey)
        return (*it).second;

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
    JSON_ASSERT_MESSAGE(
        type_ == nullValue || type_ == objectValue,
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

Value &Value::operator[](const std::string &key) {
    return (*this)[key.c_str()];
}

const Value &Value::operator[](const std::string &key) const {
    return (*this)[key.c_str()];
}

Value &Value::operator[](const StaticString &key) {
    return resolveReference(key, true);
}

#ifdef JSON_USE_CPPTL
Value &Value::operator[](const CppTL::ConstString &key) {
    return (*this)[key.c_str()];
}

const Value &Value::operator[](const CppTL::ConstString &key) const {
    return (*this)[key.c_str()];
}
#endif

Value &Value::append(const Value &value) { return (*this)[size()] = value; }

Value Value::get(const char *key, const Value &defaultValue) const {
    const Value *value = &((*this)[key]);
    return value == &null ? defaultValue : *value;
}

Value Value::get(const std::string &key, const Value &defaultValue) const {
    return get(key.c_str(), defaultValue);
}

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

Value Value::removeMember(const std::string &key) {
    return removeMember(key.c_str());
}

#ifdef JSON_USE_CPPTL
Value Value::get(const CppTL::ConstString &key,
                 const Value &defaultValue) const {
    return get(key.c_str(), defaultValue);
}
#endif

bool Value::isMember(const char *key) const {
    const Value *value = &((*this)[key]);
    return value != &null;
}

bool Value::isMember(const std::string &key) const {
    return isMember(key.c_str());
}

#ifdef JSON_USE_CPPTL
bool Value::isMember(const CppTL::ConstString &key) const {
    return isMember(key.c_str());
}
#endif

Value::Members Value::getMemberNames() const {
    JSON_ASSERT_MESSAGE(
        type_ == nullValue || type_ == objectValue,
        "in Json::Value::getMemberNames(), value must be objectValue");
    if (type_ == nullValue) return Value::Members();
    Members members;
    members.reserve(value_.map_->size());
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    ObjectValues::const_iterator it = value_.map_->begin();
    ObjectValues::const_iterator itEnd = value_.map_->end();
    for (; it != itEnd; ++it)
        members.push_back(std::string((*it).first.c_str()));
#else
    ValueInternalMap::IteratorState it;
    ValueInternalMap::IteratorState itEnd;
    value_.map_->makeBeginIterator(it);
    value_.map_->makeEndIterator(itEnd);
    for (; !ValueInternalMap::equals(it, itEnd);
         ValueInternalMap::increment(it))
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
            return value_.real_ >= minInt && value_.real_ <= maxInt &&
                   IsIntegral(value_.real_);
        default:
            break;
    }
    return false;
}

bool Value::isUInt() const {
    switch (type_) {
        case intValue:
            return value_.int_ >= 0 &&
                   LargestUInt(value_.int_) <= LargestUInt(maxUInt);
        case uintValue:
            return value_.uint_ <= maxUInt;
        case realValue:
            return value_.real_ >= 0 && value_.real_ <= maxUInt &&
                   IsIntegral(value_.real_);
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

            return value_.real_ >= double(minInt64) &&
                   value_.real_ < double(maxInt64) && IsIntegral(value_.real_);
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

            return value_.real_ >= 0 && value_.real_ < maxUInt64AsDouble &&
                   IsIntegral(value_.real_);
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

PathArgument::PathArgument(ArrayIndex index)
    : key_(), index_(index), kind_(kindIndex) {}

PathArgument::PathArgument(const char *key)
    : key_(key), index_(), kind_(kindKey) {}

PathArgument::PathArgument(const std::string &key)
    : key_(key.c_str()), index_(), kind_(kindKey) {}

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
                for (; current != end && *current >= '0' && *current <= '9';
                     ++current)
                    index = index * 10 + ArrayIndex(*current - '0');
                args_.push_back(index);
            }
            if (current == end || *current++ != ']')
                invalidPath(path, int(current - path.c_str()));
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
            if (!node->isArray() || !node->isValidIndex(arg.index_))
                return defaultValue;
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

std::string valueToString(Int value) {
    return valueToString(LargestInt(value));
}

std::string valueToString(UInt value) {
    return valueToString(LargestUInt(value));
}

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

    if (strpbrk(value, "\"\\\b\f\n\r\t") == NULL &&
        !containsControlCharacter(value))
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
                    oss << "\\u" << std::hex << std::uppercase
                        << std::setfill('0') << std::setw(4)
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

FastWriter::FastWriter()
    : yamlCompatiblityEnabled_(false),
      dropNullPlaceholders_(false),
      omitEndingLineFeed_(false) {}

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
            for (Value::Members::iterator it = members.begin();
                 it != members.end(); ++it) {
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

StyledWriter::StyledWriter()
    : rightMargin_(74), indentSize_(3), addChildValues_() {}

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
        isMultiLine =
            isMultiLine || ((childValue.isArray() || childValue.isObject()) &&
                            childValue.size() > 0);
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
    std::string normalizedComment =
        normalizeEOL(root.getComment(commentBefore));
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
        document_ +=
            " " + normalizeEOL(root.getComment(commentAfterOnSameLine));

    if (root.hasComment(commentAfter)) {
        document_ += "\n";
        document_ += normalizeEOL(root.getComment(commentAfter));
        document_ += "\n";
    }
}

bool StyledWriter::hasCommentForValue(const Value &value) {
    return value.hasComment(commentBefore) ||
           value.hasComment(commentAfterOnSameLine) ||
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
    : document_(NULL),
      rightMargin_(74),
      indentation_(indentation),
      addChildValues_() {}

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
        isMultiLine =
            isMultiLine || ((childValue.isArray() || childValue.isObject()) &&
                            childValue.size() > 0);
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
        *document_ << " " +
                          normalizeEOL(root.getComment(commentAfterOnSameLine));

    if (root.hasComment(commentAfter)) {
        *document_ << "\n";
        *document_ << normalizeEOL(root.getComment(commentAfter));
        *document_ << "\n";
    }
}

bool StyledStreamWriter::hasCommentForValue(const Value &value) {
    return value.hasComment(commentBefore) ||
           value.hasComment(commentAfterOnSameLine) ||
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

#include <algorithm>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#ifndef TANK_TEMPLATE_H
#define TANK_TEMPLATE_H
#define LZZ_INLINE inline
namespace TankGame {
enum GameResult { NotFinished = -2, Draw = -1, Blue = 0, Red = 1 };
}
namespace TankGame {
using std::istream;
using std::set;
using std::stack;
using std::string;
}  // namespace TankGame
namespace TankGame {
enum FieldItem {
    None = 0,
    Brick = 1,
    Steel = 2,
    Base = 4,
    Blue0 = 8,
    Blue1 = 16,
    Red0 = 32,
    Red1 = 64,
    Water = 128
};
}
namespace TankGame {
enum FireDirection { Nof = 0, X = 1, Y = 2, Both = 3 };
}
namespace TankGame {
template <typename T>
T operator~(T a);
}
namespace TankGame {
template <typename T>
T operator|(T a, T b);
}
namespace TankGame {
template <typename T>
T operator&(T a, T b);
}
namespace TankGame {
template <typename T>
T operator^(T a, T b);
}
namespace TankGame {
template <typename T>
T &operator|=(T &a, T b);
}
namespace TankGame {
template <typename T>
T &operator&=(T &a, T b);
}
namespace TankGame {
template <typename T>
T &operator^=(T &a, T b);
}
namespace TankGame {
enum Action {
    Invalid = -2,
    Stay = -1,
    Up,
    Right,
    Down,
    Left,
    UpShoot,
    RightShoot,
    DownShoot,
    LeftShoot
};
}
namespace TankGame {
int const fieldHeight = 9;
}
namespace TankGame {
int const fieldWidth = 9;
}
namespace TankGame {
int const sideCount = 2;
}
namespace TankGame {
int const tankPerSide = 2;
}
namespace TankGame {
extern int const(baseX)[sideCount];
}
namespace TankGame {
extern int const(baseY)[sideCount];
}
namespace TankGame {
extern int const(dx)[4];
}
namespace TankGame {
extern int const(dy)[4];
}
namespace TankGame {
extern FieldItem const((tankItemTypes)[sideCount])[tankPerSide];
}
namespace TankGame {
extern int maxTurn;
}
namespace TankGame {
bool ActionIsMove(Action x);
}
namespace TankGame {
bool ActionIsShoot(Action x);
}
namespace TankGame {
bool ActionDirectionIsOpposite(Action a, Action b);
}
namespace TankGame {
bool CoordValid(int x, int y);
}
namespace TankGame {
bool HasMultipleTank(FieldItem item);
}
namespace TankGame {
int GetTankSide(FieldItem item);
}
namespace TankGame {
int GetTankID(FieldItem item);
}
namespace TankGame {
int ExtractDirectionFromAction(Action x);
}
namespace TankGame {
struct DisappearLog {
    FieldItem item;
    int turn;
    int x;
    int y;

    bool operator<(DisappearLog const &b) const;
};
}  // namespace TankGame
namespace TankGame {
class TankField {
   public:
    FireDirection mark[sideCount][fieldHeight][fieldWidth] = {};
    int distance[sideCount][fieldHeight][fieldWidth] = {};
    FieldItem gameField[fieldHeight][fieldWidth];
    bool tankAlive[sideCount][tankPerSide] = {{true, true}, {true, true}};
    bool baseAlive[sideCount] = {true, true};
    bool shouldInit = false;
    int tankX[sideCount][tankPerSide] = {
        {fieldWidth / 2 - 2, fieldWidth / 2 + 2},
        {fieldWidth / 2 + 2, fieldWidth / 2 - 2}};
    int tankY[sideCount][tankPerSide] = {{0, 0},
                                         {fieldHeight - 1, fieldHeight - 1}};
    int currentTurn = 1;
    int mySide;
    stack<DisappearLog> logs;
    Action previousActions[101][sideCount][tankPerSide] = {
        {{Stay, Stay}, {Stay, Stay}}};
    Action nextAction[sideCount][tankPerSide] = {{Invalid, Invalid},
                                                 {Invalid, Invalid}};

    bool ActionIsValid(int side, int tank, Action act);

    bool ActionIsValid();

   private:
    void _destroyTank(int side, int tank);

    void _revertTank(int side, int tank, DisappearLog &log);

   public:
    bool DoAction();

    bool Revert();

    GameResult GetGameResult();

    TankField(int(hasBrick)[3], int(hasWater)[3], int(hasSteel)[3], int mySide);

    void DebugPrint();

    bool operator!=(TankField const &b) const;
};
}  // namespace TankGame
namespace TankGame {
extern TankField *field;
}
namespace TankGame {
namespace Internals {
extern Json::Reader reader;
}
}  // namespace TankGame
namespace TankGame {
namespace Internals {
extern Json::StyledWriter writer;
}
}  // namespace TankGame
namespace TankGame {
namespace Internals {
void _processRequestOrResponse(Json::Value &value, bool isOpponent);
}
}  // namespace TankGame
namespace TankGame {
namespace Internals {
void _submitAction(Action tank0,
                   Action tank1,
                   string debug = "",
                   string data = "",
                   string globalData = "");
}
}  // namespace TankGame
namespace TankGame {
void ReadInput(istream &in, string &outData, string &outGlobalData);
}
namespace TankGame {
void SubmitAndExit(Action tank0,
                   Action tank1,
                   string debug = "",
                   string data = "",
                   string globalData = "");
}
namespace TankGame {
void SubmitAndDontExit(Action tank0, Action tank1);
}

int RandBetween(int from, int to);

TankGame::Action RandAction(int tank);

namespace TankGame {
template <typename T>
LZZ_INLINE T operator~(T a) {
    return (T) ~(int)a;
}
}  // namespace TankGame
namespace TankGame {
template <typename T>
LZZ_INLINE T operator|(T a, T b) {
    return (T)((int)a | (int)b);
}
}  // namespace TankGame
namespace TankGame {
template <typename T>
LZZ_INLINE T operator&(T a, T b) {
    return (T)((int)a & (int)b);
}
}  // namespace TankGame
namespace TankGame {
template <typename T>
LZZ_INLINE T operator^(T a, T b) {
    return (T)((int)a ^ (int)b);
}
}  // namespace TankGame
namespace TankGame {
template <typename T>
LZZ_INLINE T &operator|=(T &a, T b) {
    return (T &)((int &)a |= (int)b);
}
}  // namespace TankGame
namespace TankGame {
template <typename T>
LZZ_INLINE T &operator&=(T &a, T b) {
    return (T &)((int &)a &= (int)b);
}
}  // namespace TankGame
namespace TankGame {
template <typename T>
LZZ_INLINE T &operator^=(T &a, T b) {
    return (T &)((int &)a ^= (int)b);
}
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE bool ActionIsMove(Action x) { return x >= Up && x <= Left; }
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE bool ActionIsShoot(Action x) {
    return x >= UpShoot && x <= LeftShoot;
}
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE bool ActionDirectionIsOpposite(Action a, Action b) {
    return a >= Up && b >= Up && (a + 2) % 4 == b % 4;
}
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE bool CoordValid(int x, int y) {
    return x >= 0 && x < fieldWidth && y >= 0 && y < fieldHeight;
}
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE bool HasMultipleTank(FieldItem item) {
    // 如果格子上只有一个物件，那么 item 的值是 2 的幂或 0
    // 对于数字 x，x & (x - 1) == 0 当且仅当 x 是 2 的幂或 0
    return !!(item & (item - 1));
}
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE int GetTankSide(FieldItem item) {
    return item == Blue0 || item == Blue1 ? Blue : Red;
}
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE int GetTankID(FieldItem item) {
    return item == Blue0 || item == Red0 ? 0 : 1;
}
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE int ExtractDirectionFromAction(Action x) {
    if (x >= Up) return x % 4;
    return -1;
}
}  // namespace TankGame
#undef LZZ_INLINE
#endif

#define LZZ_INLINE inline
using std::cin;
using std::cout;
using std::endl;
using std::flush;
using std::getline;
using std::greater;
using std::pair;
using std::priority_queue;
using std::queue;
using std::string;
using std::vector;
namespace TankGame {
int const(baseX)[sideCount] = {fieldWidth / 2, fieldWidth / 2};
}
namespace TankGame {
int const(baseY)[sideCount] = {0, fieldHeight - 1};
}
namespace TankGame {
int const(dx)[4] = {0, 1, 0, -1};
}
namespace TankGame {
int const(dy)[4] = {-1, 0, 1, 0};
}
namespace TankGame {
FieldItem const((tankItemTypes)[sideCount])[tankPerSide] = {{Blue0, Blue1},
                                                            {Red0, Red1}};
}
namespace TankGame {
int maxTurn = 100;
}
namespace TankGame {
bool DisappearLog::operator<(DisappearLog const &b) const {
    if (x == b.x) {
        if (y == b.y) return item < b.item;
        return y < b.y;
    }
    return x < b.x;
}
}  // namespace TankGame
namespace TankGame {
bool TankField::ActionIsValid(int side, int tank, Action act) {
    if (act == Invalid) return false;
    if (act > Left &&
        previousActions[currentTurn - 1][side][tank] > Left)  // 连续两回合射击
        return false;
    if (act == Stay || act > Left) return true;
    int x = tankX[side][tank] + dx[act], y = tankY[side][tank] + dy[act];
    return CoordValid(x, y) &&
           gameField[y][x] == None;  // water cannot be stepped on
}
}  // namespace TankGame
namespace TankGame {
bool TankField::ActionIsValid() {
    for (int side = 0; side < sideCount; side++)
        for (int tank = 0; tank < tankPerSide; tank++)
            if (tankAlive[side][tank] &&
                !ActionIsValid(side, tank, nextAction[side][tank]))
                return false;
    return true;
}
}  // namespace TankGame
namespace TankGame {
void TankField::_destroyTank(int side, int tank) {
    tankAlive[side][tank] = false;
    tankX[side][tank] = tankY[side][tank] = -1;
}
}  // namespace TankGame
namespace TankGame {
void TankField::_revertTank(int side, int tank, DisappearLog &log) {
    int &currX = tankX[side][tank], &currY = tankY[side][tank];
    if (tankAlive[side][tank])
        gameField[currY][currX] &= ~tankItemTypes[side][tank];
    else
        tankAlive[side][tank] = true;
    currX = log.x;
    currY = log.y;
    gameField[currY][currX] |= tankItemTypes[side][tank];
}
}  // namespace TankGame
namespace TankGame {
bool TankField::DoAction() {
    if (!ActionIsValid()) return false;

    // 1 移动
    for (int side = 0; side < sideCount; side++)
        for (int tank = 0; tank < tankPerSide; tank++) {
            Action act = nextAction[side][tank];

            // 保存动作
            previousActions[currentTurn][side][tank] = act;
            if (tankAlive[side][tank] && ActionIsMove(act)) {
                int &x = tankX[side][tank], &y = tankY[side][tank];
                FieldItem &items = gameField[y][x];

                // 记录 Log
                DisappearLog log;
                log.x = x;
                log.y = y;
                log.item = tankItemTypes[side][tank];
                log.turn = currentTurn;
                logs.push(log);

                // 变更坐标
                x += dx[act];
                y += dy[act];

                // 更换标记（注意格子可能有多个坦克）
                gameField[y][x] |= log.item;
                items &= ~log.item;
            }
        }

    // 2 射击!
    set<DisappearLog> itemsToBeDestroyed;
    for (int side = 0; side < sideCount; side++)
        for (int tank = 0; tank < tankPerSide; tank++) {
            Action act = nextAction[side][tank];
            if (tankAlive[side][tank] && ActionIsShoot(act)) {
                int dir = ExtractDirectionFromAction(act);
                int x = tankX[side][tank], y = tankY[side][tank];
                bool hasMultipleTankWithMe = HasMultipleTank(gameField[y][x]);
                while (true) {
                    x += dx[dir];
                    y += dy[dir];
                    if (!CoordValid(x, y)) break;
                    FieldItem items = gameField[y][x];
                    // tank will not be on water, and water will not be shot, so
                    // it can be handled as None
                    if (items != None && items != Water) {
                        // 对射判断
                        if (items >= Blue0 && !hasMultipleTankWithMe &&
                            !HasMultipleTank(items)) {
                            // 自己这里和射到的目标格子都只有一个坦克
                            Action theirAction = nextAction[GetTankSide(items)]
                                                           [GetTankID(items)];
                            if (ActionIsShoot(theirAction) &&
                                ActionDirectionIsOpposite(act, theirAction)) {
                                // 而且我方和对方的射击方向是反的
                                // 那么就忽视这次射击
                                break;
                            }
                        }

                        // 标记这些物件要被摧毁了（防止重复摧毁）
                        for (int mask = 1; mask <= Red1; mask <<= 1)
                            if (items & mask) {
                                DisappearLog log;
                                log.x = x;
                                log.y = y;
                                log.item = (FieldItem)mask;
                                log.turn = currentTurn;
                                itemsToBeDestroyed.insert(log);
                            }
                        break;
                    }
                }
            }
        }

    for (auto &log : itemsToBeDestroyed) {
        switch (log.item) {
            case Base: {
                int side =
                    log.x == baseX[Blue] && log.y == baseY[Blue] ? Blue : Red;
                baseAlive[side] = false;
                break;
            }
            case Blue0:
                _destroyTank(Blue, 0);
                break;
            case Blue1:
                _destroyTank(Blue, 1);
                break;
            case Red0:
                _destroyTank(Red, 0);
                break;
            case Red1:
                _destroyTank(Red, 1);
                break;
            case Steel:
                continue;
            default:;
        }
        gameField[log.y][log.x] &= ~log.item;
        logs.push(log);
    }

    for (int side = 0; side < sideCount; side++)
        for (int tank = 0; tank < tankPerSide; tank++)
            nextAction[side][tank] = Invalid;

    currentTurn++;
    return true;
}
}  // namespace TankGame
namespace TankGame {
bool TankField::Revert() {
    if (currentTurn == 1) return false;

    currentTurn--;
    while (!logs.empty()) {
        DisappearLog &log = logs.top();
        if (log.turn == currentTurn) {
            logs.pop();
            switch (log.item) {
                case Base: {
                    int side = log.x == baseX[Blue] && log.y == baseY[Blue] ?
                                   Blue :
                                   Red;
                    baseAlive[side] = true;
                    gameField[log.y][log.x] = Base;
                    break;
                }
                case Brick:
                    gameField[log.y][log.x] = Brick;
                    break;
                case Blue0:
                    _revertTank(Blue, 0, log);
                    break;
                case Blue1:
                    _revertTank(Blue, 1, log);
                    break;
                case Red0:
                    _revertTank(Red, 0, log);
                    break;
                case Red1:
                    _revertTank(Red, 1, log);
                    break;
                default:;
            }
        } else
            break;
    }
    return true;
}
}  // namespace TankGame
namespace TankGame {
GameResult TankField::GetGameResult() {
    bool fail[sideCount] = {};
    for (int side = 0; side < sideCount; side++)
        if ((!tankAlive[side][0] && !tankAlive[side][1]) || !baseAlive[side])
            fail[side] = true;
    if (fail[0] == fail[1])
        return fail[0] || currentTurn > maxTurn ? Draw : NotFinished;
    if (fail[Blue]) return Red;
    return Blue;
}
}  // namespace TankGame
namespace TankGame {
TankField::TankField(int(hasBrick)[3],
                     int(hasWater)[3],
                     int(hasSteel)[3],
                     int mySide)
    : mySide(mySide) {
    for (int i = 0; i < 3; i++) {
        int mask = 1;
        for (int y = i * 3; y < (i + 1) * 3; y++) {
            for (int x = 0; x < fieldWidth; x++) {
                if (hasBrick[i] & mask)
                    gameField[y][x] = Brick;
                else if (hasWater[i] & mask)
                    gameField[y][x] = Water;
                else if (hasSteel[i] & mask)
                    gameField[y][x] = Steel;
                mask <<= 1;
            }
        }
    }
    for (int side = 0; side < sideCount; side++) {
        for (int tank = 0; tank < tankPerSide; tank++)
            gameField[tankY[side][tank]][tankX[side][tank]] =
                tankItemTypes[side][tank];
        gameField[baseY[side]][baseX[side]] = Base;
    }
}
}  // namespace TankGame
namespace TankGame {
void TankField::DebugPrint() {
#ifndef _BOTZONE_ONLINE
    const string side2String[] = {"blue", "red"};
    const string boolean2String[] = {"exploded", "alive"};
    const char *boldHR = "==============================";
    const char *slimHR = "------------------------------";
    cout << boldHR << endl
         << "legend: " << endl
         << ". - empty\t# - brick\t% - steel\t* - base\t@ - more than 1 tanks"
         << endl
         << "b - blue0\tB - blue1\tr - red0\tR - red1\tW - water"
         << endl  // Tank2 feature
         << slimHR << endl;
    for (int y = 0; y < fieldHeight; y++) {
        for (int x = 0; x < fieldWidth; x++) {
            switch (gameField[y][x]) {
                case None:
                    cout << '.';
                    break;
                case Brick:
                    cout << '#';
                    break;
                case Steel:
                    cout << '%';
                    break;
                case Base:
                    cout << '*';
                    break;
                case Blue0:
                    cout << 'b';
                    break;
                case Blue1:
                    cout << 'B';
                    break;
                case Red0:
                    cout << 'r';
                    break;
                case Red1:
                    cout << 'R';
                    break;
                case Water:
                    cout << 'W';
                    break;
                default:
                    cout << '@';
                    break;
            }
            cout << ' ';
        }
        cout << endl;
    }
    // cout << "blue distance:" << endl;
    // for (int i = 0; i < fieldHeight; i++) {
    //     for (int j = 0; j < fieldWidth; j++) {
    //         cout << std::setw(2) << distance[0][i][j] << ' ';
    //     }
    //     cout << endl;
    // }
    // cout << endl;
    // cout << "red distance:" << endl;
    // for (int i = 0; i < fieldHeight; i++) {
    //     for (int j = 0; j < fieldWidth; j++) {
    //         cout << std::setw(2) << distance[1][i][j] << ' ';
    //     }
    //     cout << endl;
    // }
    cout << slimHR << endl;
    for (int side = 0; side < sideCount; side++) {
        cout << side2String[side] << ": base" << ' '
             << boolean2String[baseAlive[side]];
        for (int tank = 0; tank < tankPerSide; tank++)
            cout << ", tank" << tank << ' '
                 << boolean2String[tankAlive[side][tank]];
        cout << endl;
    }
    cout << "current turn: " << currentTurn << ", ";
    GameResult result = GetGameResult();
    if (result == -2)
        cout << "The game is not over" << endl;
    else if (result == -1)
        cout << "Draw" << endl;
    else
        cout << side2String[result] << " wins" << endl;
    cout << boldHR << endl;
#endif
}
}  // namespace TankGame
namespace TankGame {
bool TankField::operator!=(TankField const &b) const {
    for (int y = 0; y < fieldHeight; y++)
        for (int x = 0; x < fieldWidth; x++)
            if (gameField[y][x] != b.gameField[y][x]) return true;

    for (int side = 0; side < sideCount; side++)
        for (int tank = 0; tank < tankPerSide; tank++) {
            if (tankX[side][tank] != b.tankX[side][tank]) return true;
            if (tankY[side][tank] != b.tankY[side][tank]) return true;
            if (tankAlive[side][tank] != b.tankAlive[side][tank]) return true;
        }

    if (baseAlive[0] != b.baseAlive[0] || baseAlive[1] != b.baseAlive[1])
        return true;

    if (currentTurn != b.currentTurn) return true;

    return false;
}
}  // namespace TankGame
namespace TankGame {
TankField *field;
}
namespace TankGame {
namespace Internals {
Json::Reader reader;
}
}  // namespace TankGame
namespace TankGame {
namespace Internals {
Json::StyledWriter writer;
}
}  // namespace TankGame
namespace TankGame {
namespace Internals {
void _processRequestOrResponse(Json::Value &value, bool isOpponent) {
    if (value.isArray()) {
        if (!isOpponent) {
            for (int tank = 0; tank < tankPerSide; tank++)
                field->nextAction[field->mySide][tank] =
                    (Action)value[tank].asInt();
        } else {
            for (int tank = 0; tank < tankPerSide; tank++)
                field->nextAction[1 - field->mySide][tank] =
                    (Action)value[tank].asInt();
            field->DoAction();
        }
    } else {
        // 是第一回合，裁判在介绍场地
        if (!field || field->shouldInit) {
            int hasBrick[3], hasWater[3], hasSteel[3];
            for (int i = 0; i < 3; i++) {  // Tank2 feature(?????????????]
                hasWater[i] = value["waterfield"][i].asInt();
                hasBrick[i] = value["brickfield"][i].asInt();
                hasSteel[i] = value["steelfield"][i].asInt();
            }
            field = new TankField(hasBrick, hasWater, hasSteel,
                                  value["mySide"].asInt());
        }
    }
}
}  // namespace Internals
}  // namespace TankGame
namespace TankGame {
namespace Internals {
void _submitAction(Action tank0,
                   Action tank1,
                   string debug,
                   string data,
                   string globalData) {
    Json::Value output(Json::objectValue), response(Json::arrayValue);
    response[0U] = tank0;
    response[1U] = tank1;
    output["response"] = response;
    if (!debug.empty()) output["debug"] = debug;
    if (!data.empty()) output["data"] = data;
    if (!globalData.empty()) output["globalData"] = globalData;
    cout << writer.write(output) << endl;
}
}  // namespace Internals
}  // namespace TankGame
namespace TankGame {
void ReadInput(istream &in, string &outData, string &outGlobalData) {
    Json::Value input;
    string inputString;
    do { getline(in, inputString); } while (inputString.empty());
#ifndef _BOTZONE_ONLINE
    // 猜测是单行还是多行
    char lastChar = inputString[inputString.size() - 1];
    if (lastChar != '}' && lastChar != ']') {
        // 第一行不以}或]结尾，猜测是多行
        string newString;
        do {
            getline(in, newString);
            inputString += newString;
        } while (newString != "}" && newString != "]");
    }
#endif
    Internals::reader.parse(inputString, input);

    if (input.isObject()) {
        Json::Value requests = input["requests"],
                    responses = input["responses"];
        if (responses.empty() && field) field->shouldInit = true;
        if (!requests.isNull() && requests.isArray()) {
            size_t i, n = requests.size();
            for (i = 0; i < n; i++) {
                Internals::_processRequestOrResponse(
                    requests[(Json::Value::UInt)i], true);
                if (i < n - 1)
                    Internals::_processRequestOrResponse(
                        responses[(Json::Value::UInt)i], false);
            }
            outData = input["data"].asString();
            outGlobalData = input["globaldata"].asString();
            return;
        }
    }
    // Internals::_processRequestOrResponse(input, true); 这行好像永远不会执行的
}
}  // namespace TankGame
namespace TankGame {
void SubmitAndExit(Action tank0,
                   Action tank1,
                   string debug,
                   string data,
                   string globalData) {
    Internals::_submitAction(tank0, tank1, debug, data, globalData);
    exit(0);
}
}  // namespace TankGame
namespace TankGame {
void SubmitAndDontExit(Action tank0, Action tank1) {
    Internals::_submitAction(tank0, tank1);
    field->nextAction[field->mySide][0] = tank0;
    field->nextAction[field->mySide][1] = tank1;
    cout << ">>>BOTZONE_REQUEST_KEEP_RUNNING<<<" << endl;
}
}  // namespace TankGame

int RandBetween(int from, int to) { return rand() % (to - from) + from; }

TankGame::Action RandAction(int tank) {
    while (true) {
        auto act = (TankGame::Action)RandBetween(TankGame::Stay,
                                                 TankGame::LeftShoot + 1);
        if (TankGame::field->ActionIsValid(TankGame::field->mySide, tank, act))
            return act;
    }
}

#undef LZZ_INLINE

#ifndef BOT
#define BOT

// Policy, act_0, act_1 \in [-1,7], -2 represents for empty policy
class Policy {
   public:
    int act_0, act_1;

    Policy() {}
    Policy(int act_0, int act_1) : act_0(act_0), act_1(act_1) {}

    bool operator<(const Policy &rhs) const {
        return (act_0 < rhs.act_0) || (act_0 == rhs.act_0 && act_1 < rhs.act_1);
    }

    bool empty() const { return act_0 == -2; }
};

// Monte-Carlo Tree Search, nodes are generated by both sides' policies
class Node {
   public:
    int vis, size;
    double val;
    Node *fa, *bstCh;
    std::map<std::pair<Policy, Policy>, Node *> ch;
    int dep;
    // first for Blue, second for Red
    std::pair<Policy, Policy> pol;
    bool full;

    Node(const std::pair<Policy, Policy> pol, Node *fa = nullptr)
        : vis(0),
          size(1),
          val(0.0),
          fa(fa),
          bstCh(nullptr),
          dep(fa ? fa->dep + 1 : 0),
          pol(pol),
          full(false) {}

    ~Node() {
        for (auto &p : ch) delete p.second;
    }

    int CountSize() {
        size = sizeof *this;
        for (auto &p : ch) size += p.second->CountSize();
        return size;
    }

    Node *NewChild(const std::pair<Policy, Policy> &pol) {
        return ch[pol] = new Node(pol, this);
    }

    void DelFather() {
        fa->ch[pol] = nullptr;
        delete fa;
        fa = nullptr;
    }
};

class Bot {
   public:
    const double C;
    const int TRAIN_UNIT = 100;

    // notice that the policy is in [-1,7], so we should +1 in the following
    double val[9][9][2];
    int vis[9][9][2];

    unsigned long long timing;

    const double TIME_LIMIT;

    Node *root;
    TankGame::TankField state;

    Bot(const TankGame::TankField &s, double C)
        : state(s), TIME_LIMIT(0.8), C(C) {
        root = new Node(std::make_pair(Policy(-2, -2), Policy(-2, -2)));
    }
    ~Bot() { delete root; }

    void Update(Node *);

    bool IsFullyExpanded(Node *);

    void Move(const std::pair<Policy, Policy> &);

    Node *RandomMove(Node *);

    double Utility(TankGame::GameResult);

    void BackPropagation(Node *, double);

    void RollOut(Node *);

    bool MCTS();

    int Train();

    Policy GenDecision(bool);

    void Play(const std::pair<Policy, Policy> &);
};

#endif

void Print(Node *p) {
    for (auto i : p->ch) {
        printf("((%d, %d), (%d, %d)) %p\n", i.first.first.act_0,
               i.first.first.act_1, i.first.second.act_0, i.first.second.act_1,
               i.second);
    }
    puts("");
}

bool Bot::MCTS() {
    TankGame::TankField copy = state;
    Node *p = root;
    for (; p; p = p->bstCh) {
        if (p != root) Move(p->pol);
        if (!IsFullyExpanded(p)) break;
    }
    if (!p) return std::swap(state, copy), 0;
    p = RandomMove(p);
    Move(p->pol);
    RollOut(p);
    std::swap(state, copy);
    return 1;
}

void Bot::Move(const std::pair<Policy, Policy> &pol) {
    if (pol.first.empty() && pol.second.empty()) return;
    state.nextAction[0][0] = TankGame::Action(pol.first.act_0),
    state.nextAction[0][1] = TankGame::Action(pol.first.act_1);
    state.nextAction[1][0] = TankGame::Action(pol.second.act_0);
    state.nextAction[1][1] = TankGame::Action(pol.second.act_1);
    state.DoAction();
    // state.DebugPrint();
}

bool Bot::IsFullyExpanded(Node *p) {
    if (p->full) return 1;
    for (int actBlue0 = -1; actBlue0 < 8; actBlue0++) {
        if (!state.tankAlive[0][0] && actBlue0 >= 0) break;
        for (int actBlue1 = -1; actBlue1 < 8; actBlue1++) {
            if (!state.tankAlive[0][1] && actBlue1 >= 0) break;
            for (int actRed0 = -1; actRed0 < 8; actRed0++) {
                if (!state.tankAlive[1][0] && actRed0 >= 0) break;
                for (int actRed1 = -1; actRed1 < 8; actRed1++) {
                    if (!state.tankAlive[1][1] && actRed1 >= 0) break;
                    if (state.ActionIsValid(0, 0, TankGame::Action(actBlue0)) &&
                        state.ActionIsValid(0, 1, TankGame::Action(actBlue1)) &&
                        state.ActionIsValid(1, 0, TankGame::Action(actRed0)) &&
                        state.ActionIsValid(1, 1, TankGame::Action(actRed1)) &&
                        !p->ch.count(std::make_pair(Policy(actBlue0, actBlue1),
                                                    Policy(actRed0, actRed1))))
                        return p->full = 0;
                }
            }
        }
    }
    return p->full = 1;
}

int shuffled_list0[] = {-1, 0, 1, 2, 3, 4, 5, 6, 7};
int shuffled_list1[] = {-1, 0, 1, 2, 3, 4, 5, 6, 7};
int shuffled_list2[] = {-1, 0, 1, 2, 3, 4, 5, 6, 7};
int shuffled_list3[] = {-1, 0, 1, 2, 3, 4, 5, 6, 7};

Node *Bot::RandomMove(Node *p) {
    if (IsFullyExpanded(p)) return nullptr;
    std::random_shuffle(shuffled_list0, shuffled_list0 + 9);
    std::random_shuffle(shuffled_list1, shuffled_list1 + 9);
    std::random_shuffle(shuffled_list2, shuffled_list2 + 9);
    std::random_shuffle(shuffled_list3, shuffled_list3 + 9);
    for (auto actBlue0 : shuffled_list0) {
        if (!state.tankAlive[0][0]) actBlue0 = -1;
        for (auto actBlue1 : shuffled_list1) {
            if (!state.tankAlive[0][1]) actBlue1 = -1;
            for (auto actRed0 : shuffled_list2) {
                if (!state.tankAlive[1][0]) actRed0 = -1;
                for (auto actRed1 : shuffled_list3) {
                    if (!state.tankAlive[1][1]) actRed1 = -1;
                    if (state.ActionIsValid(0, 0, TankGame::Action(actBlue0)) &&
                        state.ActionIsValid(0, 1, TankGame::Action(actBlue1)) &&
                        state.ActionIsValid(1, 0, TankGame::Action(actRed0)) &&
                        state.ActionIsValid(1, 1, TankGame::Action(actRed1))) {
                        std::pair<Policy, Policy> pol =
                            std::make_pair(Policy(actBlue0, actBlue1),
                                           Policy(actRed0, actRed1));
                        if (!p->ch.count(pol)) { return p->NewChild(pol); }
                    }
                    if (!state.tankAlive[1][1]) break;
                }
                if (!state.tankAlive[1][0]) break;
            }
            if (!state.tankAlive[0][1]) break;
        }
        if (!state.tankAlive[0][0]) break;
    }
    return nullptr;
}

double Bot::Utility(TankGame::GameResult res) {
    if (res == 0)
        return 1.0;
    else if (res == 1)
        return 0.0;
    else
        return 0.5;
}

// random roll-out until terminal state
void Bot::RollOut(Node *p) {
    Node *ch;
    for (Node *t = p;; t = ch, Move(t->pol)) {
        ch = RandomMove(t);
        TankGame::GameResult res = state.GetGameResult();
        if (res != TankGame::NotFinished) {
            if (!p->ch.empty()) {
                delete p->ch.begin()->second;
                p->full = 0;
                p->ch.clear();
            }
            BackPropagation(p, Utility(res));
            return;
        }
    }
}

void Bot::BackPropagation(Node *p, double utility) {
    while (p) {
        p->val += utility;
        p->vis++;
        Update(p);
        p = p->fa;
    }
}

// Using Decoupled UCT
void Bot::Update(Node *p) {
    if (p->ch.empty()) {
        p->bstCh = nullptr;
        return;
    }
    memset(val, 0, sizeof val);
    memset(vis, 0, sizeof vis);
    for (auto &i : p->ch) {
        // if(i.first.first.act_0==-2){
        //     puts("----");
        // }
        val[i.first.first.act_0 + 1][i.first.first.act_1 + 1][0] +=
            i.second->val;
        val[i.first.second.act_0 + 1][i.first.second.act_1 + 1][1] +=
            i.second->val;
        vis[i.first.first.act_0 + 1][i.first.first.act_1 + 1][0] +=
            i.second->vis;
        vis[i.first.second.act_0 + 1][i.first.second.act_1 + 1][1] +=
            i.second->vis;
    }
    double mx = -1, tmp;
    int act_1 = -1, act_2 = -1;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (vis[i][j][0]) {
                tmp = val[i][j][0] / vis[i][j][0] +  // Expectaion
                      C * sqrt(log(p->vis) / vis[i][j][0]);  // UCB length
                if (tmp > mx) {
                    mx = tmp;
                    act_1 = i, act_2 = j;
                }
            }
        }
    }
    Policy polBlue(act_1 - 1, act_2 - 1);
    mx = -1;
    act_1 = -1, act_2 = -1;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (vis[i][j][1]) {
                tmp = 1.0 - val[i][j][1] / vis[i][j][1] +  // Expectaion
                      C * sqrt(log(p->vis) / vis[i][j][1]);  // UCB length
                if (tmp > mx) {
                    mx = tmp;
                    act_1 = i, act_2 = j;
                }
            }
        }
    }
    Policy polRed(act_1 - 1, act_2 - 1);
    std::pair<Policy, Policy> pol = std::make_pair(polBlue, polRed);
    if (!p->ch.count(pol))
        p->bstCh = p->NewChild(pol);
    else
        p->bstCh = p->ch[pol];
}

#define DEBUG
#undef DEBUG

int Bot::Train() {
    int ret = 0;
#ifndef DEBUG
    while ((unsigned)clock() < timing) {
#else
    for (int __T = 0; __T < 60; __T++) {
#endif
        // printf("%d\n", root->CountSize());
        for (int i = 0; i < TRAIN_UNIT; i++) {
            if (!MCTS()) return ret;
        }
        ret++;
    }
    return ret;
}

// return mySide policy
Policy Bot::GenDecision(bool first) {
    if (first)
        timing = clock() + int((1.0 + TIME_LIMIT) * CLOCKS_PER_SEC);
    else
        timing = clock() + int(TIME_LIMIT * CLOCKS_PER_SEC);
    int res = Train();
    // printf("%d\n", res);
    if (root->ch.empty()) return Policy(-2, -2);
    memset(vis, 0, sizeof vis);
    memset(val, 0, sizeof val);
    bool flag = state.mySide == 1;
    if (flag) {
        for (auto &p : root->ch) {
            vis[p.first.second.act_0 + 1][p.first.second.act_1 + 1][0] +=
                p.second->vis;
            val[p.first.second.act_0 + 1][p.first.second.act_1 + 1][0] +=
                p.second->val;
        }
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (vis[i][j][0])
                    val[i][j][0] = 1 - val[i][j][0] / vis[i][j][0];
                else
                    val[i][j][0] = 0;
            }
        }
    } else {
        for (auto &p : root->ch) {
            vis[p.first.first.act_0 + 1][p.first.first.act_1 + 1][0] +=
                p.second->vis;
            val[p.first.first.act_0 + 1][p.first.first.act_1 + 1][0] +=
                p.second->val;
        }
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (vis[i][j][0])
                    val[i][j][0] = val[i][j][0] / vis[i][j][0];
                else
                    val[i][j][0] = 0;
            }
        }
    }
    // for (int i = 0; i < 9; i++) {
    //     for (int j = 0; j < 9; j++) { printf("%.2lf ", val[i][j][0]); }
    //     puts("");
    // }
    double max = 0;
    int act_0 = -1, act_1 = -1;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (max < val[i][j][0]) {
                max = val[i][j][0];
                act_0 = i, act_1 = j;
            }
        }
    }
    return Policy(act_0 - 1, act_1 - 1);
}

void Bot::Play(const std::pair<Policy, Policy> &pol) {
    if (!root->ch.count(pol))
        root = root->NewChild(pol);
    else
        root = root->ch[pol];
    root->DelFather();
    Move(root->pol);
}

using namespace TankGame;

const double TIME_LIMIT = 0.80;

int main() {
    // #ifndef _BOTZONE_ONLINE
    //     freopen("in", "r", stdin);
    // #endif
    srand((unsigned)time(0));
    string data, globaldata;
    ReadInput(std::cin, data, globaldata);
    Bot bot(*field, 0.9);
    Policy decision = bot.GenDecision(1);
    field->DebugPrint();
    SubmitAndDontExit(Action(decision.act_0), Action(decision.act_1));
    while (1) {
        ReadInput(std::cin, data, globaldata);
        bot.Play(std::make_pair(
            Policy(field->previousActions[field->currentTurn - 1][0][0],
                   field->previousActions[field->currentTurn - 1][0][1]),
            Policy(field->previousActions[field->currentTurn - 1][1][0],
                   field->previousActions[field->currentTurn - 1][1][1])));
        decision = bot.GenDecision(0);
        field->DebugPrint();
        SubmitAndDontExit(Action(decision.act_0), Action(decision.act_1));
    }
}
